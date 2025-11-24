#include "core/canvas.h"
#include "mainwindow.h"

// Selection menu implementations

void MainWindow::selectAll() {
  if (!m_canvas)
    return;

  // Create selection of entire canvas
  int width = m_canvas->layerManager()->composite(800, 600).width();
  int height = m_canvas->layerManager()->composite(800, 600).height();

  // Set selection to full canvas rectangle
  QRect fullRect(0, 0, width, height);
  // TODO: Add method to Canvas to set selection programmatically
  // For now, this is a placeholder
}

void MainWindow::deselect() {
  if (!m_canvas)
    return;

  // Clear selection
  // TODO: Add method to Canvas to clear selection
  // For now this is a placeholder
}

void MainWindow::invertSelection() {
  if (!m_canvas)
    return;

  // Invert current selection
  // TODO: Implement selection inversion
  // For now this is a placeholder
}
