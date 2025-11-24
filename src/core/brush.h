#ifndef BRUSH_H
#define BRUSH_H

#include <QColor>
#include <QObject>
#include <QPainter>
#include <QPointF>

class Brush : public QObject {
  Q_OBJECT

public:
  Brush(QObject *parent = nullptr);

  void setSize(int size);
  int size() const { return m_size; }

  void setColor(const QColor &color);
  QColor color() const { return m_color; }

  void setOpacity(int opacity);
  int opacity() const { return m_opacity; }

  void setHardness(int hardness);
  int hardness() const { return m_hardness; }

  void setTolerance(int tolerance);
  int tolerance() const { return m_tolerance; }

  void setEraser(bool eraser);
  bool isEraser() const { return m_isEraser; }

  void paint(QPainter &painter, const QPointF &start, const QPointF &end,
             double pressure);

signals:
  void sizeChanged(int size);
  void colorChanged(QColor color);
  void opacityChanged(int opacity);
  void hardnessChanged(int hardness);
  void toleranceChanged(int tolerance);

private:
  int m_size;
  QColor m_color;
  int m_opacity;   // 0-100
  int m_hardness;  // 0-100
  int m_tolerance; // 0-255 for flood fill
  bool m_isEraser;
};

#endif // BRUSH_H
