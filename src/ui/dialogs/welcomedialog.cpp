#include "welcomedialog.h"
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

WelcomeDialog::WelcomeDialog(QWidget *parent)
    : QDialog(parent), m_width(800), m_height(600) {
  setupUi();
}

WelcomeDialog::~WelcomeDialog() {}

void WelcomeDialog::setupUi() {
  setWindowTitle("Welcome to Aria");
  setMinimumSize(600, 450);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(15);
  mainLayout->setContentsMargins(20, 20, 20, 20);

  // Title
  QLabel *titleLabel = new QLabel("Create New Image");
  QFont titleFont = titleLabel->font();
  titleFont.setPointSize(16);
  titleFont.setBold(true);
  titleLabel->setFont(titleFont);
  mainLayout->addWidget(titleLabel);

  // Templates section
  QGroupBox *templatesGroup = new QGroupBox("Templates");
  QVBoxLayout *templatesLayout = new QVBoxLayout(templatesGroup);

  m_templateList = new QListWidget();
  m_templateList->setMaximumHeight(200);
  templatesLayout->addWidget(m_templateList);

  // Add common templates
  addTemplate("HD (1920×1080)", 1920, 1080, "Full HD resolution");
  addTemplate("A4 (300 DPI)", 2480, 3508, "Print quality A4");
  addTemplate("Square (1000×1000)", 1000, 1000, "Social media square");
  addTemplate("4K (3840×2160)", 3840, 2160, "4K UHD resolution");
  addTemplate("Desktop Wallpaper", 2560, 1440, "2K desktop wallpaper");
  addTemplate("Mobile (1080×1920)", 1080, 1920, "Mobile screen portrait");
  addTemplate("Custom", 800, 600, "Choose your own size");

  // Set the first item as selected by default
  m_templateList->setCurrentRow(0);

  connect(m_templateList, &QListWidget::itemClicked, this,
          &WelcomeDialog::onTemplateSelected);

  mainLayout->addWidget(templatesGroup);

  // Custom size section
  QGroupBox *sizeGroup = new QGroupBox("Canvas Size");
  QFormLayout *sizeLayout = new QFormLayout(sizeGroup);

  m_widthSpinBox = new QSpinBox();
  m_widthSpinBox->setRange(1, 16384);
  m_widthSpinBox->setValue(m_width);
  m_widthSpinBox->setSuffix(" px");
  sizeLayout->addRow("Width:", m_widthSpinBox);

  m_heightSpinBox = new QSpinBox();
  m_heightSpinBox->setRange(1, 16384);
  m_heightSpinBox->setValue(m_height);
  m_heightSpinBox->setSuffix(" px");
  sizeLayout->addRow("Height:", m_heightSpinBox);

  mainLayout->addWidget(sizeGroup);

  // Buttons
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->addStretch();

  QPushButton *cancelButton = new QPushButton("Cancel");
  connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
  buttonLayout->addWidget(cancelButton);

  QPushButton *createButton = new QPushButton("Create");
  createButton->setDefault(true);
  connect(createButton, &QPushButton::clicked, this,
          &WelcomeDialog::onCreateClicked);
  buttonLayout->addWidget(createButton);

  mainLayout->addLayout(buttonLayout);

  // Apply modern dark styling
  setStyleSheet(R"(
    QDialog {
      background-color: #2b2b2b;
      color: #e0e0e0;
    }
    QGroupBox {
      border: 2px solid #404040;
      border-radius: 6px;
      margin-top: 12px;
      padding-top: 12px;
      font-weight: bold;
      color: #e0e0e0;
    }
    QGroupBox::title {
      subcontrol-origin: margin;
      subcontrol-position: top left;
      padding: 0 5px;
      color: #4a9eff;
    }
    QListWidget {
      background-color: #1e1e1e;
      border: 1px solid #404040;
      border-radius: 4px;
      color: #e0e0e0;
      padding: 4px;
    }
    QListWidget::item {
      padding: 8px;
      border-radius: 3px;
    }
    QListWidget::item:selected {
      background-color: #4a9eff;
      color: #ffffff;
    }
    QListWidget::item:hover {
      background-color: #363636;
    }
    QSpinBox {
      background-color: #1e1e1e;
      border: 1px solid #404040;
      border-radius: 4px;
      color: #e0e0e0;
      padding: 5px;
    }
    QPushButton {
      background-color: #4a9eff;
      color: #ffffff;
      border: none;
      border-radius: 4px;
      padding: 8px 20px;
      font-weight: bold;
    }
    QPushButton:hover {
      background-color: #5aa6ff;
    }
    QPushButton:pressed {
      background-color: #3a8eef;
    }
    QPushButton#cancelButton {
      background-color: #404040;
      color: #e0e0e0;
    }
    QPushButton#cancelButton:hover {
      background-color: #505050;
    }
  )");

  cancelButton->setObjectName("cancelButton");
}

void WelcomeDialog::addTemplate(const QString &name, int width, int height,
                                const QString &description) {
  Template tmpl;
  tmpl.name = name;
  tmpl.width = width;
  tmpl.height = height;
  tmpl.description = description;
  m_templates.append(tmpl);

  QListWidgetItem *item = new QListWidgetItem();
  item->setText(QString("%1\n%2×%3 - %4")
                    .arg(name)
                    .arg(width)
                    .arg(height)
                    .arg(description));
  item->setData(Qt::UserRole, m_templates.size() - 1);
  m_templateList->addItem(item);
}

void WelcomeDialog::onTemplateSelected(QListWidgetItem *item) {
  int index = item->data(Qt::UserRole).toInt();
  if (index >= 0 && index < m_templates.size()) {
    const Template &tmpl = m_templates[index];
    m_widthSpinBox->setValue(tmpl.width);
    m_heightSpinBox->setValue(tmpl.height);
  }
}

void WelcomeDialog::onCreateClicked() {
  m_width = m_widthSpinBox->value();
  m_height = m_heightSpinBox->value();
  accept();
}
