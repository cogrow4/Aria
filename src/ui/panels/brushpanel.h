#ifndef BRUSHPANEL_H
#define BRUSHPANEL_H

#include <QWidget>

class QSlider;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;
class Canvas;

class BrushPanel : public QWidget {
  Q_OBJECT

public:
  explicit BrushPanel(Canvas *canvas, QWidget *parent = nullptr);
  ~BrushPanel();

private slots:
  void onSizeChanged(int size);
  void onOpacityChanged(int opacity);
  void onHardnessChanged(int hardness);
  void onEraserToggled(bool checked); // Added slot

private:
  void setupUi();

  Canvas *m_canvas;

  QCheckBox *m_eraserCheckBox; // Added member variable

  QSlider *m_sizeSlider;
  QSpinBox *m_sizeSpinBox;

  QSlider *m_opacitySlider;
  QSpinBox *m_opacitySpinBox;

  QSlider *m_hardnessSlider;
  QSpinBox *m_hardnessSpinBox;
};

#endif // BRUSHPANEL_H
