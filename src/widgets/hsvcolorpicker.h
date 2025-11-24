#ifndef HSVCOLORPICKER_H
#define HSVCOLORPICKER_H

#include <QColor>
#include <QWidget>

class HSVColorPicker : public QWidget {
  Q_OBJECT

public:
  explicit HSVColorPicker(QWidget *parent = nullptr);
  ~HSVColorPicker();

  QColor currentColor() const { return m_currentColor; }
  void setColor(const QColor &color);

signals:
  void colorChanged(const QColor &color);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

private:
  void updateGeometry();
  void drawHueCircle(QPainter &painter);
  void drawSVTriangle(QPainter &painter);
  void drawSelectors(QPainter &painter);

  QPointF hueToCirclePoint(float hue) const;
  float circlePointToHue(const QPointF &point) const;
  QPointF svToTrianglePoint(float sat, float val) const;
  void trianglePointToSV(const QPointF &point, float &sat, float &val) const;

  bool isInHueCircle(const QPointF &point) const;
  bool isInSVTriangle(const QPointF &point) const;

  // HSV/RGB conversion utilities
  static QColor hsvToRgb(float h, float s, float v);
  static void rgbToHsv(const QColor &rgb, float &h, float &s, float &v);

  QColor m_currentColor;
  float m_hue;        // 0.0 - 360.0
  float m_saturation; // 0.0 - 1.0
  float m_value;      // 0.0 - 1.0

  // Geometry
  QPointF m_center;
  float m_outerRadius;
  float m_innerRadius;
  QPointF m_trianglePoints[3];

  // Interaction state
  enum DragMode { None, DraggingHue, DraggingSV };
  DragMode m_dragMode;
};

#endif // HSVCOLORPICKER_H
