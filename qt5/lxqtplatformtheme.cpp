/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://lxde.org/
 *
 * Copyright: 2014 LXQt team
 * Authors:
 *   Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "lxqtplatformtheme.h"
#include <QVariant>
#include <QDebug>

#include <QIcon>
#include <QStandardPaths>
#include <QApplication>
#include <QWidget>

#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaEnum>
#include <QToolBar>

LXQtPlatformTheme::LXQtPlatformTheme() {
    // qDebug() << "LXQT Platform Theme loaded";
    connect(LxQt::Settings::globalSettings(), SIGNAL(iconThemeChanged()), SLOT(onIconThemeChanged()));
    connect(LxQt::Settings::globalSettings(), SIGNAL(settingsChanged()), SLOT(onSettingsChanged()));

    loadIconTheme();
    loadSettings();
}

LXQtPlatformTheme::~LXQtPlatformTheme() {
}

void LXQtPlatformTheme::loadIconTheme() {
  iconTheme_ = LxQt::Settings::globalSettings()->value("icon_theme").toString();
}

void LXQtPlatformTheme::onIconThemeChanged() {
  loadIconTheme();
  notifyChange();
}

void LXQtPlatformTheme::loadSettings() {
  // read other widget related settings form LxQt settings.
  QByteArray tb_style = LxQt::Settings::globalSettings()->value("tool_button_style").toByteArray();
  // convert toolbar style name to value
  QMetaEnum me = QToolBar::staticMetaObject.property(QToolBar::staticMetaObject.indexOfProperty("toolButtonStyle")).enumerator();
  int value = me.keyToValue(tb_style.constData());
  if(value == -1)
    toolButtonStyle_ = Qt::ToolButtonTextBesideIcon;
  else
	toolButtonStyle_ = static_cast<Qt::ToolButtonStyle>(value);

  singleClickActivate_ = LxQt::Settings::globalSettings()->value("single_click_activate").toBool();
}

void LXQtPlatformTheme::onSettingsChanged() {
  loadSettings();
  notifyChange();
}

void LXQtPlatformTheme::notifyChange() {
  Q_FOREACH(QWidget* widget, QApplication::allWidgets()) {
    QEvent event(QEvent::StyleChange);
    QApplication::sendEvent(widget, &event);
  }
}

bool LXQtPlatformTheme::usePlatformNativeDialog(DialogType type) const {
    return false;
}

#if 0
QPlatformDialogHelper *LXQtPlatformTheme::createPlatformDialogHelper(DialogType type) const {
    return 0;
}

const QPalette *LXQtPlatformTheme::palette(Palette type) const {
    return new QPalette();
}

const QFont *LXQtPlatformTheme::font(Font type) const {
    return NULL;
}
#endif

QVariant LXQtPlatformTheme::themeHint(ThemeHint hint) const {
    qDebug() << "themeHint" << hint;
    switch (hint) {
    case SystemIconThemeName:
        qDebug() << "iconTheme" << iconTheme_;
        return iconTheme_;
    case IconThemeSearchPaths: // FIXME: should use XDG_DATA_DIRS instead
        return QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("icons"), QStandardPaths::LocateDirectory)
            << QStandardPaths::locate(QStandardPaths::HomeLocation, QStringLiteral(".icons"), QStandardPaths::LocateDirectory);
    case StyleNames: // FIXME: do not hard code fusion
        qDebug() << "StyleNames";
        return QStringList() << "fusion";
    case ToolButtonStyle: {
        return QVariant(toolButtonStyle_);
    }
    case ToolBarIconSize:
        break;
    case ItemViewActivateItemOnSingleClick:
        return QVariant(singleClickActivate_);
    case DropShadow:
        return QVariant(true);
    case DialogButtonBoxLayout:
        break;
    case DialogButtonBoxButtonsHaveIcons:
        return QVariant(true);
    case UiEffects:
        break;
    default:
        break;
    }
    return QPlatformTheme::themeHint(hint);
}
