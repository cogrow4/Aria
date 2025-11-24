#ifndef CANVAS_H
#define CANVAS_H

#include "core/brush.h"
#include "core/layermanager.h"
#include <QColor>
#include <QImage>
#include <QPainter>
#include <QPointF>
#include <QRect>
#include <QWidget>
#include <memory>

class Brush;
class Canvas : public QWidget {
  Q_OBJECT

public:
  enum ToolType {
    BrushTool,
    EraserTool,
    EyedropperTool,
    RectSelectTool,
    EllipseSelectTool,
    LassoTool,
    FillBucketTool,
    LineTool,
    TextTool
  };

  explicit Canvas(QWidget *parent = nullptr);
  ~Canvas();

  void setBrush(const Brush &brush);
  Brush *brush() { return &m_brush; }

  void setTool(ToolType tool);
  ToolType currentTool() const { return m_currentTool; }

  void newImage(const QSize &size, const QColor &color = Qt::white);
  void newImage(int width, int height, const QColor &color = Qt::white);

  LayerManager *layerManager() { return &m_layerManager; }

signals:
  void colorPicked(QColor color);

protected:
  // Event handlers
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

  // Tablet support
  void tabletEvent(QTabletEvent *event) override;

private:
  void drawLineTo(const QPointF &endPoint, double pressure);
  void resizeImage(QImage *image, const QSize &newSize);
  void floodFill(const QPoint &startPoint, const QColor &fillColor);

  QImage m_image; // This will now act as the composited cache
  QPointF m_lastPoint;
  bool m_drawing;

  ToolType m_currentTool;

  bool m_selectionActive;
  QRect m_selectionRect;
  QRegion m_selectionRegion; // For complex selections later
  QPolygon m_lassoPath;      // For lasso selection

  Brush m_brush;
  LayerManager m_layerManager;
};

#endif // CANVAS_H
