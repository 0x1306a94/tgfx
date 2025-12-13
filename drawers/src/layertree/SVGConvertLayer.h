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
#pragma once

#include <tgfx/svg/SVGLengthContext.h>
#include <memory>
#include <optional>
#include <tuple>

namespace tgfx {
class SVGDOM;
class SVGNode;
class SVGCircle;
class SVGEllipse;
class SVGPath;
class SVGPoly;
class SVGRect;
class SVGText;
class SVGGroup;
class Layer;
class ShapeLayer;
class TextLayer;
class Typeface;
};  // namespace tgfx

namespace drawers {
class SVGTextLayer;

using SVGConvertLayerResult = std::optional<std::tuple<std::shared_ptr<tgfx::Layer>, tgfx::Size>>;

SVGConvertLayerResult convertSVGDomToLayer(std::shared_ptr<tgfx::SVGDOM> dom,
                                           std::shared_ptr<tgfx::Typeface> defaultTypeface);

std::shared_ptr<tgfx::Layer> convertSVGNodeToLayer(tgfx::SVGNode* node,
                                                   const tgfx::SVGLengthContext& lengthContext,
                                                   std::shared_ptr<tgfx::Typeface> defaultTypeface);
std::shared_ptr<tgfx::Layer> convertGroup(tgfx::SVGGroup* node,
                                          const tgfx::SVGLengthContext& lengthContext,
                                          std::shared_ptr<tgfx::Typeface> defaultTypeface);
std::shared_ptr<tgfx::ShapeLayer> convertCircle(tgfx::SVGCircle* node,
                                                const tgfx::SVGLengthContext& lengthContext);
std::shared_ptr<tgfx::ShapeLayer> convertEllipse(tgfx::SVGEllipse* node,
                                                 const tgfx::SVGLengthContext& lengthContext);
std::shared_ptr<tgfx::ShapeLayer> convertPath(tgfx::SVGPath* node,
                                              const tgfx::SVGLengthContext& lengthContext);
std::shared_ptr<tgfx::ShapeLayer> convertPoly(tgfx::SVGPoly* node,
                                              const tgfx::SVGLengthContext& lengthContext);
std::shared_ptr<tgfx::ShapeLayer> convertRect(tgfx::SVGRect* node,
                                              const tgfx::SVGLengthContext& lengthContext);
std::shared_ptr<tgfx::TextLayer> convertText(tgfx::SVGText* node,
                                             const tgfx::SVGLengthContext& lengthContext,
                                             std::shared_ptr<tgfx::Typeface> defaultTypeface);

}  // namespace drawers
#endif
