#include "layermanager.h"
#include <QPainter>

LayerManager::LayerManager(QObject *parent)
    : QObject(parent), m_currentLayerIndex(-1) {}

void LayerManager::addLayer(const QString &name, int width, int height) {
  auto layer = std::make_unique<Layer>(name, width, height);
  m_layers.push_back(std::move(layer));
  m_currentLayerIndex = m_layers.size() - 1;

  emit layerAdded(m_currentLayerIndex);
  emit currentLayerChanged(m_currentLayerIndex);
  emit canvasUpdateNeeded();
}

void LayerManager::deleteLayer(int index) {
  if (index < 0 || index >= m_layers.size())
    return;

  m_layers.erase(m_layers.begin() + index);

  if (m_currentLayerIndex >= m_layers.size())
    m_currentLayerIndex = m_layers.size() - 1;

  emit layerRemoved(index);
  emit currentLayerChanged(m_currentLayerIndex);
  emit canvasUpdateNeeded();
}

void LayerManager::duplicateLayer(int index) {
  if (index < 0 || index >= m_layers.size())
    return;

  Layer *source = m_layers[index].get();
  auto newLayer =
      std::make_unique<Layer>(source->name() + " copy", source->image().width(),
                              source->image().height());

  // Copy image content
  QPainter painter(&newLayer->image());
  painter.drawImage(0, 0, source->image());

  // Copy properties
  newLayer->setOpacity(source->opacity());
  newLayer->setBlendMode(source->blendMode());
  newLayer->setClippingMask(source->isClippingMask());

  m_layers.insert(m_layers.begin() + index + 1, std::move(newLayer));
  m_currentLayerIndex = index + 1;

  emit layerAdded(index + 1);
  emit currentLayerChanged(m_currentLayerIndex);
  emit canvasUpdateNeeded();
}

void LayerManager::moveLayer(int fromIndex, int toIndex) {
  if (fromIndex < 0 || fromIndex >= m_layers.size() || toIndex < 0 ||
      toIndex >= m_layers.size())
    return;

  if (fromIndex == toIndex)
    return;

  auto layer = std::move(m_layers[fromIndex]);
  m_layers.erase(m_layers.begin() + fromIndex);
  m_layers.insert(m_layers.begin() + toIndex, std::move(layer));

  // Update current index if needed
  if (m_currentLayerIndex == fromIndex) {
    m_currentLayerIndex = toIndex;
  } else if (fromIndex < m_currentLayerIndex &&
             toIndex >= m_currentLayerIndex) {
    m_currentLayerIndex--;
  } else if (fromIndex > m_currentLayerIndex &&
             toIndex <= m_currentLayerIndex) {
    m_currentLayerIndex++;
  }

  emit layerMoved(fromIndex, toIndex);
  emit currentLayerChanged(m_currentLayerIndex);
  emit canvasUpdateNeeded();
}

int LayerManager::layerCount() const { return m_layers.size(); }

Layer *LayerManager::layerAt(int index) {
  if (index < 0 || index >= m_layers.size())
    return nullptr;
  return m_layers[index].get();
}

int LayerManager::currentLayerIndex() const { return m_currentLayerIndex; }

Layer *LayerManager::currentLayer() { return layerAt(m_currentLayerIndex); }

void LayerManager::setCurrentLayer(int index) {
  if (index < 0 || index >= m_layers.size())
    return;

  if (m_currentLayerIndex != index) {
    m_currentLayerIndex = index;
    emit currentLayerChanged(m_currentLayerIndex);
  }
}

QImage LayerManager::composite(int width, int height) {
  QImage result(width, height, QImage::Format_ARGB32_Premultiplied);
  result.fill(Qt::white); // Background color

  QPainter painter(&result);

  for (int i = 0; i < m_layers.size(); ++i) {
    Layer *layer = m_layers[i].get();
    if (!layer->isVisible())
      continue;

    // Handle clipping masks
    if (layer->isClippingMask() && i > 0) {
      // This is a simplified clipping mask implementation
      // In a real engine, we'd need to composite this layer into a temporary
      // buffer masked by the alpha of the layer below. For now, we'll just draw
      // it normally but respecting opacity

      // TODO: Implement proper clipping mask logic using
      // QPainter::CompositionMode_DestinationIn
    }

    painter.setOpacity(layer->opacity());

    // Set composition mode based on blend mode
    switch (layer->blendMode()) {
    case Layer::Multiply:
      painter.setCompositionMode(QPainter::CompositionMode_Multiply);
      break;
    case Layer::Screen:
      painter.setCompositionMode(QPainter::CompositionMode_Screen);
      break;
    case Layer::Overlay:
      painter.setCompositionMode(QPainter::CompositionMode_Overlay);
      break;
    default:
      painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
      break;
    }

    painter.drawImage(0, 0, layer->image());
  }

  return result;
}

void LayerManager::render(QPainter &painter, const QRect &rect) {
  // Fill background for the rect
  painter.setCompositionMode(QPainter::CompositionMode_Source);
  painter.fillRect(rect, Qt::white); // Background color

  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

  for (int i = 0; i < m_layers.size(); ++i) {
    Layer *layer = m_layers[i].get();
    if (!layer->isVisible())
      continue;

    // Handle clipping masks (simplified)
    if (layer->isClippingMask() && i > 0) {
      // TODO: Implement proper clipping
    }

    painter.setOpacity(layer->opacity());

    // Set composition mode based on blend mode
    switch (layer->blendMode()) {
    case Layer::Multiply:
      painter.setCompositionMode(QPainter::CompositionMode_Multiply);
      break;
    case Layer::Screen:
      painter.setCompositionMode(QPainter::CompositionMode_Screen);
      break;
    case Layer::Overlay:
      painter.setCompositionMode(QPainter::CompositionMode_Overlay);
      break;
    default:
      painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
      break;
    }

    painter.drawImage(rect, layer->image(), rect);
  }
}
