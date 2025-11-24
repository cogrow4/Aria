#include "canvas.h"
#include "core/brush.h"

void Canvas::setBrush(const Brush &brush) { m_brush = brush; }

void Canvas::setTool(ToolType tool) {
  m_currentTool = tool;
  // Deactivate selection when switching tools
  if (tool != Canvas::RectSelectTool && tool != Canvas::EllipseSelectTool &&
      tool != Canvas::LassoTool) {
    m_selectionActive = false;
  }
}
