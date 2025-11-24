#ifndef ARIACOLORPICKER_H
#define ARIACOLORPICKER_H

#include <QColor>
#include <QWidget>

class Canvas;
class QPushButton;

class AriaColorPicker : public QWidget {
  Q_OBJECT

public:
  explicit AriaColorPicker(Canvas *canvas, QWidget *parent = nullptr);
  ~AriaColorPicker();

public slots:
  void setColor(const QColor &color);

private slots:
  void onColorChanged(const QColor &color);
  void openColorDialog();

private:
  void setupUi();
  void updateColorButton();

  Canvas *m_canvas;
  QColor m_currentColor;
  QPushButton *m_colorButton;
};

#endif // ARIACOLORPICKER_H
