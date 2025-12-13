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

#include "SVGLayerTree.h"
#include <tgfx/svg/SVGDOM.h>
#include "SVGConvertLayer.h"
namespace drawers {
std::shared_ptr<tgfx::Layer> SVGLayerTree::buildLayerTree(const AppHost* host) {
  auto root = tgfx::Layer::Make();
  auto data = host->getSVGData("default");
  if (!data) {
    return root;
  }

  auto stream = tgfx::Stream::MakeFromData(data);
  auto dom = tgfx::SVGDOM::Make(*stream);
  if (!dom) {
    return root;
  }

  auto typeface = host->getTypeface("default");
  auto result = convertSVGDomToLayer(dom, typeface);
  if (!result) {
    return root;
  }

  auto [layer, size] = result.value();
  (void)size;
  root->addChild(layer);
  return root;
}
}  // namespace drawers
#endif
