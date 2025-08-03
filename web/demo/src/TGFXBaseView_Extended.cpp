/////////////////////////////////////////////////////////////////////////////////////////////////
//
// 扩展的TGFXBaseView实现 - 展示快速原型的底层逻辑
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "TGFXBaseView_Extended.h"
#include <cmath>

namespace hello2d {

TGFXExtendedView::TGFXExtendedView(const std::string& canvasID) : TGFXBaseView(canvasID) {
  // 初始化默认画笔
  defaultPaint.setColor(tgfx::Color::Black());
  defaultPaint.setAntiAlias(true);
}

// 🎯 核心：开始自定义绘制模式
bool TGFXExtendedView::beginCustomDraw() {
  if (appHost->width() <= 0 || appHost->height() <= 0) {
    return false;
  }
  
  // 获取WebGL窗口（复用原有逻辑）
  if (window == nullptr) {
    window = tgfx::WebGLWindow::MakeFrom(canvasID);
  }
  if (window == nullptr) {
    return false;
  }
  
  // 获取设备和上下文（复用原有逻辑）
  auto device = window->getDevice();
  currentContext = device->lockContext();
  if (currentContext == nullptr) {
    return false;
  }
  
  // 获取绘制表面（复用原有逻辑）
  currentSurface = window->getSurface(currentContext);
  if (currentSurface == nullptr) {
    device->unlock();
    return false;
  }
  
  // 🎯 关键：获取真正的TGFX Canvas对象
  currentCanvas = currentSurface->getCanvas();
  if (currentCanvas == nullptr) {
    device->unlock();
    return false;
  }
  
  isInCustomDrawMode = true;
  return true;
}

// 🎯 核心：结束自定义绘制并刷新到屏幕
bool TGFXExtendedView::endCustomDraw() {
  if (!isInCustomDrawMode || currentContext == nullptr) {
    return false;
  }
  
  // 🎯 关键：刷新TGFX渲染结果到WebGL
  currentContext->flushAndSubmit();
  window->present(currentContext);
  
  // 清理状态
  auto device = window->getDevice();
  device->unlock();
  
  currentCanvas = nullptr;
  currentSurface = nullptr;
  currentContext = nullptr;
  isInCustomDrawMode = false;
  
  return true;
}

// 🎯 直接调用TGFX API：清除画布
void TGFXExtendedView::clearCanvas(float r, float g, float b, float a) {
  if (currentCanvas) {
    tgfx::Color clearColor(r, g, b, a);
    currentCanvas->clear(clearColor);  // ✅ 真正的TGFX API调用！
  }
}

// 🎯 直接调用TGFX API：绘制矩形
void TGFXExtendedView::drawRect(float x, float y, float width, float height, float r, float g, float b, float a) {
  if (currentCanvas) {
    tgfx::Paint paint;
    paint.setColor(tgfx::Color(r, g, b, a));
    paint.setAntiAlias(true);
    
    tgfx::Rect rect = tgfx::Rect::MakeXYWH(x, y, width, height);
    currentCanvas->drawRect(rect, paint);  // ✅ 真正的TGFX API调用！
  }
}

// 🎯 直接调用TGFX API：绘制圆形
void TGFXExtendedView::drawCircle(float centerX, float centerY, float radius, float r, float g, float b, float a) {
  if (currentCanvas) {
    tgfx::Paint paint;
    paint.setColor(tgfx::Color(r, g, b, a));
    paint.setAntiAlias(true);
    
    tgfx::Point center = tgfx::Point::Make(centerX, centerY);
    currentCanvas->drawCircle(center, radius, paint);  // ✅ 真正的TGFX API调用！
  }
}

// 🎯 直接调用TGFX API：Canvas变换
void TGFXExtendedView::save() {
  if (currentCanvas) {
    currentCanvas->save();  // ✅ 真正的TGFX API调用！
  }
}

void TGFXExtendedView::restore() {
  if (currentCanvas) {
    currentCanvas->restore();  // ✅ 真正的TGFX API调用！
  }
}

void TGFXExtendedView::translate(float dx, float dy) {
  if (currentCanvas) {
    currentCanvas->translate(dx, dy);  // ✅ 真正的TGFX API调用！
  }
}

void TGFXExtendedView::rotate(float degrees) {
  if (currentCanvas) {
    currentCanvas->rotate(degrees);  // ✅ 真正的TGFX API调用！
  }
}

void TGFXExtendedView::scale(float sx, float sy) {
  if (currentCanvas) {
    currentCanvas->scale(sx, sy);  // ✅ 真正的TGFX API调用！
  }
}

// 🎯 Paint对象管理
int TGFXExtendedView::createPaint() {
  auto paint = std::make_unique<tgfx::Paint>();
  paint->setColor(tgfx::Color::Black());
  paint->setAntiAlias(true);
  
  paints.push_back(std::move(paint));
  return paints.size() - 1;  // 返回Paint的ID
}

void TGFXExtendedView::setPaintColor(int paintId, float r, float g, float b, float a) {
  if (paintId >= 0 && paintId < paints.size()) {
    paints[paintId]->setColor(tgfx::Color(r, g, b, a));  // ✅ 真正的TGFX API调用！
  }
}

void TGFXExtendedView::drawRectWithPaint(float x, float y, float width, float height, int paintId) {
  if (currentCanvas && paintId >= 0 && paintId < paints.size()) {
    tgfx::Rect rect = tgfx::Rect::MakeXYWH(x, y, width, height);
    currentCanvas->drawRect(rect, *paints[paintId]);  // ✅ 真正的TGFX API调用！
  }
}

// 🎯 Path对象管理
int TGFXExtendedView::createPath() {
  auto path = std::make_unique<tgfx::Path>();
  paths.push_back(std::move(path));
  return paths.size() - 1;  // 返回Path的ID
}

void TGFXExtendedView::pathMoveTo(int pathId, float x, float y) {
  if (pathId >= 0 && pathId < paths.size()) {
    paths[pathId]->moveTo(x, y);  // ✅ 真正的TGFX API调用！
  }
}

void TGFXExtendedView::pathLineTo(int pathId, float x, float y) {
  if (pathId >= 0 && pathId < paths.size()) {
    paths[pathId]->lineTo(x, y);  // ✅ 真正的TGFX API调用！
  }
}

void TGFXExtendedView::pathClose(int pathId) {
  if (pathId >= 0 && pathId < paths.size()) {
    paths[pathId]->close();  // ✅ 真正的TGFX API调用！
  }
}

void TGFXExtendedView::drawPath(int pathId, int paintId) {
  if (currentCanvas && 
      pathId >= 0 && pathId < paths.size() && 
      paintId >= 0 && paintId < paints.size()) {
    currentCanvas->drawPath(*paths[pathId], *paints[paintId]);  // ✅ 真正的TGFX API调用！
  }
}

} // namespace hello2d