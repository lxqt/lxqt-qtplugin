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
#include <QSettings>
#include <QTimer>
#include <QFileSystemWatcher>
#include "qiconloader_p.h"

LXQtPlatformTheme::LXQtPlatformTheme():
    settingsWatcher_(NULL)
{
    // qDebug() << "LXQT Platform Theme loaded";
    // When the plugin is loaded, it seems that the app is not yet running and
    // QThread environment is not completely set up. So creating filesystem watcher
    // does not work since it uses QSocketNotifier internally which can only be
    // created within QThread thread. So let's schedule a idle handler to initialize
    // the watcher in the main event loop.
    loadSettings();
    QTimer::singleShot(0, this, SLOT(initWatch()));
}

LXQtPlatformTheme::~LXQtPlatformTheme() {
    if(settingsWatcher_)
        delete settingsWatcher_;
}

void LXQtPlatformTheme::initWatch()
{
    settingsWatcher_ = new QFileSystemWatcher();
    settingsWatcher_->addPath(settingsFile_);
    connect(settingsWatcher_, SIGNAL(fileChanged(QString)), SLOT(onSettingsChanged()));
}

void LXQtPlatformTheme::loadSettings() {
    // QSettings is really handy. It tries to read from /etc/xdg/lxqt/lxqt.conf
    // as a fallback if a key is missing from the user config file ~/.config/lxqt/lxqt.conf.
    // So we can customize the default values in /etc/xdg/lxqt/lxqt.conf and does
    // not necessarily to hard code the default values here.
    QSettings settings(QSettings::UserScope, "lxqt", "lxqt");
    settingsFile_ = settings.fileName();

    // icon theme
    iconTheme_ = settings.value("icon_theme", "oxygen").toString();

    // read other widget related settings form LxQt settings.
    QByteArray tb_style = settings.value("tool_button_style").toByteArray();
    // convert toolbar style name to value
    QMetaEnum me = QToolBar::staticMetaObject.property(QToolBar::staticMetaObject.indexOfProperty("toolButtonStyle")).enumerator();
    int value = me.keyToValue(tb_style.constData());
    if(value == -1)
        toolButtonStyle_ = Qt::ToolButtonTextBesideIcon;
    else
        toolButtonStyle_ = static_cast<Qt::ToolButtonStyle>(value);

    // single click activation
    singleClickActivate_ = settings.value("single_click_activate").toBool();

    // load Qt settings
    settings.beginGroup(QLatin1String("Qt"));

    // widget style
    style_ = settings.value(QLatin1String("style"), QLatin1String("fusion")).toString();

    // SystemFont
    fontStr_ = settings.value(QLatin1String("font")).toString();
    if(!fontStr_.isEmpty()) {
        font_.fromString(fontStr_);
        // qDebug() << "font: " << font_.toString();
    }

    // FixedFont
    fixedFontStr_ = settings.value(QLatin1String("fixedFont")).toString();
    if(!fixedFontStr_.isEmpty()) {
        fixedFont_.fromString(fixedFontStr_);
        // qDebug() << "fixedFont: " << fixedFont_.toString();
    }

    // mouse
    doubleClickInterval_ = settings.value(QLatin1String("doubleClickInterval"));
    wheelScrollLines_ = settings.value(QLatin1String("wheelScrollLines"));

    // keyboard
    cursorFlashTime_ = settings.value(QLatin1String("cursorFlashTime"));
    settings.endGroup();
}

// this is called whenever the config file is changed.
void LXQtPlatformTheme::onSettingsChanged() {
    // qDebug() << "onSettingsChanged";

    // NOTE: in Qt4, Qt monitors the change of _QT_SETTINGS_TIMESTAMP root property and
    // reload Trolltech.conf when the value is changed. Then, it automatically
    // applies the new settings.
    // Unfortunately, this is no longer the case in Qt5. Yes, yet another regression.
    // We're free to provide our own platform plugin, but Qt5 does not
    // update the settings and repaint the UI. We need to do it ourselves
    // through dirty hacks and private Qt internal APIs.
    QString oldStyle = style_;
    QString oldIconTheme = iconTheme_;
    QString oldFont = fontStr_;
    QString oldFixedFont = fixedFontStr_;

    loadSettings(); // reload the config file

    if(style_ != oldStyle) // the widget style is changed
        qApp->setStyle(style_); // ask Qt5 to apply the new style

    if(iconTheme_ != oldIconTheme) // the icon theme is changed
        QIconLoader::instance()->updateSystemTheme(); // this is a private internal API of Qt5.

    // if font is changed
    if(oldFont != fontStr_ || oldFixedFont != fixedFontStr_){
        // FIXME: to my knowledge there is no way to ask Qt to reload the fonts.
        // Should we call QApplication::setFont() to override the font?
        // This does not work with QSS, though.
    }

    // emit a ThemeChange event to all widgets
    Q_FOREACH(QWidget* widget, QApplication::allWidgets()) {
        // Qt5 added a QEvent::ThemeChange event.
        QEvent event(QEvent::ThemeChange);
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

#endif

const QFont *LXQtPlatformTheme::font(Font type) const {
    if(type == SystemFont && !fontStr_.isEmpty()) {
        return &font_;
    }
    else if(type == FixedFont && !fixedFontStr_.isEmpty()) {
        return &fixedFont_;
    }
    return QPlatformTheme::font(type);
}

QVariant LXQtPlatformTheme::themeHint(ThemeHint hint) const {
    // qDebug() << "themeHint" << hint;
    switch (hint) {
    case CursorFlashTime:
        return cursorFlashTime_;
    case KeyboardInputInterval:
        break;
    case MouseDoubleClickInterval:
        return doubleClickInterval_;
    case StartDragDistance:
        break;
    case StartDragTime:
        break;
    case KeyboardAutoRepeatRate:
        break;
    case PasswordMaskDelay:
        break;
    case StartDragVelocity:
        break;
    case TextCursorWidth:
        break;
    case DropShadow:
        return QVariant(true);
    case MaximumScrollBarDragDistance:
        break;
    case ToolButtonStyle:
        return QVariant(toolButtonStyle_);
    case ToolBarIconSize:
        break;
    case ItemViewActivateItemOnSingleClick:
        return QVariant(singleClickActivate_);
    case SystemIconThemeName:
        qDebug() << "iconTheme" << iconTheme_;
        return iconTheme_;
    case SystemIconFallbackThemeName:
        return "hicolor";
    case IconThemeSearchPaths: // FIXME: should use XDG_DATA_DIRS instead
        return QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("icons"), QStandardPaths::LocateDirectory)
            << QStandardPaths::locate(QStandardPaths::HomeLocation, QStringLiteral(".icons"), QStandardPaths::LocateDirectory);
    case StyleNames:
        // qDebug() << "StyleNames";
        return QStringList() << style_;
    case WindowAutoPlacement:
        break;
    case DialogButtonBoxLayout:
        break;
    case DialogButtonBoxButtonsHaveIcons:
        return QVariant(true);
    case UseFullScreenForPopupMenu:
        break;
    case KeyboardScheme:
        return QVariant(X11KeyboardScheme);
    case UiEffects:
        break;
    case SpellCheckUnderlineStyle:
        break;
    case TabAllWidgets:
        break;
    case IconPixmapSizes:
        break;
    case PasswordMaskCharacter:
        break;
    case DialogSnapToDefaultButton:
        break;
    case ContextMenuOnMouseRelease:
        break;
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
    // this was introduced in Qt 5.3.
    case MousePressAndHoldInterval:
        break;
#endif
    case MouseDoubleClickDistance:
        break;
    default:
        break;
    }
    return QPlatformTheme::themeHint(hint);
}
