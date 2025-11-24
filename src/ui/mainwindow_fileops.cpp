#include "core/canvas.h"
#include "ui/dialogs/welcomedialog.h"
#include "mainwindow.h"

#include <QDialog>
#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QInputDialog>
#include <QMessageBox>

void MainWindow::onNew() {
  WelcomeDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted) {
    m_canvas->newImage(dialog.canvasWidth(), dialog.canvasHeight(), Qt::white);
  }
}

void MainWindow::onOpen() {
  QString fileName = QFileDialog::getOpenFileName(
      this, "Open Image", QString(), "Images (*.png *.jpg *.jpeg *.bmp)");
  if (fileName.isEmpty())
    return;

  QImage image(fileName);
  if (image.isNull()) {
    QMessageBox::warning(this, "Open Image", "Failed to load image.");
    return;
  }

  m_canvas->newImage(image.width(), image.height());
  if (m_canvas->layerManager()->layerCount() > 0) {
    m_canvas->layerManager()->layerAt(0)->setImage(image);
    m_canvas->update();
  }
}

void MainWindow::onSave() {
  QString fileName = QFileDialog::getSaveFileName(
      this, "Save Image", QString(),
      "PNG Images (*.png);;JPEG Images (*.jpg *.jpeg)");
  if (fileName.isEmpty())
    return;

  int width = m_canvas->layerManager()->layerAt(0)->image().width();
  int height = m_canvas->layerManager()->layerAt(0)->image().height();
  QImage composite = m_canvas->layerManager()->composite(width, height);
  if (!composite.save(fileName)) {
    QMessageBox::warning(this, "Save Image", "Failed to save image.");
  }
}

void MainWindow::onSaveAs() { onSave(); }
