#include "shortcutmanager.h"

ShortcutManager *ShortcutManager::s_instance = nullptr;

ShortcutManager::ShortcutManager(QObject *parent) : QObject(parent) {}

ShortcutManager::~ShortcutManager() {}

ShortcutManager *ShortcutManager::instance() {
  if (!s_instance) {
    s_instance = new ShortcutManager();
    s_instance->loadSettings();
  }
  return s_instance;
}

void ShortcutManager::registerAction(const QString &actionId, QAction *action,
                                     const QKeySequence &defaultShortcut) {
  ActionInfo info;
  info.action = action;
  info.defaultShortcut = defaultShortcut;
  info.name = action->text().remove('&'); // Remove accelerator markers

  m_actions[actionId] = info;
  action->setShortcut(defaultShortcut);
}

QKeySequence ShortcutManager::shortcut(const QString &actionId) const {
  if (m_actions.contains(actionId)) {
    return m_actions[actionId].action->shortcut();
  }
  return QKeySequence();
}

void ShortcutManager::setShortcut(const QString &actionId,
                                  const QKeySequence &shortcut) {
  if (m_actions.contains(actionId)) {
    m_actions[actionId].action->setShortcut(shortcut);
    emit shortcutChanged(actionId, shortcut);
  }
}

void ShortcutManager::resetToDefaults(const QString &actionId) {
  if (m_actions.contains(actionId)) {
    QKeySequence defaultShortcut = m_actions[actionId].defaultShortcut;
    setShortcut(actionId, defaultShortcut);
  }
}

void ShortcutManager::resetAllToDefaults() {
  for (auto it = m_actions.begin(); it != m_actions.end(); ++it) {
    it.value().action->setShortcut(it.value().defaultShortcut);
  }
}

void ShortcutManager::saveSettings() {
  QSettings settings("Aria", "Aria");
  settings.beginGroup("Shortcuts");

  for (auto it = m_actions.begin(); it != m_actions.end(); ++it) {
    settings.setValue(it.key(), it.value().action->shortcut().toString());
  }

  settings.endGroup();
}

void ShortcutManager::loadSettings() {
  QSettings settings("Aria", "Aria");
  settings.beginGroup("Shortcuts");

  for (auto it = m_actions.begin(); it != m_actions.end(); ++it) {
    QString shortcutStr = settings.value(it.key()).toString();
    if (!shortcutStr.isEmpty()) {
      it.value().action->setShortcut(QKeySequence(shortcutStr));
    }
  }

  settings.endGroup();
}

QStringList ShortcutManager::actionIds() const { return m_actions.keys(); }

QString ShortcutManager::actionName(const QString &actionId) const {
  if (m_actions.contains(actionId)) {
    return m_actions[actionId].name;
  }
  return QString();
}

QKeySequence ShortcutManager::defaultShortcut(const QString &actionId) const {
  if (m_actions.contains(actionId)) {
    return m_actions[actionId].defaultShortcut;
  }
  return QKeySequence();
}

QString ShortcutManager::findConflict(const QString &actionId,
                                      const QKeySequence &shortcut) const {
  if (shortcut.isEmpty()) {
    return QString();
  }

  for (auto it = m_actions.begin(); it != m_actions.end(); ++it) {
    if (it.key() != actionId && it.value().action->shortcut() == shortcut) {
      return it.value().name;
    }
  }

  return QString();
}
