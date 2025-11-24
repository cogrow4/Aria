#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H

#include "core/layer.h"
#include <QImage>
#include <QObject>
#include <QPainter>
#include <QRect>
#include <memory>
#include <vector>

class LayerManager : public QObject {
  Q_OBJECT

public:
  explicit LayerManager(QObject *parent = nullptr);

  void addLayer(const QString &name, int width, int height);
  void deleteLayer(int index);
  void duplicateLayer(int index);
  void moveLayer(int fromIndex, int toIndex);

  int layerCount() const;
  Layer *layerAt(int index);
  int currentLayerIndex() const;
  Layer *currentLayer();

  void setCurrentLayer(int index);

  QImage composite(int width, int height);
  void render(QPainter &painter, const QRect &rect);

signals:
  void layerAdded(int index);
  void layerRemoved(int index);
  void layerMoved(int from, int to);
  void currentLayerChanged(int index);
  void layerContentChanged(int index); // For thumbnail updates
  void canvasUpdateNeeded();

private:
  std::vector<std::unique_ptr<Layer>> m_layers; // 0 is bottom, size-1 is top
  int m_currentLayerIndex;
};

#endif // LAYERMANAGER_H
