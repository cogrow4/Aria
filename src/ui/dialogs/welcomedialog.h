#ifndef WELCOMEDIALOG_H
#define WELCOMEDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QSpinBox>

class WelcomeDialog : public QDialog {
  Q_OBJECT

public:
  explicit WelcomeDialog(QWidget *parent = nullptr);
  ~WelcomeDialog();

  int canvasWidth() const { return m_width; }
  int canvasHeight() const { return m_height; }

private slots:
  void onTemplateSelected(QListWidgetItem *item);
  void onCreateClicked();

private:
  void setupUi();
  void addTemplate(const QString &name, int width, int height,
                   const QString &description);

  QListWidget *m_templateList;
  QSpinBox *m_widthSpinBox;
  QSpinBox *m_heightSpinBox;

  int m_width;
  int m_height;

  struct Template {
    QString name;
    int width;
    int height;
    QString description;
  };
  QList<Template> m_templates;
};

#endif // WELCOMEDIALOG_H
