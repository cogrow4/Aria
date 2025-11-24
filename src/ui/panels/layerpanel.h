#ifndef LAYERPANEL_H
#define LAYERPANEL_H

#include <QListView>
#include <QStandardItemModel>
#include <QWidget>

class LayerManager;

class LayerPanel : public QWidget {
  Q_OBJECT

public:
  explicit LayerPanel(LayerManager *manager, QWidget *parent = nullptr);
  ~LayerPanel();

signals:
  void requestCanvasSize(int &width, int &height);

private slots:
  void onAddLayer();
  void onDeleteLayer();
  void onDuplicateLayer();
  void onLayerSelectionChanged(const QModelIndex &current,
                               const QModelIndex &previous);
  void refreshLayerList();

private:
  void setupUi();

  LayerManager *m_manager;
  QListView *m_layerListView;
  QStandardItemModel *m_layerModel;
};

#endif // LAYERPANEL_H
