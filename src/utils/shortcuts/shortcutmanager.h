#ifndef SHORTCUTMANAGER_H
#define SHORTCUTMANAGER_H

#include <QAction>
#include <QHash>
#include <QKeySequence>
#include <QObject>
#include <QSettings>

class ShortcutManager : public QObject {
  Q_OBJECT

public:
  static ShortcutManager *instance();

  // Register an action with a default shortcut
  void registerAction(const QString &actionId, QAction *action,
                      const QKeySequence &defaultShortcut);

  // Get/set shortcuts
  QKeySequence shortcut(const QString &actionId) const;
  void setShortcut(const QString &actionId, const QKeySequence &shortcut);

  // Reset to defaults
  void resetToDefaults(const QString &actionId);
  void resetAllToDefaults();

  // Save/load settings
  void saveSettings();
  void loadSettings();

  // Get all registered actions
  QStringList actionIds() const;
  QString actionName(const QString &actionId) const;
  QKeySequence defaultShortcut(const QString &actionId) const;

  // Check for conflicts
  QString findConflict(const QString &actionId,
                       const QKeySequence &shortcut) const;

signals:
  void shortcutChanged(const QString &actionId, const QKeySequence &shortcut);

private:
  explicit ShortcutManager(QObject *parent = nullptr);
  ~ShortcutManager();

  struct ActionInfo {
    QAction *action;
    QKeySequence defaultShortcut;
    QString name;
  };

  QHash<QString, ActionInfo> m_actions;
  static ShortcutManager *s_instance;
};

#endif // SHORTCUTMANAGER_H
