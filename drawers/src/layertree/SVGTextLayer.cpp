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

#include "SVGTextLayer.h"
#include <tgfx/core/Canvas.h>
#include <tgfx/core/TextBlob.h>
#include <tgfx/layers/LayerRecorder.h>

namespace drawers {

std::shared_ptr<SVGTextLayer> SVGTextLayer::Make() {
  return std::shared_ptr<SVGTextLayer>(new SVGTextLayer());
}

void SVGTextLayer::setTextBlob(std::shared_ptr<tgfx::TextBlob> textBlob) {
  if (_textBlob == textBlob) {
    return;
  }
  _textBlob = textBlob;
  invalidateContent();
}

void SVGTextLayer::setTextColor(const tgfx::Color& color) {
  if (_textColor == color) {
    return;
  }
  _textColor = color;
  invalidateContent();
}

void SVGTextLayer::setStrokeColor(const tgfx::Color& color) {
  if (_strokeColor == color) {
    return;
  }
  _strokeColor = color;
  invalidateContent();
}

void SVGTextLayer::setTextAlign(tgfx::TextAlign align) {
  if (_textAlign == align) {
    return;
  }
  _textAlign = align;
  invalidateContent();
}

void SVGTextLayer::setLineCap(tgfx::LineCap cap) {
  if (_stroke.cap == cap) {
    return;
  }
  _stroke.cap = cap;
  invalidateContent();
}

void SVGTextLayer::setLineJoin(tgfx::LineJoin join) {
  if (_stroke.join == join) {
    return;
  }
  _stroke.join = join;
  invalidateContent();
}

void SVGTextLayer::setMiterLimit(float limit) {
  if (_stroke.miterLimit == limit) {
    return;
  }
  _stroke.miterLimit = limit;
  invalidateContent();
}

void SVGTextLayer::setLineWidth(float width) {
  if (_stroke.width == width) {
    return;
  }
  _stroke.width = width;
  invalidateContent();
}

float SVGTextLayer::getAlignmentFactor() const {
  switch (_textAlign) {
    case tgfx::TextAlign::Left:
      return 0.0f;
    case tgfx::TextAlign::Center:
      return -0.5f;
    case tgfx::TextAlign::Right:
      return -1.0f;
    case tgfx::TextAlign::Justify:
      return 0.0f;
  }
}

void SVGTextLayer::onUpdateContent(tgfx::LayerRecorder* recorder) {
  (void)recorder;
  //  if (_textBlob == nullptr) {
  //    return;
  //  }
  //
  //  auto bounds = _textBlob->getTightBounds();
  //  auto x = getAlignmentFactor() * bounds.width();
  //  auto y = 0.0f;
  //
  //  auto canvas = recorder->getCanvas();
  //  if (_textColor != tgfx::Color::Transparent() && _textColor.alpha != 0.0f) {
  //    tgfx::Paint paint = {};
  //    paint.setColor(_textColor);
  //    paint.setAntiAlias(true);
  //    canvas->drawTextBlob(_textBlob, x, y, paint);
  //  }
  //
  //  if (_strokeColor != tgfx::Color::Transparent() && _strokeColor.alpha != 0.0f &&
  //      _stroke.width != 0.0f) {
  //    tgfx::Paint paint = {};
  //    paint.setStyle(tgfx::PaintStyle::Stroke);
  //    paint.setColor(_strokeColor);
  //    paint.setStroke(_stroke);
  //    canvas->drawTextBlob(_textBlob, x, y, paint);
  //  }
}

}  // namespace drawers
#endif
