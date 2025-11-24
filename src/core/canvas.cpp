#include "canvas.h"

#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QSet>
#include <QStack>
#include <QTabletEvent>

Canvas::Canvas(QWidget *parent)
    : QWidget(parent), m_drawing(false), m_currentTool(BrushTool),
      m_selectionActive(false) {
  setAttribute(Qt::WA_StaticContents);

  // Enable mouse tracking so we get move events even without buttons pressed
  setMouseTracking(true);

  connect(&m_layerManager, &LayerManager::canvasUpdateNeeded, this,
          QOverload<>::of(&Canvas::update));

  // Initialize with a default white canvas
  newImage(800, 600, Qt::white);
}

Canvas::~Canvas() {}

void Canvas::newImage(int width, int height, const QColor &backgroundColor) {
  m_image = QImage(width, height, QImage::Format_ARGB32_Premultiplied);
  m_image.fill(backgroundColor);

  // Clear existing layers
  while (m_layerManager.layerCount() > 0) {
    m_layerManager.deleteLayer(0);
  }

  // Add default background layer
  m_layerManager.addLayer("Background", width, height);
  Layer *bgLayer = m_layerManager.layerAt(0);
  if (bgLayer) {
    bgLayer->image().fill(backgroundColor);
  }

  update();
}

void Canvas::paintEvent(QPaintEvent *event) {
  QPainter painter(this);

  // Always composite the layers to show latest changes
  QImage composited =
      m_layerManager.composite(m_image.width(), m_image.height());

  // Center the image in the widget
  int xOffset = (width() - composited.width()) / 2;
  int yOffset = (height() - composited.height()) / 2;

  // Fill background
  painter.fillRect(rect(), Qt::darkGray);

  // Draw the composited image
  painter.drawImage(xOffset, yOffset, composited);

  // Draw selection preview during drag
  if (m_selectionActive && !m_selectionRect.isNull()) {
    QRect adjustedRect = m_selectionRect.translated(xOffset, yOffset);
    // High-contrast dual outline (black then white)
    painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
    painter.setBrush(Qt::NoBrush);
    if (m_currentTool == RectSelectTool) {
      painter.drawRect(adjustedRect);
    } else if (m_currentTool == EllipseSelectTool) {
      painter.drawEllipse(adjustedRect);
    }
    painter.setPen(QPen(Qt::white, 1, Qt::DashLine));
    if (m_currentTool == RectSelectTool) {
      painter.drawRect(adjustedRect);
    } else if (m_currentTool == EllipseSelectTool) {
      painter.drawEllipse(adjustedRect);
    }
  }

  // Draw lasso path preview during drag
  if (m_currentTool == LassoTool && m_selectionActive &&
      m_lassoPath.size() > 1) {
    QPolygon adjustedPath = m_lassoPath.translated(xOffset, yOffset);
    painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
    painter.drawPolyline(adjustedPath);
    painter.setPen(QPen(Qt::white, 1, Qt::DashLine));
    painter.drawPolyline(adjustedPath);
  }

  // Draw finalized selection with VERY visible outline
  if (!m_selectionRegion.isEmpty() && !m_selectionActive) {
    QRegion adjustedRegion = m_selectionRegion.translated(xOffset, yOffset);
    QRect boundingRect = adjustedRegion.boundingRect();
    // Draw thick black outline
    painter.setPen(QPen(Qt::black, 3, Qt::SolidLine));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(boundingRect.adjusted(-1, -1, 1, 1));
    // Draw white dashed line on top
    painter.setPen(QPen(Qt::white, 2, Qt::DashLine));
    painter.drawRect(boundingRect);
  }
}

void Canvas::resizeEvent(QResizeEvent *event) {
  // In a real app, we might want to handle zoom/pan here,
  // but for now we just let the canvas stay centered.
  QWidget::resizeEvent(event);
}

void Canvas::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    int xOffset = (width() - m_image.width()) / 2;
    int yOffset = (height() - m_image.height()) / 2;
    QPointF currentPoint = event->position() - QPointF(xOffset, yOffset);

    m_lastPoint = currentPoint;

    if (m_currentTool == EyedropperTool) {
      // Pick color
      if (currentPoint.x() >= 0 && currentPoint.x() < m_image.width() &&
          currentPoint.y() >= 0 && currentPoint.y() < m_image.height()) {
        QImage composited =
            m_layerManager.composite(m_image.width(), m_image.height());
        QColor pickedColor = composited.pixelColor(currentPoint.toPoint());
        m_brush.setColor(pickedColor);
        emit colorPicked(pickedColor);
      }
      m_drawing = false;
    } else if (m_currentTool == FillBucketTool) {
      // Fill with current color
      if (currentPoint.x() >= 0 && currentPoint.x() < m_image.width() &&
          currentPoint.y() >= 0 && currentPoint.y() < m_image.height()) {
        floodFill(currentPoint.toPoint(), m_brush.color());
      }
      m_drawing = false;
    } else if (m_currentTool == RectSelectTool ||
               m_currentTool == EllipseSelectTool) {
      // Start new selection (clears old one)
      m_selectionRect = QRect(currentPoint.toPoint(), QSize(0, 0));
      m_selectionActive = true;
      m_selectionRegion = QRegion(); // Clear old selection
      m_drawing = false;
      update();
    } else if (m_currentTool == LassoTool) {
      // Start new lasso selection (clears old one)
      m_lassoPath.clear();
      m_lassoPath << currentPoint.toPoint();
      m_selectionActive = true;
      m_selectionRegion = QRegion(); // Clear old selection
      m_drawing = false;
      update();
    } else {
      // Brush tool - start drawing
      m_drawing = true;
      // Draw initial dot at press point
      drawLineTo(currentPoint, 1.0);
    }
  }
}

void Canvas::mouseMoveEvent(QMouseEvent *event) {
  if (m_currentTool == RectSelectTool || m_currentTool == EllipseSelectTool) {
    // Update selection preview
    int xOffset = (width() - m_image.width()) / 2;
    int yOffset = (height() - m_image.height()) / 2;
    QPointF currentPoint = event->position() - QPointF(xOffset, yOffset);
    m_selectionRect = QRectF(m_lastPoint, currentPoint).toRect();
    m_selectionActive = true;
    update();
  } else if (m_currentTool == LassoTool && m_selectionActive) {
    // Add point to lasso path
    int xOffset = (width() - m_image.width()) / 2;
    int yOffset = (height() - m_image.height()) / 2;
    QPointF currentPoint = event->position() - QPointF(xOffset, yOffset);
    m_lassoPath << currentPoint.toPoint();
    update();
  } else if (m_drawing && (event->buttons() & Qt::LeftButton)) {
    int xOffset = (width() - m_image.width()) / 2;
    int yOffset = (height() - m_image.height()) / 2;
    QPointF currentPoint = event->position() - QPointF(xOffset, yOffset);
    // Draw line from last point to current point (drawLineTo updates
    // m_lastPoint)
    drawLineTo(currentPoint, 1.0);
  }
}

void Canvas::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    if (m_currentTool == RectSelectTool || m_currentTool == EllipseSelectTool) {
      // Finalize selection
      if (m_currentTool == RectSelectTool) {
        m_selectionRegion = QRegion(m_selectionRect.normalized());
      } else {
        m_selectionRegion =
            QRegion(m_selectionRect.normalized(), QRegion::Ellipse);
      }
      // Keep selection active but not in preview mode
      m_selectionActive = false;
      m_selectionRect = QRect(); // Clear rect but keep region
      update();
    } else if (m_currentTool == LassoTool && m_selectionActive) {
      // Finalize lasso selection
      if (m_lassoPath.size() > 2) {
        m_selectionRegion = QRegion(m_lassoPath);
      }
      m_selectionActive = false;
      m_lassoPath.clear();
      update();
    } else if (m_drawing) {
      int xOffset = (width() - m_image.width()) / 2;
      int yOffset = (height() - m_image.height()) / 2;
      QPointF currentPoint = event->position() - QPointF(xOffset, yOffset);
      drawLineTo(currentPoint, 1.0);
      m_drawing = false;
    }
  }
}

void Canvas::tabletEvent(QTabletEvent *event) {
  // Basic tablet support
  int xOffset = (width() - m_image.width()) / 2;
  int yOffset = (height() - m_image.height()) / 2;
  QPointF currentPoint = event->position() - QPointF(xOffset, yOffset);

  switch (event->type()) {
  case QEvent::TabletPress:
    m_drawing = true;
    m_lastPoint = currentPoint;
    break;
  case QEvent::TabletMove:
    if (m_drawing) {
      drawLineTo(currentPoint, event->pressure());
      m_lastPoint = currentPoint;
    }
    break;
  case QEvent::TabletRelease:
    m_drawing = false;
    break;
  default:
    break;
  }
  event->accept();
}

void Canvas::drawLineTo(const QPointF &endPoint, qreal pressure) {
  if (!m_layerManager.currentLayer())
    return;

  QImage &layerImage = m_layerManager.currentLayer()->image();
  QPainter painter(&layerImage);
  painter.setRenderHint(QPainter::Antialiasing, true);

  // Apply clipping if selection is active
  if (!m_selectionRegion.isEmpty()) {
    painter.setClipRegion(m_selectionRegion);
  }

  // Get brush properties (ignore pressure for now - use full size)
  qreal size = m_brush.size();
  QColor color = m_brush.color();
  color.setAlphaF(m_brush.opacity());

  // Set up painter for brush or eraser
  if (m_brush.isEraser()) {
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    color = QColor(0, 0, 0, int(m_brush.opacity() * 255));
  } else {
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  }

  // Simple line drawing with round cap
  QPen pen(color, size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  painter.setPen(pen);

  // Draw line from last point to current point
  qDebug() << "Drawing from" << m_lastPoint << "to" << endPoint
           << "size:" << size;
  painter.drawLine(m_lastPoint, endPoint);

  // Calculate update rect BEFORE updating m_lastPoint
  int rad = int(size / 2) + 2;
  QRect updateRect = QRect(m_lastPoint.toPoint(), endPoint.toPoint())
                         .normalized()
                         .adjusted(-rad, -rad, +rad, +rad);

  // Update m_lastPoint for next segment
  m_lastPoint = endPoint;

  // Convert to widget coordinates
  int xOffset = (width() - m_image.width()) / 2;
  int yOffset = (height() - m_image.height()) / 2;
  update(updateRect.translated(xOffset, yOffset));
}

void Canvas::resizeImage(QImage *image, const QSize &newSize) {
  if (image->size() == newSize)
    return;

  QImage newImage(newSize, QImage::Format_ARGB32_Premultiplied);
  newImage.fill(Qt::white);
  QPainter painter(&newImage);
  painter.drawImage(QPoint(0, 0), *image);
  *image = newImage;
}
// Flood fill implementation added at end of canvas.cpp

void Canvas::floodFill(const QPoint &startPoint, const QColor &fillColor) {
  if (!m_layerManager.currentLayer())
    return;

  QImage &layerImage = m_layerManager.currentLayer()->image();

  // Check bounds
  if (startPoint.x() < 0 || startPoint.x() >= layerImage.width() ||
      startPoint.y() < 0 || startPoint.y() >= layerImage.height()) {
    return;
  }

  QColor targetColor = layerImage.pixelColor(startPoint);

  // Don't fill if same color
  if (targetColor == fillColor) {
    return;
  }

  int tolerance = m_brush.tolerance();

  // Scanline flood fill algorithm
  QStack<QPoint> stack;
  stack.push(startPoint);

  auto colorMatch = [&](const QColor &c1, const QColor &c2) -> bool {
    if (tolerance == 0) {
      return c1 == c2;
    }
    int dr = qAbs(c1.red() - c2.red());
    int dg = qAbs(c1.green() - c2.green());
    int db = qAbs(c1.blue() - c2.blue());
    int da = qAbs(c1.alpha() - c2.alpha());
    return (dr <= tolerance && dg <= tolerance && db <= tolerance &&
            da <= tolerance);
  };

  QSet<QPoint> visited;

  while (!stack.isEmpty()) {
    QPoint p = stack.pop();

    if (p.x() < 0 || p.x() >= layerImage.width() || p.y() < 0 ||
        p.y() >= layerImage.height()) {
      continue;
    }

    if (visited.contains(p)) {
      continue;
    }

    // Check if within selection (if active)
    if (!m_selectionRegion.isEmpty() && !m_selectionRegion.contains(p)) {
      continue;
    }

    QColor currentColor = layerImage.pixelColor(p);
    if (!colorMatch(currentColor, targetColor)) {
      continue;
    }

    visited.insert(p);
    layerImage.setPixelColor(p, fillColor);

    // Add neighbors
    stack.push(QPoint(p.x() + 1, p.y()));
    stack.push(QPoint(p.x() - 1, p.y()));
    stack.push(QPoint(p.x(), p.y() + 1));
    stack.push(QPoint(p.x(), p.y() - 1));
  }

  update();
}
