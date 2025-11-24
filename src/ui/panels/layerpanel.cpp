#include "layerpanel.h"
#include "core/layer.h"
#include "core/layermanager.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QListView>
#include <QPushButton>
#include <QStandardItemModel>
#include <QToolButton>
#include <QVBoxLayout>

LayerPanel::LayerPanel(LayerManager *manager, QWidget *parent)
    : QWidget(parent), m_manager(manager) {
  setupUi();

  connect(m_manager, &LayerManager::layerAdded, this,
          &LayerPanel::refreshLayerList);
  connect(m_manager, &LayerManager::layerRemoved, this,
          &LayerPanel::refreshLayerList);
  connect(m_manager, &LayerManager::layerMoved, this,
          &LayerPanel::refreshLayerList);
  connect(m_manager, &LayerManager::currentLayerChanged, this,
          [this](int index) {
            m_layerListView->setCurrentIndex(
                m_layerModel->index(m_manager->layerCount() - 1 - index, 0));
          });

  refreshLayerList();
}

LayerPanel::~LayerPanel() {}

void LayerPanel::setupUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(5, 5, 5, 5);

  // Toolbar
  QHBoxLayout *toolbarLayout = new QHBoxLayout();

  QToolButton *addBtn = new QToolButton(this);
  addBtn->setText("+");
  connect(addBtn, &QToolButton::clicked, this, &LayerPanel::onAddLayer);

  QToolButton *delBtn = new QToolButton(this);
  delBtn->setText("-");
  connect(delBtn, &QToolButton::clicked, this, &LayerPanel::onDeleteLayer);

  QToolButton *dupBtn = new QToolButton(this);
  dupBtn->setText("Dup");
  connect(dupBtn, &QToolButton::clicked, this, &LayerPanel::onDuplicateLayer);

  toolbarLayout->addWidget(addBtn);
  toolbarLayout->addWidget(delBtn);
  toolbarLayout->addWidget(dupBtn);
  toolbarLayout->addStretch();

  mainLayout->addLayout(toolbarLayout);

  // Layer List
  m_layerListView = new QListView(this);
  m_layerModel = new QStandardItemModel(this);
  m_layerListView->setModel(m_layerModel);

  connect(m_layerListView->selectionModel(),
          &QItemSelectionModel::currentChanged, this,
          &LayerPanel::onLayerSelectionChanged);

  mainLayout->addWidget(m_layerListView);
}

void LayerPanel::onAddLayer() {
  // Request canvas size from parent (MainWindow will connect to this)
  int width = 800;
  int height = 600;
  emit requestCanvasSize(width, height);

  m_manager->addLayer("New Layer", width, height);
}

void LayerPanel::onDeleteLayer() {
  m_manager->deleteLayer(m_manager->currentLayerIndex());
}

void LayerPanel::onDuplicateLayer() {
  m_manager->duplicateLayer(m_manager->currentLayerIndex());
}

void LayerPanel::onLayerSelectionChanged(const QModelIndex &current,
                                         const QModelIndex &previous) {
  if (!current.isValid())
    return;

  // Map list index (top-down) to layer index (bottom-up)
  int layerIndex = m_manager->layerCount() - 1 - current.row();
  m_manager->setCurrentLayer(layerIndex);
}

void LayerPanel::refreshLayerList() {
  m_layerModel->clear();

  // List layers from top to bottom
  for (int i = m_manager->layerCount() - 1; i >= 0; --i) {
    Layer *layer = m_manager->layerAt(i);
    if (!layer)
      continue;

    QStandardItem *item = new QStandardItem(layer->name());
    item->setCheckable(true);
    item->setCheckState(layer->isVisible() ? Qt::Checked : Qt::Unchecked);

    // TODO: Add thumbnail

    m_layerModel->appendRow(item);
  }

  // Restore selection
  int currentRow = m_manager->layerCount() - 1 - m_manager->currentLayerIndex();
  m_layerListView->setCurrentIndex(m_layerModel->index(currentRow, 0));
}
