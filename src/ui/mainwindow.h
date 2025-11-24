#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Canvas;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  Canvas *canvas() const { return m_canvas; }

private:
  void setupUi();
  void createMenus();
  void createToolbars();
  void createDockPanels();

private slots:
  void onNew();
  void onOpen();
  void onSave();
  void onSaveAs();

  // Select menu slots
  void selectAll();
  void deselect();
  void invertSelection();

private:
  Canvas *m_canvas;
  QMenu *m_viewMenu;               // For adding dock panel toggle actions
  QActionGroup *m_toolActionGroup; // For exclusive tool selection
};

#endif // MAINWINDOW_H
