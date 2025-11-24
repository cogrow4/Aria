#include "brush.h"
#include <QtCore>

Brush::Brush(QObject *parent)
    : QObject(parent), m_size(10), m_color(Qt::black), m_opacity(100),
      m_hardness(100), m_tolerance(30), m_isEraser(false) {}

void Brush::setSize(int size) {
  m_size = qBound(1, size, 500);
  emit sizeChanged(m_size);
}

void Brush::setColor(const QColor &color) {
  m_color = color;
  emit colorChanged(m_color);
}

void Brush::setOpacity(int opacity) {
  m_opacity = qBound(0, opacity, 100);
  emit opacityChanged(m_opacity);
}

void Brush::setHardness(int hardness) {
  m_hardness = qBound(0, hardness, 100);
  emit hardnessChanged(m_hardness);
}

void Brush::setTolerance(int tolerance) {
  m_tolerance = qBound(0, tolerance, 255);
  emit toleranceChanged(m_tolerance);
}

void Brush::setEraser(bool eraser) { m_isEraser = eraser; }

void Brush::paint(QPainter &painter, const QPointF &start, const QPointF &end,
                  double pressure) {
  painter.setRenderHint(QPainter::Antialiasing, true);

  // Adjust size and opacity based on pressure
  double currentSize = m_size * pressure;
  if (currentSize < 1.0)
    currentSize = 1.0;

  double currentOpacity = m_opacity / 100.0;

  QColor paintColor = m_color;
  paintColor.setAlphaF(currentOpacity);

  if (m_isEraser) {
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    if (currentOpacity < 1.0) {
      painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
      paintColor = Qt::black;
      paintColor.setAlphaF(currentOpacity);
    }
  } else {
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  }

  QPen pen(paintColor, currentSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  painter.setPen(pen);
  painter.drawLine(start, end);

  // Reset composition mode
  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
}
