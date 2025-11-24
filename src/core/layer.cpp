#include "layer.h"
#include <QPainter>

Layer::Layer(const QString &name, int width, int height)
    : m_id(QUuid::createUuid().toString()), m_name(name), m_visible(true),
      m_opacity(1.0), m_blendMode(Normal), m_isClippingMask(false) {
  m_image = QImage(width, height, QImage::Format_ARGB32_Premultiplied);
  m_image.fill(Qt::transparent);
}

QString Layer::id() const { return m_id; }

QString Layer::name() const { return m_name; }

void Layer::setName(const QString &name) { m_name = name; }

bool Layer::isVisible() const { return m_visible; }

void Layer::setVisible(bool visible) { m_visible = visible; }

double Layer::opacity() const { return m_opacity; }

void Layer::setOpacity(double opacity) { m_opacity = opacity; }

Layer::BlendMode Layer::blendMode() const { return m_blendMode; }

void Layer::setBlendMode(BlendMode mode) { m_blendMode = mode; }

bool Layer::isClippingMask() const { return m_isClippingMask; }

void Layer::setClippingMask(bool clipping) { m_isClippingMask = clipping; }

QImage &Layer::image() { return m_image; }

const QImage &Layer::image() const { return m_image; }

void Layer::setImage(const QImage &image) { m_image = image; }

void Layer::resize(int newWidth, int newHeight) {
  QImage newImage(newWidth, newHeight, QImage::Format_ARGB32_Premultiplied);
  newImage.fill(Qt::transparent);
  QPainter painter(&newImage);
  painter.drawImage(QPoint(0, 0), m_image);
  m_image = newImage;
}
