#include "hsvcolorpicker.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QtMath>

HSVColorPicker::HSVColorPicker(QWidget *parent)
    : QWidget(parent), m_hue(0.0f), m_saturation(1.0f), m_value(1.0f),
      m_dragMode(None) {
  setMinimumSize(200, 200);
  m_currentColor = hsvToRgb(m_hue, m_saturation, m_value);
  updateGeometry();
}

HSVColorPicker::~HSVColorPicker() {}

void HSVColorPicker::setColor(const QColor &color) {
  rgbToHsv(color, m_hue, m_saturation, m_value);
  m_currentColor = color;
  update();
  emit colorChanged(m_currentColor);
}

void HSVColorPicker::updateGeometry() {
  int size = qMin(width(), height());
  m_center = QPointF(width() / 2.0, height() / 2.0);
  m_outerRadius = size / 2.0 - 10.0;
  m_innerRadius = m_outerRadius * 0.75;

  // Calculate triangle points (equilateral triangle rotated with hue)
  float angleOffset = qDegreesToRadians(m_hue - 90.0);
  for (int i = 0; i < 3; ++i) {
    float angle = angleOffset + i * 2.0 * M_PI / 3.0;
    m_trianglePoints[i] = m_center + QPointF(m_innerRadius * qCos(angle),
                                             m_innerRadius * qSin(angle));
  }
}

void HSVColorPicker::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  drawHueCircle(painter);
  drawSVTriangle(painter);
  drawSelectors(painter);
}

void HSVColorPicker::drawHueCircle(QPainter &painter) {
  painter.save();

  // Draw hue ring
  int segments = 360;
  float angleStep = 360.0 / segments;

  for (int i = 0; i < segments; ++i) {
    float angle1 = qDegreesToRadians(i * angleStep - 90.0);
    float angle2 = qDegreesToRadians((i + 1) * angleStep - 90.0);

    QColor color = hsvToRgb(i * angleStep, 1.0, 1.0);

    QPointF p1_outer = m_center + QPointF(m_outerRadius * qCos(angle1),
                                          m_outerRadius * qSin(angle1));
    QPointF p2_outer = m_center + QPointF(m_outerRadius * qCos(angle2),
                                          m_outerRadius * qSin(angle2));
    QPointF p1_inner = m_center + QPointF(m_innerRadius * qCos(angle1),
                                          m_innerRadius * qSin(angle1));
    QPointF p2_inner = m_center + QPointF(m_innerRadius * qCos(angle2),
                                          m_innerRadius * qSin(angle2));

    QPainterPath path;
    path.moveTo(p1_inner);
    path.lineTo(p1_outer);
    path.lineTo(p2_outer);
    path.lineTo(p2_inner);
    path.closeSubpath();

    painter.fillPath(path, color);
  }

  painter.restore();
}

void HSVColorPicker::drawSVTriangle(QPainter &painter) {
  painter.save();

  // Create SV triangle gradient
  // Top vertex: white (S=0, V=1)
  // Bottom-left: black (S=0, V=0)
  // Bottom-right: pure hue color (S=1, V=1)

  QColor pureHue = hsvToRgb(m_hue, 1.0, 1.0);

  // Draw triangle with gradient approximation
  int steps = 50;
  for (int i = 0; i < steps; ++i) {
    for (int j = 0; j < steps - i; ++j) {
      float s = (float)j / steps;
      float v = 1.0 - (float)i / steps;

      float s2 = (float)(j + 1) / steps;
      float v2 = 1.0 - (float)(i + 1) / steps;

      QColor c1 = hsvToRgb(m_hue, s, v);
      QColor c2 = hsvToRgb(m_hue, s2, v);
      QColor c3 = hsvToRgb(m_hue, s, v2);

      QPointF p1 = svToTrianglePoint(s, v);
      QPointF p2 = svToTrianglePoint(s2, v);
      QPointF p3 = svToTrianglePoint(s, v2);

      QPainterPath path;
      path.moveTo(p1);
      path.lineTo(p2);
      path.lineTo(p3);
      path.closeSubpath();

      QLinearGradient gradient(p1, p2);
      gradient.setColorAt(0, c1);
      gradient.setColorAt(1, c2);

      painter.fillPath(path, gradient);
    }
  }

  // Draw triangle outline
  painter.setPen(QPen(Qt::black, 2));
  painter.drawLine(m_trianglePoints[0], m_trianglePoints[1]);
  painter.drawLine(m_trianglePoints[1], m_trianglePoints[2]);
  painter.drawLine(m_trianglePoints[2], m_trianglePoints[0]);

  painter.restore();
}

void HSVColorPicker::drawSelectors(QPainter &painter) {
  painter.save();

  // Draw hue selector on circle
  QPointF huePoint = hueToCirclePoint(m_hue);
  painter.setPen(QPen(Qt::white, 3));
  painter.setBrush(Qt::NoBrush);
  painter.drawEllipse(huePoint, 6, 6);
  painter.setPen(QPen(Qt::black, 1));
  painter.drawEllipse(huePoint, 6, 6);

  // Draw SV selector in triangle
  QPointF svPoint = svToTrianglePoint(m_saturation, m_value);
  painter.setPen(QPen(Qt::white, 3));
  painter.drawEllipse(svPoint, 6, 6);
  painter.setPen(QPen(Qt::black, 1));
  painter.drawEllipse(svPoint, 6, 6);

  painter.restore();
}

void HSVColorPicker::mousePressEvent(QMouseEvent *event) {
  QPointF pos = event->pos();

  if (isInHueCircle(pos)) {
    m_dragMode = DraggingHue;
    m_hue = circlePointToHue(pos);
    updateGeometry();
    m_currentColor = hsvToRgb(m_hue, m_saturation, m_value);
    emit colorChanged(m_currentColor);
    update();
  } else if (isInSVTriangle(pos)) {
    m_dragMode = DraggingSV;
    trianglePointToSV(pos, m_saturation, m_value);
    m_currentColor = hsvToRgb(m_hue, m_saturation, m_value);
    emit colorChanged(m_currentColor);
    update();
  }
}

void HSVColorPicker::mouseMoveEvent(QMouseEvent *event) {
  QPointF pos = event->pos();

  if (m_dragMode == DraggingHue) {
    m_hue = circlePointToHue(pos);
    updateGeometry();
    m_currentColor = hsvToRgb(m_hue, m_saturation, m_value);
    emit colorChanged(m_currentColor);
    update();
  } else if (m_dragMode == DraggingSV) {
    trianglePointToSV(pos, m_saturation, m_value);
    m_currentColor = hsvToRgb(m_hue, m_saturation, m_value);
    emit colorChanged(m_currentColor);
    update();
  }
}

void HSVColorPicker::mouseReleaseEvent(QMouseEvent *event) {
  Q_UNUSED(event);
  m_dragMode = None;
}

void HSVColorPicker::resizeEvent(QResizeEvent *event) {
  Q_UNUSED(event);
  updateGeometry();
}

QPointF HSVColorPicker::hueToCirclePoint(float hue) const {
  float angle = qDegreesToRadians(hue - 90.0);
  float radius = (m_outerRadius + m_innerRadius) / 2.0;
  return m_center + QPointF(radius * qCos(angle), radius * qSin(angle));
}

float HSVColorPicker::circlePointToHue(const QPointF &point) const {
  QPointF delta = point - m_center;
  float angle = qAtan2(delta.y(), delta.x());
  float hue = qRadiansToDegrees(angle) + 90.0;
  if (hue < 0)
    hue += 360.0;
  if (hue >= 360.0)
    hue -= 360.0;
  return hue;
}

QPointF HSVColorPicker::svToTrianglePoint(float sat, float val) const {
  // Map S,V to barycentric coordinates in triangle
  // Top vertex (0): white (S=0, V=1)
  // Bottom-left (1): black (S=0, V=0)
  // Bottom-right (2): pure hue (S=1, V=1)

  float w0 = (1.0 - sat) * val;         // white
  float w1 = (1.0 - sat) * (1.0 - val); // black
  float w2 = sat;                       // hue

  return w0 * m_trianglePoints[0] + w1 * m_trianglePoints[1] +
         w2 * m_trianglePoints[2];
}

void HSVColorPicker::trianglePointToSV(const QPointF &point, float &sat,
                                       float &val) const {
  // Convert point to barycentric coordinates
  QPointF v0 = m_trianglePoints[1] - m_trianglePoints[0];
  QPointF v1 = m_trianglePoints[2] - m_trianglePoints[0];
  QPointF v2 = point - m_trianglePoints[0];

  float dot00 = QPointF::dotProduct(v0, v0);
  float dot01 = QPointF::dotProduct(v0, v1);
  float dot02 = QPointF::dotProduct(v0, v2);
  float dot11 = QPointF::dotProduct(v1, v1);
  float dot12 = QPointF::dotProduct(v1, v2);

  float invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
  float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
  float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

  // Clamp to triangle
  u = qBound(0.0f, u, 1.0f);
  v = qBound(0.0f, v, 1.0f);
  if (u + v > 1.0) {
    float sum = u + v;
    u /= sum;
    v /= sum;
  }

  // Convert barycentric to SV
  float w0 = 1.0 - u - v; // white influence
  float w1 = u;           // black influence
  float w2 = v;           // hue influence

  sat = w2;
  val = w0 + w2;

  sat = qBound(0.0f, sat, 1.0f);
  val = qBound(0.0f, val, 1.0f);
}

bool HSVColorPicker::isInHueCircle(const QPointF &point) const {
  float dist = QLineF(m_center, point).length();
  return dist >= m_innerRadius && dist <= m_outerRadius;
}

bool HSVColorPicker::isInSVTriangle(const QPointF &point) const {
  // Point in triangle test using barycentric coordinates
  QPointF v0 = m_trianglePoints[2] - m_trianglePoints[0];
  QPointF v1 = m_trianglePoints[1] - m_trianglePoints[0];
  QPointF v2 = point - m_trianglePoints[0];

  float dot00 = QPointF::dotProduct(v0, v0);
  float dot01 = QPointF::dotProduct(v0, v1);
  float dot02 = QPointF::dotProduct(v0, v2);
  float dot11 = QPointF::dotProduct(v1, v1);
  float dot12 = QPointF::dotProduct(v1, v2);

  float invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
  float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
  float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

  return (u >= 0) && (v >= 0) && (u + v <= 1);
}

QColor HSVColorPicker::hsvToRgb(float h, float s, float v) {
  QColor color;
  color.setHsvF(h / 360.0, s, v);
  return color;
}

void HSVColorPicker::rgbToHsv(const QColor &rgb, float &h, float &s, float &v) {
  h = rgb.hsvHueF() * 360.0;
  if (h < 0)
    h = 0; // Handle achromatic case
  s = rgb.hsvSaturationF();
  v = rgb.valueF();
}
