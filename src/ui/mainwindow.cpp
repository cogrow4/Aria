#include "mainwindow.h"
#include "core/canvas.h"
#include "ui/panels/brushpanel.h"
#include "ui/panels/layerpanel.h"
#include "utils/shortcuts/shortcutmanager.h"
#include "widgets/hsvcolorpicker.h"

#include <QAction>
#include <QActionGroup>
#include <QDockWidget>
#include <QFileDialog>
#include <QImageWriter>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QToolBar>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) { setupUi(); }

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
  resize(1200, 800);
  setWindowTitle("Aria Digital Painting");

  // Central Widget (Canvas Area)
  QWidget *centralWidget = new QWidget(this);
  QVBoxLayout *layout = new QVBoxLayout(centralWidget);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  // Placeholder for Canvas (will be replaced by actual Canvas widget)
  m_canvas = new Canvas(this);
  layout->addWidget(m_canvas);

  setCentralWidget(centralWidget);

  createMenus();
  createToolbars();
  createDockPanels();
}

void MainWindow::createMenus() {
  QMenuBar *menuBar = this->menuBar();
  ShortcutManager *shortcuts = ShortcutManager::instance();

  QMenu *fileMenu = menuBar->addMenu("&File");
  QAction *newAction = fileMenu->addAction("New");
  connect(newAction, &QAction::triggered, [this](bool) { onNew(); });
  shortcuts->registerAction("file.new", newAction, QKeySequence::New); // Ctrl+N

  QAction *openAction = fileMenu->addAction("Open");
  connect(openAction, &QAction::triggered, [this](bool) { onOpen(); });
  shortcuts->registerAction("file.open", openAction,
                            QKeySequence::Open); // Ctrl+O

  QAction *saveAction = fileMenu->addAction("Save");
  connect(saveAction, &QAction::triggered, [this](bool) { onSave(); });
  shortcuts->registerAction("file.save", saveAction,
                            QKeySequence::Save); // Ctrl+S

  QAction *saveAsAction = fileMenu->addAction("Save As...");
  connect(saveAsAction, &QAction::triggered, [this](bool) { onSaveAs(); });
  shortcuts->registerAction("file.saveAs", saveAsAction,
                            QKeySequence::SaveAs); // Ctrl+Shift+S

  fileMenu->addSeparator();
  QAction *exitAction = fileMenu->addAction("Exit");
  exitAction->setShortcut(QKeySequence::Quit); // Ctrl+Q
  connect(exitAction, &QAction::triggered, this, &QWidget::close);

  QMenu *editMenu = menuBar->addMenu("&Edit");
  QAction *undoAction = editMenu->addAction("Undo");
  shortcuts->registerAction("edit.undo", undoAction,
                            QKeySequence::Undo); // Ctrl+Z

  QAction *redoAction = editMenu->addAction("Redo");
  shortcuts->registerAction("edit.redo", redoAction,
                            QKeySequence::Redo); // Ctrl+Shift+Z

  QMenu *viewMenu = menuBar->addMenu("&View");
  // Will be populated in createDockPanels() with dock toggle actions
  m_viewMenu = viewMenu;
  viewMenu->addSeparator();
  viewMenu->addAction("Zoom In");
  viewMenu->addAction("Zoom Out");
  viewMenu->addAction("Fit to Screen");

  QMenu *layerMenu = menuBar->addMenu("&Layer");
  QAction *newLayerAction = layerMenu->addAction("New Layer");
  shortcuts->registerAction("layer.new", newLayerAction,
                            QKeySequence(Qt::CTRL | Qt::Key_L));

  layerMenu->addAction("Delete Layer");
  layerMenu->addAction("Duplicate Layer");

  QMenu *filterMenu = menuBar->addMenu("Filte&r");
  filterMenu->addAction("Blur");
  filterMenu->addAction("Sharpen");
}

void MainWindow::createToolbars() {
  QToolBar *toolsToolbar = new QToolBar("Tools", this);
  toolsToolbar->setObjectName("ToolsToolbar");
  addToolBar(Qt::LeftToolBarArea, toolsToolbar);

  ShortcutManager *shortcuts = ShortcutManager::instance();

  // Create action group for exclusive tool selection
  m_toolActionGroup = new QActionGroup(this);
  m_toolActionGroup->setExclusive(true);

  // Brush Tool
  QAction *brushAction = new QAction("🖌️ Brush", this);
  brushAction->setCheckable(true);
  brushAction->setChecked(true); // Default tool
  connect(brushAction, &QAction::triggered,
          [this](bool) { m_canvas->setTool(Canvas::BrushTool); });
  toolsToolbar->addAction(brushAction);
  m_toolActionGroup->addAction(brushAction);
  shortcuts->registerAction("tool.brush", brushAction, QKeySequence(Qt::Key_B));

  // Eraser Tool
  QAction *eraserAction = new QAction("🧹 Eraser", this);
  eraserAction->setCheckable(true);
  connect(eraserAction, &QAction::triggered,
          [this](bool) { m_canvas->setTool(Canvas::EraserTool); });
  toolsToolbar->addAction(eraserAction);
  m_toolActionGroup->addAction(eraserAction);
  shortcuts->registerAction("tool.eraser", eraserAction,
                            QKeySequence(Qt::Key_E));

  // Eyedropper Tool
  QAction *eyedropperAction = new QAction("💧 Eyedropper", this);
  eyedropperAction->setCheckable(true);
  connect(eyedropperAction, &QAction::triggered,
          [this](bool) { m_canvas->setTool(Canvas::EyedropperTool); });
  toolsToolbar->addAction(eyedropperAction);
  m_toolActionGroup->addAction(eyedropperAction);
  shortcuts->registerAction("tool.eyedropper", eyedropperAction,
                            QKeySequence(Qt::Key_I));

  // Fill Bucket Tool
  QAction *fillAction = new QAction("🪣 Fill", this);
  fillAction->setCheckable(true);
  connect(fillAction, &QAction::triggered,
          [this](bool) { m_canvas->setTool(Canvas::FillBucketTool); });
  toolsToolbar->addAction(fillAction);
  m_toolActionGroup->addAction(fillAction);
  shortcuts->registerAction("tool.fill", fillAction, QKeySequence(Qt::Key_G));

  toolsToolbar->addSeparator();

  // Rectangle Select Tool
  QAction *rectSelectAction = new QAction("▭ Rect Select", this);
  rectSelectAction->setCheckable(true);
  connect(rectSelectAction, &QAction::triggered,
          [this](bool) { m_canvas->setTool(Canvas::RectSelectTool); });
  toolsToolbar->addAction(rectSelectAction);
  m_toolActionGroup->addAction(rectSelectAction);
  shortcuts->registerAction("tool.rectSelect", rectSelectAction,
                            QKeySequence(Qt::Key_M));

  // Ellipse Select Tool
  QAction *ellipseSelectAction = new QAction("⬭ Ellipse Select", this);
  ellipseSelectAction->setCheckable(true);
  connect(ellipseSelectAction, &QAction::triggered,
          [this](bool) { m_canvas->setTool(Canvas::EllipseSelectTool); });
  toolsToolbar->addAction(ellipseSelectAction);
  m_toolActionGroup->addAction(ellipseSelectAction);
  shortcuts->registerAction("tool.ellipseSelect", ellipseSelectAction,
                            QKeySequence(Qt::SHIFT | Qt::Key_M));

  // Lasso Select Tool
  QAction *lassoAction = new QAction("✂️ Lasso", this);
  lassoAction->setCheckable(true);
  connect(lassoAction, &QAction::triggered,
          [this](bool) { m_canvas->setTool(Canvas::LassoTool); });
  toolsToolbar->addAction(lassoAction);
  m_toolActionGroup->addAction(lassoAction);
  shortcuts->registerAction("tool.lasso", lassoAction, QKeySequence(Qt::Key_O));

  toolsToolbar->addSeparator();

  // Line Tool
  QAction *lineAction = new QAction("╱ Line", this);
  lineAction->setCheckable(true);
  connect(lineAction, &QAction::triggered,
          [this](bool) { m_canvas->setTool(Canvas::LineTool); });
  toolsToolbar->addAction(lineAction);
  m_toolActionGroup->addAction(lineAction);
  shortcuts->registerAction("tool.line", lineAction, QKeySequence(Qt::Key_L));

  // Text Tool - Hidden for now but keep in code
  QAction *textAction = new QAction("T Text", this);
  textAction->setCheckable(true);
  textAction->setVisible(false); // Hide from toolbar
  connect(textAction, &QAction::triggered,
          [this](bool) { m_canvas->setTool(Canvas::TextTool); });
  // toolsToolbar->addAction(textAction);  // Commented out - don't add to
  // toolbar
  m_toolActionGroup->addAction(textAction);
  shortcuts->registerAction("tool.text", textAction, QKeySequence(Qt::Key_T));
}

void MainWindow::createDockPanels() {
  // Layer Panel
  QDockWidget *layersDock = new QDockWidget("Layers", this);
  layersDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
  layersDock->setObjectName("LayersPanel"); // For saving state

  LayerPanel *layerPanel = new LayerPanel(m_canvas->layerManager(), this);
  layersDock->setWidget(layerPanel);

  // Connect layer panel to get canvas size
  connect(layerPanel, &LayerPanel::requestCanvasSize, this,
          [this](int &width, int &height) {
            QImage &img = m_canvas->layerManager()->currentLayer()->image();
            width = img.width();
            height = img.height();
          });

  addDockWidget(Qt::RightDockWidgetArea, layersDock);

  // Add toggle action to View menu
  if (m_viewMenu) {
    m_viewMenu->insertAction(m_viewMenu->actions().first(),
                             layersDock->toggleViewAction());
  }

  // Brush Settings Panel
  QDockWidget *brushDock = new QDockWidget("Brush Settings", this);
  brushDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
  brushDock->setObjectName("BrushPanel"); // For saving state

  BrushPanel *brushPanel = new BrushPanel(m_canvas, this);

  QWidget *brushContainer = new QWidget();
  QVBoxLayout *brushLayout = new QVBoxLayout(brushContainer);
  brushLayout->setContentsMargins(0, 0, 0, 0);
  brushLayout->addWidget(brushPanel);

  HSVColorPicker *colorPicker = new HSVColorPicker(this);
  brushLayout->addWidget(colorPicker);

  connect(colorPicker, &HSVColorPicker::colorChanged,
          [this](const QColor &color) {
            if (m_canvas && m_canvas->brush()) {
              m_canvas->brush()->setColor(color);
            }
          });

  connect(m_canvas, &Canvas::colorPicked, colorPicker,
          &HSVColorPicker::setColor);

  brushDock->setWidget(brushContainer);

  addDockWidget(Qt::RightDockWidgetArea, brushDock);

  // Add toggle action to View menu
  if (m_viewMenu) {
    m_viewMenu->insertAction(m_viewMenu->actions().first(),
                             brushDock->toggleViewAction());
  }
}
