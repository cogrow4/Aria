#ifndef LAYER_H
#define LAYER_H

#include <QImage>
#include <QObject>
#include <QString>
#include <QUuid>

class Layer {
public:
  enum BlendMode { Normal, Multiply, Screen, Overlay };

  Layer(const QString &name, int width, int height);

  QString id() const;

  QString name() const;
  void setName(const QString &name);

  bool isVisible() const;
  void setVisible(bool visible);

  double opacity() const;
  void setOpacity(double opacity);

  BlendMode blendMode() const;
  void setBlendMode(BlendMode mode);

  bool isClippingMask() const;
  void setClippingMask(bool clipping);

  void setImage(const QImage &image);
  QImage &image();
  const QImage &image() const;

  void resize(int width, int height);

private:
  QString m_id;
  QString m_name;
  bool m_visible;
  double m_opacity;
  BlendMode m_blendMode;
  bool m_isClippingMask;
  QImage m_image;
};

#endif // LAYER_H
