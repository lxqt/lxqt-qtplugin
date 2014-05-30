/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://lxde.org/
 *
 * Copyright: 2013 LXDE-Qt team
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

#include "lxqtguiplatformplugin.h"
#include <QDebug>
#include <QIcon>
#include <QFileInfo>
#include <QApplication>
#include <QWidget>

#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaEnum>
#include <QToolBar>

Q_EXPORT_PLUGIN2(LxQtGuiPlatformPlugin, LxQtGuiPlatformPlugin)

LxQtGuiPlatformPlugin::LxQtGuiPlatformPlugin() {
  // qDebug() << "LxQtGuiPlatformPlugin constructed";
  connect(LxQt::Settings::globalSettings(), SIGNAL(iconThemeChanged()), SLOT(onIconThemeChanged()));
  connect(LxQt::Settings::globalSettings(), SIGNAL(settingsChanged()), SLOT(onSettingsChanged()));

  loadIconTheme();
  loadSettings();
}

LxQtGuiPlatformPlugin::~LxQtGuiPlatformPlugin() {
}

// actually, this is never called if style is specified in Trolltech.conf.
QString LxQtGuiPlatformPlugin::styleName() {
  // qDebug() << "LxQtGuiPlatformPlugin::styleName() is called";
  return "Cleanlooks"; // by default, choose "Cleanlooks"
}

// actually, this is never called if a palette is specified in Trolltech.conf.
QPalette LxQtGuiPlatformPlugin::palette() {
  return QGuiPlatformPlugin::palette();
}

QString LxQtGuiPlatformPlugin::systemIconThemeName() {
  // qDebug() << "LxQtGuiPlatformPlugin::systemIconThemeName() is called";
  return iconTheme_;
}

/*
QIcon LxQtGuiPlatformPlugin::fileSystemIcon(const QFileInfo& info) {
  return QGuiPlatformPlugin::fileSystemIcon(info);
}

QStringList LxQtGuiPlatformPlugin::iconThemeSearchPaths() {
  return QGuiPlatformPlugin::iconThemeSearchPaths();
}
*/

int LxQtGuiPlatformPlugin::platformHint(QGuiPlatformPlugin::PlatformHint hint) {
  // qDebug() << "LxQtGuiPlatformPlugin::platformHint() is called";
  int ret = 0;
  switch(hint) {
    case PH_ToolButtonStyle:
      ret = toolButtonStyle_;
      break;
    case PH_ToolBarIconSize:
      break;
    case PH_ItemView_ActivateItemOnSingleClick:
      ret = singleClickActivate_;
      break;
    default:
      return QGuiPlatformPlugin::platformHint(hint);
  }
  return ret;
}

void LxQtGuiPlatformPlugin::loadIconTheme() {
  iconTheme_ = LxQt::Settings::globalSettings()->value("icon_theme").toString();
}

void LxQtGuiPlatformPlugin::onIconThemeChanged() {
  loadIconTheme();
  notifyChange();
}

void LxQtGuiPlatformPlugin::loadSettings() {
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

void LxQtGuiPlatformPlugin::onSettingsChanged() {
  loadSettings();
  notifyChange();
}

void LxQtGuiPlatformPlugin::notifyChange() {
  Q_FOREACH(QWidget* widget, QApplication::allWidgets()) {
    QEvent event(QEvent::StyleChange);
    QApplication::sendEvent(widget, &event);
  }
}
