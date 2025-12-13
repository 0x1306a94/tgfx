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

#include <tgfx/core/Stroke.h>
#include <tgfx/layers/Layer.h>
#include <tgfx/layers/TextAlign.h>

namespace tgfx {
class TextBlob;
};

namespace drawers {

class SVGTextLayer : public tgfx::Layer {
 public:
  static std::shared_ptr<SVGTextLayer> Make();

  virtual ~SVGTextLayer() override = default;

  tgfx::LayerType type() const override {
    return static_cast<tgfx::LayerType>(static_cast<int>(tgfx::LayerType::Text) + 100);
  }

  const std::shared_ptr<tgfx::TextBlob> textBlob() const {
    return _textBlob;
  }

  void setTextBlob(std::shared_ptr<tgfx::TextBlob> textBlob);

  const tgfx::Color& textColor() const {
    return _textColor;
  }

  void setTextColor(const tgfx::Color& color);

  const tgfx::Color& strokeColor() const {
    return _strokeColor;
  }

  void setStrokeColor(const tgfx::Color& color);

  tgfx::TextAlign textAlign() const {
    return _textAlign;
  }

  void setTextAlign(tgfx::TextAlign align);

  tgfx::LineCap lineCap() const {
    return _stroke.cap;
  }

  void setLineCap(tgfx::LineCap cap);

  tgfx::LineJoin lineJoin() const {
    return _stroke.join;
  }

  void setLineJoin(tgfx::LineJoin join);

  float miterLimit() const {
    return _stroke.miterLimit;
  }

  void setMiterLimit(float limit);

  float lineWidth() const {
    return _stroke.width;
  }

  void setLineWidth(float width);

 protected:
  SVGTextLayer() = default;

  float getAlignmentFactor() const;

  void onUpdateContent(tgfx::LayerRecorder* recorder) override;

 private:
  std::shared_ptr<tgfx::TextBlob> _textBlob{nullptr};
  tgfx::Color _textColor{tgfx::Color::Transparent()};
  tgfx::TextAlign _textAlign{tgfx::TextAlign::Left};
  tgfx::Stroke _stroke{};
  tgfx::Color _strokeColor{tgfx::Color::Transparent()};
};

}  // namespace drawers
#endif
