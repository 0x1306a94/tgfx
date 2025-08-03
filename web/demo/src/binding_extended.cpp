/////////////////////////////////////////////////////////////////////////////////////////////////
//
// 扩展的JavaScript绑定 - 暴露真正的TGFX API
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <emscripten/bind.h>
#include "TGFXBaseView_Extended.h"

using namespace hello2d;
using namespace emscripten;

EMSCRIPTEN_BINDINGS(TGFXExtendedAPI) {
  // 原有的基础绑定
  class_<TGFXBaseView>("TGFXBaseView")
      .function("setImagePath", &TGFXBaseView::setImagePath)
      .function("updateSize", &TGFXBaseView::updateSize)
      .function("draw", &TGFXBaseView::draw);

  // 🎯 新增：扩展的TGFX API绑定
  class_<TGFXExtendedView, base<TGFXBaseView>>("TGFXExtendedView")
      .smart_ptr<std::shared_ptr<TGFXExtendedView>>("TGFXExtendedView")
      .class_function("MakeFrom", optional_override([](const std::string& canvasID) {
                        if (canvasID.empty()) {
                          return std::shared_ptr<TGFXExtendedView>(nullptr);
                        }
                        return std::make_shared<TGFXExtendedView>(canvasID);
                      }))
      
      // 🎯 绘制会话管理
      .function("beginCustomDraw", &TGFXExtendedView::beginCustomDraw)
      .function("endCustomDraw", &TGFXExtendedView::endCustomDraw)
      
      // 🎯 基础绘制API
      .function("clearCanvas", &TGFXExtendedView::clearCanvas)
      .function("drawRect", &TGFXExtendedView::drawRect)
      .function("drawCircle", &TGFXExtendedView::drawCircle)
      .function("drawLine", &TGFXExtendedView::drawLine)
      
      // 🎯 Canvas变换API
      .function("save", &TGFXExtendedView::save)
      .function("restore", &TGFXExtendedView::restore)
      .function("translate", &TGFXExtendedView::translate)
      .function("rotate", &TGFXExtendedView::rotate)
      .function("scale", &TGFXExtendedView::scale)
      
      // 🎯 Paint管理API
      .function("createPaint", &TGFXExtendedView::createPaint)
      .function("setPaintColor", &TGFXExtendedView::setPaintColor)
      .function("setPaintStrokeWidth", &TGFXExtendedView::setPaintStrokeWidth)
      .function("drawRectWithPaint", &TGFXExtendedView::drawRectWithPaint)
      
      // 🎯 Path管理API
      .function("createPath", &TGFXExtendedView::createPath)
      .function("pathMoveTo", &TGFXExtendedView::pathMoveTo)
      .function("pathLineTo", &TGFXExtendedView::pathLineTo)
      .function("pathAddRect", &TGFXExtendedView::pathAddRect)
      .function("pathClose", &TGFXExtendedView::pathClose)
      .function("drawPath", &TGFXExtendedView::drawPath);
}