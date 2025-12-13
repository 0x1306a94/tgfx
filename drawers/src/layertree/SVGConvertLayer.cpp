/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Tencent is pleased to support the open source community by making tgfx available.
//
//  Copyright (C) 2025 Tencent. All rights reserved.
//
//  Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
//  in compliance with the License. You may obtain a copy of the License at
//
//      https://opensource.org/licenses/BSD-3-Clause
//
//  unless required by applicable law or agreed to in writing, software distributed under the
//  license is distributed on an "as is" basis, without warranties or conditions of any kind,
//  either express or implied. see the license for the specific language governing permissions
//  and limitations under the license.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TGFX_BUILD_SVG

#include "SVGConvertLayer.h"
#include <tgfx/core/Path.h>
#include <tgfx/core/Rect.h>
#include <tgfx/layers/DisplayList.h>
#include <tgfx/layers/ImageLayer.h>
#include <tgfx/layers/ShapeLayer.h>
#include <tgfx/layers/SolidColor.h>
#include <tgfx/layers/TextLayer.h>
#include <tgfx/svg/SVGDOM.h>
#include <tgfx/svg/SVGLengthContext.h>
#include <tgfx/svg/node/SVGCircle.h>
#include <tgfx/svg/node/SVGEllipse.h>
#include <tgfx/svg/node/SVGGroup.h>
#include <tgfx/svg/node/SVGPath.h>
#include <tgfx/svg/node/SVGPoly.h>
#include <tgfx/svg/node/SVGRect.h>
#include <tgfx/svg/node/SVGText.h>
#include "SVGTextLayer.h"

namespace drawers {

static std::shared_ptr<tgfx::Typeface> resolveTypeface(const tgfx::SVGText* node) {
  auto weight = [](const tgfx::SVGFontWeight& w) {
    switch (w.type()) {
      case tgfx::SVGFontWeight::Type::W100:
        return tgfx::FontWeight::Thin;
      case tgfx::SVGFontWeight::Type::W200:
        return tgfx::FontWeight::ExtraLight;
      case tgfx::SVGFontWeight::Type::W300:
        return tgfx::FontWeight::Light;
      case tgfx::SVGFontWeight::Type::W400:
        return tgfx::FontWeight::Normal;
      case tgfx::SVGFontWeight::Type::W500:
        return tgfx::FontWeight::Medium;
      case tgfx::SVGFontWeight::Type::W600:
        return tgfx::FontWeight::SemiBold;
      case tgfx::SVGFontWeight::Type::W700:
        return tgfx::FontWeight::Bold;
      case tgfx::SVGFontWeight::Type::W800:
        return tgfx::FontWeight::ExtraBold;
      case tgfx::SVGFontWeight::Type::W900:
        return tgfx::FontWeight::Black;
      case tgfx::SVGFontWeight::Type::Normal:
        return tgfx::FontWeight::Normal;
      case tgfx::SVGFontWeight::Type::Bold:
        return tgfx::FontWeight::Bold;
      case tgfx::SVGFontWeight::Type::Bolder:
        return tgfx::FontWeight::ExtraBold;
      case tgfx::SVGFontWeight::Type::Lighter:
        return tgfx::FontWeight::Light;
      case tgfx::SVGFontWeight::Type::Inherit: {
        return tgfx::FontWeight::Normal;
      }
    }
  };

  auto slant = [](const tgfx::SVGFontStyle& s) {
    switch (s.type()) {
      case tgfx::SVGFontStyle::Type::Normal:
        return tgfx::FontSlant::Upright;
      case tgfx::SVGFontStyle::Type::Italic:
        return tgfx::FontSlant::Italic;
      case tgfx::SVGFontStyle::Type::Oblique:
        return tgfx::FontSlant::Oblique;
      case tgfx::SVGFontStyle::Type::Inherit: {
        return tgfx::FontSlant::Upright;
      }
    }
  };

  auto fontFamily = node->getFontFamily().get();
  if (!fontFamily) {
    return nullptr;
  }
  const auto& family = fontFamily->family();
  const auto fontWeight =
      node->getFontWeight().get().value_or(tgfx::SVGFontWeight(tgfx::SVGFontWeight::Type::Normal));
  const auto fontStyle =
      node->getFontStyle().get().value_or(tgfx::SVGFontStyle(tgfx::SVGFontStyle::Type::Normal));

  const tgfx::FontStyle style(weight(fontWeight), tgfx::FontWidth::Normal, slant(fontStyle));
  return tgfx::Typeface::MakeFromName(family, style);
}

static std::vector<float> resolveTextLengths(const tgfx::SVGLengthContext& lengthContext,
                                             const std::vector<tgfx::SVGLength>& lengths,
                                             tgfx::SVGLengthContext::LengthType lengthType,
                                             const tgfx::SVGFontSize& fontSize) {
  std::vector<float> resolved;
  resolved.reserve(lengths.size());

  for (const auto& length : lengths) {
    if (length.unit() == tgfx::SVGLength::Unit::EMS ||
        length.unit() == tgfx::SVGLength::Unit::EXS) {
      auto resolvedLength =
          lengthContext.resolve(fontSize.size(), tgfx::SVGLengthContext::LengthType::Horizontal) *
          length.value();
      resolved.push_back(resolvedLength);
    } else {
      resolved.push_back(lengthContext.resolve(length, lengthType));
    }
  }

  return resolved;
}

static void applyShapeLayerStyle(tgfx::ShapeLayer* shape, tgfx::SVGNode* node,
                                 const tgfx::SVGLengthContext& lengthContext) {

  (void)lengthContext;
  auto hasStroke = false;
  if (const auto& attribute = node->getStroke().get();
      attribute && attribute->type() == tgfx::SVGPaint::Type::Color) {
    auto color = attribute->color().color();
    shape->addStrokeStyle(tgfx::SolidColor::Make(color));
    hasStroke = true;
  }

  if (const auto& attribute = node->getStrokeWidth().get(); attribute && hasStroke) {
    auto width = attribute.value().value();
    hasStroke = !(width == 0.0f);
    shape->setLineWidth(width);
  }

  if (const auto& attribute = node->getFill().get();
      attribute && attribute->type() == tgfx::SVGPaint::Type::Color) {
    auto color = attribute->color().color();
    shape->addFillStyle(tgfx::SolidColor::Make(color));
  } else if (!hasStroke) {
    shape->addFillStyle(tgfx::SolidColor::Make(tgfx::Color::Black()));
  }

  if (const auto& attribute = node->getStrokeDashArray().get(); attribute) {
    std::vector<float> dash{};
    for (const auto& item : attribute.value().dashArray()) {
      dash.push_back(item.value());
    }
    shape->setLineDashPattern(dash);
  }

  if (const auto& attribute = node->getStrokeDashOffset().get(); attribute) {
    shape->setLineDashPhase(attribute.value().value());
  }

  if (const auto& attribute = node->getStrokeLineCap().get(); attribute) {
    switch (attribute.value()) {
      case tgfx::SVGLineCap::Butt:
        shape->setLineCap(tgfx::LineCap::Butt);
        break;
      case tgfx::SVGLineCap::Round:
        shape->setLineCap(tgfx::LineCap::Round);
        break;
      case tgfx::SVGLineCap::Square:
        shape->setLineCap(tgfx::LineCap::Square);
        break;
      default:
        break;
    }
  }

  if (const auto& attribute = node->getStrokeLineJoin().get(); attribute) {
    switch (attribute.value().type()) {
      case tgfx::SVGLineJoin::Type::Miter:
        shape->setLineJoin(tgfx::LineJoin::Miter);
        break;
      case tgfx::SVGLineJoin::Type::Round:
        shape->setLineJoin(tgfx::LineJoin::Round);
        break;
      case tgfx::SVGLineJoin::Type::Bevel:
        shape->setLineJoin(tgfx::LineJoin::Bevel);
        break;
      default:
        break;
    }
  }

  if (const auto& attribute = node->getStrokeMiterLimit().get(); attribute) {
    shape->setMiterLimit(attribute.value());
  }
}

static void applyTextLayerStyle(tgfx::TextLayer* textLayer, tgfx::SVGText* node,
                                const tgfx::SVGLengthContext& lengthContext) {

  (void)lengthContext;
  auto hasStroke = false;
  //  if (const auto& attribute = node->getStroke().get();
  //      attribute && attribute->type() == tgfx::SVGPaint::Type::Color) {
  //    auto color = attribute->color().color();
  //    textLayer->setStrokeColor(color);
  //    hasStroke = true;
  //  }
  //
  //  if (const auto& attribute = node->getStrokeWidth().get(); attribute && hasStroke) {
  //    auto width = attribute.value().value();
  //    hasStroke = !(width == 0.0f);
  //    textLayer->setLineWidth(attribute.value().value());
  //  }

  if (const auto& attribute = node->getFill().get();
      attribute && attribute->type() == tgfx::SVGPaint::Type::Color) {
    auto color = attribute->color().color();
    textLayer->setTextColor(color);
  } else if (!hasStroke) {
    // 默认黑色
    textLayer->setTextColor(tgfx::Color::Black());
  }

  if (const auto& attribute = node->getTextAnchor().get(); attribute) {
    auto type = attribute.value().type();
    switch (type) {
      case tgfx::SVGTextAnchor::Type::Start:
        textLayer->setTextAlign(tgfx::TextAlign::Left);
        break;
      case tgfx::SVGTextAnchor::Type::Middle:
        textLayer->setTextAlign(tgfx::TextAlign::Center);
        break;
      case tgfx::SVGTextAnchor::Type::End:
        textLayer->setTextAlign(tgfx::TextAlign::Right);
        break;
      default:
        break;
    }
  }

  //  if (const auto& attribute = node->getStrokeLineCap().get(); attribute) {
  //    switch (attribute.value()) {
  //      case tgfx::SVGLineCap::Butt:
  //        textLayer->setLineCap(tgfx::LineCap::Butt);
  //        break;
  //      case tgfx::SVGLineCap::Round:
  //        textLayer->setLineCap(tgfx::LineCap::Round);
  //        break;
  //      case tgfx::SVGLineCap::Square:
  //        textLayer->setLineCap(tgfx::LineCap::Square);
  //        break;
  //      default:
  //        break;
  //    }
  //  }
  //
  //  if (const auto& attribute = node->getStrokeLineJoin().get(); attribute) {
  //    switch (attribute.value().type()) {
  //      case tgfx::SVGLineJoin::Type::Miter:
  //        textLayer->setLineJoin(tgfx::LineJoin::Miter);
  //        break;
  //      case tgfx::SVGLineJoin::Type::Round:
  //        textLayer->setLineJoin(tgfx::LineJoin::Round);
  //        break;
  //      case tgfx::SVGLineJoin::Type::Bevel:
  //        textLayer->setLineJoin(tgfx::LineJoin::Bevel);
  //        break;
  //      default:
  //        break;
  //    }
  //  }
  //
  //  if (const auto& attribute = node->getStrokeMiterLimit().get(); attribute) {
  //    textLayer->setMiterLimit(attribute.value());
  //  }
}

SVGConvertLayerResult convertSVGDomToLayer(std::shared_ptr<tgfx::SVGDOM> dom,
                                           std::shared_ptr<tgfx::Typeface> defaultTypeface) {
  if (dom == nullptr) {
    return std::nullopt;
  }

  auto& rootNode = dom->getRoot();
  if (!rootNode->hasChildren()) {
    return std::nullopt;
  }

  auto rootWidth = rootNode->getWidth();
  auto rootHeight = rootNode->getHeight();

  tgfx::SVGLengthContext viewportLengthContext(tgfx::Size::Make(100, 100));

  tgfx::Size containerSize{};

  if (rootNode->getViewBox().has_value()) {
    viewportLengthContext = tgfx::SVGLengthContext(rootNode->getViewBox()->size());
    containerSize = tgfx::Size::Make(
        viewportLengthContext.resolve(rootWidth, tgfx::SVGLengthContext::LengthType::Horizontal),
        viewportLengthContext.resolve(rootHeight, tgfx::SVGLengthContext::LengthType::Vertical));
  } else {
    containerSize = tgfx::Size::Make(
        viewportLengthContext.resolve(rootWidth, tgfx::SVGLengthContext::LengthType::Horizontal),
        viewportLengthContext.resolve(rootHeight, tgfx::SVGLengthContext::LengthType::Vertical));
  }

  tgfx::Path path;
  path.addRect(tgfx::Rect::MakeWH(containerSize.width, containerSize.height));

  auto container = tgfx::ShapeLayer::Make();
  container->setPath(path);

  auto& childrens = rootNode->getChildren();
  for (const auto& child : childrens) {
    auto layer = convertSVGNodeToLayer(child.get(), viewportLengthContext, defaultTypeface);
    if (layer) {
      container->addChild(layer);
    }
  }

  return {{std::move(container), containerSize}};
}

std::shared_ptr<tgfx::Layer> convertSVGNodeToLayer(
    tgfx::SVGNode* node, const tgfx::SVGLengthContext& lengthContext,
    std::shared_ptr<tgfx::Typeface> defaultTypeface) {

  auto tag = node->tag();
  switch (tag) {
    case tgfx::SVGTag::G:
      return convertGroup(static_cast<tgfx::SVGGroup*>(node), lengthContext, defaultTypeface);
    case tgfx::SVGTag::Circle:
      return convertCircle(static_cast<tgfx::SVGCircle*>(node), lengthContext);
    case tgfx::SVGTag::Ellipse:
      return convertEllipse(static_cast<tgfx::SVGEllipse*>(node), lengthContext);
    case tgfx::SVGTag::Rect:
      return convertRect(static_cast<tgfx::SVGRect*>(node), lengthContext);
    case tgfx::SVGTag::Path:
      return convertPath(static_cast<tgfx::SVGPath*>(node), lengthContext);
    case tgfx::SVGTag::Polygon:
    case tgfx::SVGTag::Polyline:
      return convertPoly(static_cast<tgfx::SVGPoly*>(node), lengthContext);
    case tgfx::SVGTag::Text:
      return convertText(static_cast<tgfx::SVGText*>(node), lengthContext, defaultTypeface);
    default:
      break;
  }

  return nullptr;
}

std::shared_ptr<tgfx::Layer> convertGroup(tgfx::SVGGroup* node,
                                          const tgfx::SVGLengthContext& lengthContext,
                                          std::shared_ptr<tgfx::Typeface> defaultTypeface) {
  if (!node->hasChildren()) {
    return nullptr;
  }
  auto root = tgfx::Layer::Make();
  for (auto const& child : node->getChildren()) {
    auto layer = convertSVGNodeToLayer(child.get(), lengthContext, defaultTypeface);
    if (layer) {
      root->addChild(layer);
    }
  }
  return root;
}

std::shared_ptr<tgfx::ShapeLayer> convertCircle(tgfx::SVGCircle* node,
                                                const tgfx::SVGLengthContext& lengthContext) {
  const auto cx =
      lengthContext.resolve(node->getCx(), tgfx::SVGLengthContext::LengthType::Horizontal);
  const auto cy =
      lengthContext.resolve(node->getCy(), tgfx::SVGLengthContext::LengthType::Vertical);
  const auto r = lengthContext.resolve(node->getR(), tgfx::SVGLengthContext::LengthType::Other);
  tgfx::Path path;
  path.addOval(tgfx::Rect::MakeXYWH(cx - r, cy - r, 2 * r, 2 * r));
  path.transform(node->getTransform());

  auto shape = tgfx::ShapeLayer::Make();
  shape->setPath(path);
  applyShapeLayerStyle(shape.get(), node, lengthContext);

  return shape;
}

std::shared_ptr<tgfx::ShapeLayer> convertEllipse(tgfx::SVGEllipse* node,
                                                 const tgfx::SVGLengthContext& lengthContext) {
  const auto cx =
      lengthContext.resolve(node->getCx(), tgfx::SVGLengthContext::LengthType::Horizontal);
  const auto cy =
      lengthContext.resolve(node->getCy(), tgfx::SVGLengthContext::LengthType::Vertical);
  const auto [rx, ry] = lengthContext.resolveOptionalRadii(node->getRx(), node->getRy());

  if (rx <= 0 || ry <= 0) {
    return nullptr;
  }

  tgfx::Path path{};
  path.addOval(tgfx::Rect::MakeXYWH(cx - rx, cy - ry, rx * 2, ry * 2));
  path.transform(node->getTransform());

  auto shape = tgfx::ShapeLayer::Make();
  shape->setPath(path);
  applyShapeLayerStyle(shape.get(), node, lengthContext);

  return shape;
}

std::shared_ptr<tgfx::ShapeLayer> convertPath(tgfx::SVGPath* node,
                                              const tgfx::SVGLengthContext& lengthContext) {
  auto shape = tgfx::ShapeLayer::Make();

  auto path = node->getShapePath();
  auto clipRule = node->getClipRule().get();
  if (clipRule) {
    path.setFillType(clipRule->asFillType());
  }
  shape->setPath(std::move(path));
  shape->setMatrix(node->getTransform());
  applyShapeLayerStyle(shape.get(), node, lengthContext);

  return shape;
}

std::shared_ptr<tgfx::ShapeLayer> convertPoly(tgfx::SVGPoly* node,
                                              const tgfx::SVGLengthContext& lengthContext) {
  auto points = node->getPoints();
  if (points.empty()) {
    return nullptr;
  }
  auto shape = tgfx::ShapeLayer::Make();
  tgfx::Path path{};
  path.moveTo(points[0]);
  for (uint32_t i = 1; i < points.size(); i++) {
    path.lineTo(points[i]);
  }
  path.close();
  path.transform(node->getTransform());

  auto clipRule = node->getClipRule().get();
  if (clipRule) {
    path.setFillType(clipRule->asFillType());
  }

  shape->setPath(path);

  applyShapeLayerStyle(shape.get(), node, lengthContext);

  return shape;
}

std::shared_ptr<tgfx::ShapeLayer> convertRect(tgfx::SVGRect* node,
                                              const tgfx::SVGLengthContext& lengthContext) {

  auto shape = tgfx::ShapeLayer::Make();
  const auto rect =
      lengthContext.resolveRect(node->getX(), node->getY(), node->getWidth(), node->getHeight());
  const auto [rx, ry] = lengthContext.resolveOptionalRadii(node->getRx(), node->getRy());

  tgfx::RRect rrect;
  rrect.setRectXY(rect, std::min(rx, rect.width() / 2), std::min(ry, rect.height() / 2));

  tgfx::Path path{};
  path.addRRect(rrect);
  path.transform(node->getTransform());

  shape->setPath(path);

  applyShapeLayerStyle(shape.get(), node, lengthContext);

  return shape;
}

std::shared_ptr<tgfx::TextLayer> convertText(tgfx::SVGText* node,
                                             const tgfx::SVGLengthContext& lengthContext,
                                             std::shared_ptr<tgfx::Typeface> defaultTypeface) {

  if (!defaultTypeface) {
    return nullptr;
  }

  // 暂时只支持单个文本节点
  std::shared_ptr<tgfx::SVGTextLiteral> literal = nullptr;
  for (const auto& child : node->getTextChildren()) {
    if (child->tag() == tgfx::SVGTag::TextLiteral) {
      literal = std::static_pointer_cast<tgfx::SVGTextLiteral>(child);
      break;
    }
  }

  if (literal == nullptr) {
    return nullptr;
  }

  auto text = literal->getText();
  if (text.empty()) {
    return nullptr;
  }

  auto typeface = resolveTypeface(node);

  auto fontSize = tgfx::SVGFontSize(tgfx::SVGLength(10.0f, tgfx::SVGLength::Unit::PT));
  if (auto attribute = node->getFontSize().get(); attribute) {
    fontSize = attribute.value();
  }

  auto x = resolveTextLengths(lengthContext, node->getX(),
                              tgfx::SVGLengthContext::LengthType::Horizontal, fontSize);
  auto y = resolveTextLengths(lengthContext, node->getY(),
                              tgfx::SVGLengthContext::LengthType::Vertical, fontSize);
  auto dx = resolveTextLengths(lengthContext, node->getDx(),
                               tgfx::SVGLengthContext::LengthType::Horizontal, fontSize);
  auto dy = resolveTextLengths(lengthContext, node->getDy(),
                               tgfx::SVGLengthContext::LengthType::Vertical, fontSize);

  if (x.empty() || y.empty()) {
    return nullptr;
  }

  auto finalFontSize =
      lengthContext.resolve(fontSize.size(), tgfx::SVGLengthContext::LengthType::Vertical);
  tgfx::Font font(typeface, finalFontSize);

  auto offsetX = x[0] + (dx.empty() ? 0.0f : dx[0]);
  auto offsetY = y[0] + (dy.empty() ? 0.0f : dy[0]);

  //  auto shaper = tgfx::TextShaper::Make({defaultTypeface});
  //  if (shaper == nullptr) {
  //    return nullptr;
  //  }
  //  auto textBlob = shaper->shape(text, typeface, finalFontSize);
  //  if (textBlob == nullptr) {
  //    return nullptr;
  //  }

  auto matrix = tgfx::Matrix::MakeTrans(offsetX, offsetY);
  matrix.postConcat(node->getTransform());
  auto layer = tgfx::TextLayer::Make();
  layer->setMatrix(matrix);
  //  layer->setTextBlob(std::move(textBlob));
  layer->setText(text);
  layer->setFont(tgfx::Font(defaultTypeface, finalFontSize));

  applyTextLayerStyle(layer.get(), node, lengthContext);

  return layer;
}

}  // namespace drawers
#endif
