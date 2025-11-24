#include "brushpanel.h"
#include "core/brush.h"
#include "core/canvas.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>

BrushPanel::BrushPanel(Canvas *canvas, QWidget *parent)
    : QWidget(parent), m_canvas(canvas) {
  setupUi();
}

BrushPanel::~BrushPanel() {}

void BrushPanel::setupUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(15);

  // Eraser Toggle
  m_eraserCheckBox = new QCheckBox("Eraser Mode", this);
  connect(m_eraserCheckBox, &QCheckBox::toggled, this,
          &BrushPanel::onEraserToggled);
  mainLayout->addWidget(m_eraserCheckBox);

  // Brush Size
  QGroupBox *sizeGroup = new QGroupBox("Size", this);
  QVBoxLayout *sizeLayout = new QVBoxLayout(sizeGroup);

  QHBoxLayout *sizeControlLayout = new QHBoxLayout();
  m_sizeSlider = new QSlider(Qt::Horizontal, this);
  m_sizeSlider->setRange(1, 500);
  m_sizeSlider->setValue(10);

  m_sizeSpinBox = new QSpinBox(this);
  m_sizeSpinBox->setRange(1, 500);
  m_sizeSpinBox->setValue(10);

  connect(m_sizeSlider, &QSlider::valueChanged, m_sizeSpinBox,
          &QSpinBox::setValue);
  connect(m_sizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          m_sizeSlider, &QSlider::setValue);
  connect(m_sizeSlider, &QSlider::valueChanged, this,
          &BrushPanel::onSizeChanged);

  sizeControlLayout->addWidget(m_sizeSlider);
  sizeControlLayout->addWidget(m_sizeSpinBox);
  sizeLayout->addLayout(sizeControlLayout);
  mainLayout->addWidget(sizeGroup);

  // Opacity
  QGroupBox *opacityGroup = new QGroupBox("Opacity", this);
  QVBoxLayout *opacityLayout = new QVBoxLayout(opacityGroup);

  QHBoxLayout *opacityControlLayout = new QHBoxLayout();
  m_opacitySlider = new QSlider(Qt::Horizontal, this);
  m_opacitySlider->setRange(0, 100);
  m_opacitySlider->setValue(100);

  m_opacitySpinBox = new QSpinBox(this);
  m_opacitySpinBox->setRange(0, 100);
  m_opacitySpinBox->setValue(100);
  m_opacitySpinBox->setSuffix("%");

  connect(m_opacitySlider, &QSlider::valueChanged, m_opacitySpinBox,
          &QSpinBox::setValue);
  connect(m_opacitySpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          m_opacitySlider, &QSlider::setValue);
  connect(m_opacitySlider, &QSlider::valueChanged, this,
          &BrushPanel::onOpacityChanged);

  opacityControlLayout->addWidget(m_opacitySlider);
  opacityControlLayout->addWidget(m_opacitySpinBox);
  opacityLayout->addLayout(opacityControlLayout);
  mainLayout->addWidget(opacityGroup);

  // Hardness
  QGroupBox *hardnessGroup = new QGroupBox("Hardness", this);
  QVBoxLayout *hardnessLayout = new QVBoxLayout(hardnessGroup);

  QHBoxLayout *hardnessControlLayout = new QHBoxLayout();
  m_hardnessSlider = new QSlider(Qt::Horizontal, this);
  m_hardnessSlider->setRange(0, 100);
  m_hardnessSlider->setValue(100);

  m_hardnessSpinBox = new QSpinBox(this);
  m_hardnessSpinBox->setRange(0, 100);
  m_hardnessSpinBox->setValue(100);
  m_hardnessSpinBox->setSuffix("%");

  connect(m_hardnessSlider, &QSlider::valueChanged, m_hardnessSpinBox,
          &QSpinBox::setValue);
  connect(m_hardnessSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          m_hardnessSlider, &QSlider::setValue);
  connect(m_hardnessSlider, &QSlider::valueChanged, this,
          &BrushPanel::onHardnessChanged);

  hardnessControlLayout->addWidget(m_hardnessSlider);
  hardnessControlLayout->addWidget(m_hardnessSpinBox);
  hardnessLayout->addLayout(hardnessControlLayout);
  mainLayout->addWidget(hardnessGroup);

  mainLayout->addStretch();
}

void BrushPanel::onSizeChanged(int size) {
  if (m_canvas && m_canvas->brush()) {
    m_canvas->brush()->setSize(size);
  }
}

void BrushPanel::onOpacityChanged(int opacity) {
  if (m_canvas && m_canvas->brush()) {
    m_canvas->brush()->setOpacity(opacity / 100.0);
  }
}

void BrushPanel::onHardnessChanged(int hardness) {
  if (m_canvas && m_canvas->brush()) {
    m_canvas->brush()->setHardness(hardness / 100.0);
  }
}

void BrushPanel::onEraserToggled(bool checked) {
  if (m_canvas && m_canvas->brush()) {
    m_canvas->brush()->setEraser(checked);
  }
}
