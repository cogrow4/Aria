#include "ariacolorpicker.h"
#include "core/brush.h"
#include "core/canvas.h"

#include <QColorDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

AriaColorPicker::AriaColorPicker(Canvas *canvas, QWidget *parent)
    : QWidget(parent), m_canvas(canvas), m_currentColor(Qt::black) {
  setupUi();
}

AriaColorPicker::~AriaColorPicker() {}

void AriaColorPicker::setupUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(5, 5, 5, 5);

  m_colorButton = new QPushButton(this);
  m_colorButton->setFixedHeight(30);
  updateColorButton();

  connect(m_colorButton, &QPushButton::clicked, this,
          &AriaColorPicker::openColorDialog);

  layout->addWidget(m_colorButton);
}

void AriaColorPicker::updateColorButton() {
  QString style =
      QString(
          "background-color: %1; border: 1px solid #555; border-radius: 4px;")
          .arg(m_currentColor.name());
  m_colorButton->setStyleSheet(style);
}

void AriaColorPicker::openColorDialog() {
  QColor color = QColorDialog::getColor(
      m_currentColor, static_cast<QWidget *>(this), "Select Color");
  if (color.isValid()) {
    onColorChanged(color);
  }
}

void AriaColorPicker::onColorChanged(const QColor &color) {
  m_currentColor = color;
  updateColorButton();

  if (m_canvas && m_canvas->brush()) {
    m_canvas->brush()->setColor(color);
  }
}

void AriaColorPicker::setColor(const QColor &color) {
  m_currentColor = color;
  updateColorButton();
  // We don't need to update brush here because this is usually called when
  // brush is already updated (e.g. from eyedropper) But to be safe/consistent,
  // we could. However, if it comes from Canvas::colorPicked, brush is already
  // set. Let's just update UI.
}
