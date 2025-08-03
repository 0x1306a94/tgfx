/////////////////////////////////////////////////////////////////////////////////////////////////
//
// 扩展的TGFXBaseView - 支持直接TGFX API调用
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TGFXBaseView.h"
#include "tgfx/core/Canvas.h"
#include "tgfx/core/Paint.h"
#include "tgfx/core/Color.h"
#include "tgfx/core/Point.h"
#include "tgfx/core/Rect.h"
#include "tgfx/core/Path.h"

namespace hello2d {

class TGFXExtendedView : public TGFXBaseView {
 public:
  TGFXExtendedView(const std::string& canvasID);

  // 🎯 核心：开始/结束自定义绘制模式
  bool beginCustomDraw();
  bool endCustomDraw();

  // 🎯 直接的TGFX Canvas API绑定
  void clearCanvas(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
  void drawRect(float x, float y, float width, float height, float r, float g, float b, float a = 1.0f);
  void drawCircle(float centerX, float centerY, float radius, float r, float g, float b, float a = 1.0f);
  void drawLine(float x1, float y1, float x2, float y2, float strokeWidth, float r, float g, float b, float a = 1.0f);
  
  // 🎯 Canvas变换API
  void save();
  void restore();
  void translate(float dx, float dy);
  void rotate(float degrees);
  void scale(float sx, float sy);
  
  // 🎯 Paint管理API
  int createPaint();
  void setPaintColor(int paintId, float r, float g, float b, float a = 1.0f);
  void setPaintStrokeWidth(int paintId, float width);
  void drawRectWithPaint(float x, float y, float width, float height, int paintId);
  
  // 🎯 Path API
  int createPath();
  void pathMoveTo(int pathId, float x, float y);
  void pathLineTo(int pathId, float x, float y);
  void pathAddRect(int pathId, float x, float y, float width, float height);
  void pathClose(int pathId);
  void drawPath(int pathId, int paintId);

 private:
  // 当前的绘制状态
  tgfx::Canvas* currentCanvas = nullptr;
  std::shared_ptr<tgfx::Surface> currentSurface = nullptr;
  std::shared_ptr<tgfx::Context> currentContext = nullptr;
  
  // Paint和Path对象管理
  std::vector<std::unique_ptr<tgfx::Paint>> paints;
  std::vector<std::unique_ptr<tgfx::Path>> paths;
  
  // 当前画笔状态
  tgfx::Paint defaultPaint;
  
  bool isInCustomDrawMode = false;
};

} // namespace hello2d