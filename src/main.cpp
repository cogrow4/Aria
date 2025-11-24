#include "core/canvas.h"
#include "ui/dialogs/welcomedialog.h"
#include "ui/mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // Set application metadata
  app.setApplicationName("Aria");
  app.setApplicationDisplayName("Aria Digital Painting");
  app.setOrganizationName("AriaProject");
  app.setOrganizationDomain("aria.app");

  // Load stylesheet
  QFile file(":/dark_theme.qss");
  if (file.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());
  }

  // Show welcome dialog first
  WelcomeDialog welcomeDialog;
  if (welcomeDialog.exec() == QDialog::Accepted) {
    MainWindow mainWindow;

    // Create new image with selected size
    mainWindow.show();
    mainWindow.canvas()->newImage(welcomeDialog.canvasWidth(),
                                  welcomeDialog.canvasHeight());

    return app.exec();
  }

  return 0;
}
