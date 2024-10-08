/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org/
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
#include <QDir>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QStyle>
#include <private/xdgiconloader/xdgiconloader_p.h>
#include <QLibrary>

#include <utility>


// Function to create a new Fm::FileDialogHelper object.
// This is dynamically loaded at runtime on demand from libfm-qt.
typedef QPlatformDialogHelper* (*CreateFileDialogHelperFunc)();
static CreateFileDialogHelperFunc createFileDialogHelper = nullptr;


LXQtPlatformTheme::LXQtPlatformTheme():
    iconFollowColorScheme_(true)
    , settingsWatcher_(nullptr)
    , LXQtPalette_(nullptr)
{
    loadSettings();
    // Note: When the plugin is loaded, it seems that the app is not yet running and
    // QThread environment is not completely set up. So creating filesystem watcher
    // does not work since it uses QSocketNotifier internally which can only be
    // created within QThread thread. So let's schedule a idle handler to initialize
    // the watcher in the main event loop.

    // Note2: the QTimer::singleShot with also needs a QThread environment
    // (there is a workaround in qtcore for the 0-timer with the old SLOT notation)

    // TODO: use the "PointerToMemberFunction" overloads of invokeMethod(), but they
    // are available from Qt v5.10
    //QMetaObject::ivokeMethod(this, &LXQtPlatformTheme::lazyInit, Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

LXQtPlatformTheme::~LXQtPlatformTheme() {
    delete LXQtPalette_;
    delete settingsWatcher_;
}

void LXQtPlatformTheme::lazyInit()
{
    settingsWatcher_ = new QFileSystemWatcher();
    settingsWatcher_->addPath(settingsFile_);
    connect(settingsWatcher_, &QFileSystemWatcher::fileChanged, this, &LXQtPlatformTheme::onSettingsChanged);

    XdgIconLoader::instance()->setFollowColorScheme(iconFollowColorScheme_);
}

void LXQtPlatformTheme::loadSettings() {
    // QSettings is really handy. It tries to read from /etc/xdg/lxqt/lxqt.conf
    // as a fallback if a key is missing from the user config file ~/.config/lxqt/lxqt.conf.
    // So we can customize the default values in /etc/xdg/lxqt/lxqt.conf and does
    // not necessarily to hard code the default values here.
    QSettings settings(QSettings::UserScope, QLatin1String("lxqt"), QLatin1String("lxqt"));
    settingsFile_ = settings.fileName();

    // icon theme
    iconTheme_ = settings.value(QLatin1String("icon_theme"), QLatin1String("oxygen")).toString();
    iconFollowColorScheme_ = settings.value(QLatin1String("icon_follow_color_scheme"), iconFollowColorScheme_).toBool();

    // read other widget related settings form LxQt settings.
    QByteArray tb_style = settings.value(QLatin1String("tool_button_style")).toByteArray();
    // convert toolbar style name to value
    QMetaEnum me = QToolBar::staticMetaObject.property(QToolBar::staticMetaObject.indexOfProperty("toolButtonStyle")).enumerator();
    int value = me.keyToValue(tb_style.constData());
    if(value == -1)
        toolButtonStyle_ = Qt::ToolButtonTextBesideIcon;
    else
        toolButtonStyle_ = static_cast<Qt::ToolButtonStyle>(value);

    // toolbar icon size
    toolBarIconSize_ = qBound(0, settings.value(QLatin1String("tool_bar_icon_size")).toInt(), 48);
    if (toolBarIconSize_ < 16)
        toolBarIconSize_ = 0; // let the style decide

    // single click activation
    singleClickActivate_ = settings.value(QLatin1String("single_click_activate")).toBool();

    // palette
    settings.beginGroup(QLatin1String("Palette"));
    paletteChanged_ = false;

    QColor color = winColor_;
    winColor_ = QColor::fromString(settings.value(QLatin1String("window_color"), QLatin1String("#efefef")).toString());
    if(!winColor_.isValid())
        winColor_ = QColor::fromString(QStringLiteral("#efefef"));
    paletteChanged_ = color != winColor_;

    color = baseColor_;
    baseColor_ = QColor::fromString(settings.value(QLatin1String("base_color")).toString());
    if (!paletteChanged_)
        paletteChanged_ = baseColor_.isValid() && color != baseColor_;

    color = highlightColor_;
    highlightColor_ = QColor::fromString(settings.value(QLatin1String("highlight_color")).toString());
    if (!paletteChanged_)
        paletteChanged_ = highlightColor_.isValid() && color != highlightColor_;

    color = winTextColor_;
    winTextColor_ = QColor::fromString(settings.value(QLatin1String("window_text_color")).toString());
    if (!paletteChanged_)
        paletteChanged_ = winTextColor_.isValid() && color != winTextColor_;

    color = textColor_;
    textColor_ = QColor::fromString(settings.value(QLatin1String("text_color")).toString());
    if (!paletteChanged_)
        paletteChanged_ = textColor_.isValid() && color != textColor_;

    color = highlightedTextColor_;
    highlightedTextColor_ = QColor::fromString(settings.value(QLatin1String("highlighted_text_color")).toString());
    if (!paletteChanged_)
        paletteChanged_ = highlightedTextColor_.isValid() && color != highlightedTextColor_;

    color = linkColor_;
    linkColor_ = QColor::fromString(settings.value(QLatin1String("link_color")).toString());
    if (!paletteChanged_)
        paletteChanged_ = linkColor_.isValid() && color != linkColor_;

    color = linkVisitedColor_;
    linkVisitedColor_ = QColor::fromString(settings.value(QLatin1String("link_visited_color")).toString());
    if (!paletteChanged_)
        paletteChanged_ = linkVisitedColor_.isValid() && color != linkVisitedColor_;

    color = tooltipBaseCol_;
    tooltipBaseCol_ = QColor::fromString(settings.value(QLatin1String("tooltip_base_color")).toString());
    if (!paletteChanged_)
        paletteChanged_ = tooltipBaseCol_.isValid() && color != tooltipBaseCol_;

    color = tooltipTextCol_;
    tooltipTextCol_ = QColor::fromString(settings.value(QLatin1String("tooltip_text_color")).toString());
    if (!paletteChanged_)
        paletteChanged_ = tooltipTextCol_.isValid() && color != tooltipTextCol_;

    if(paletteChanged_)
    {
        delete LXQtPalette_;
        // This sets all colors appropriately but valid custom colors are set below.
        // If a custom color is not valid, Qt's calculated color will be used.
        LXQtPalette_ = new QPalette(winColor_);

        if (baseColor_.isValid())
        {
            LXQtPalette_->setColor(QPalette::Base, baseColor_);
            // Qt makes the alternate base color (used by some item views) by mixing the button
            // color (= window color) and base color. That can result in unreadable texts when
            // the base and window colors have a high contrast with each other.
            color = baseColor_;
            int l = color.lightness();
            if (l < 127)
                l += 10;
            else
                l -= 10;
            color.setHsl(color.hue(), color.saturation(), l);
            LXQtPalette_->setColor(QPalette::AlternateBase, color);
        }
        if (highlightColor_.isValid())
        {
            LXQtPalette_->setColor(QPalette::Highlight, highlightColor_);
            int gray = qGray(highlightColor_.rgb());
            color = QColor(gray, gray, gray);
            LXQtPalette_->setColor(QPalette::Disabled, QPalette::Highlight, color);
        }
        else
        {
            // Qt's default highlight color and that of Fusion may be different. This is a workaround:
            LXQtPalette_->setColor(QPalette::Highlight, QColor(60, 140, 230));
            if (highlightedTextColor_.isValid())
                LXQtPalette_->setColor(QPalette::HighlightedText, QColor(255, 255, 255));
        }
        if (winTextColor_.isValid())
        {
            LXQtPalette_->setColor(QPalette::WindowText, winTextColor_);
            LXQtPalette_->setColor(QPalette::ButtonText, winTextColor_);
            color = winTextColor_;
            color.setAlpha(130);
            LXQtPalette_->setColor(QPalette::Disabled, QPalette::WindowText, color);
            LXQtPalette_->setColor(QPalette::Disabled, QPalette::ButtonText, color);
        }
        if (textColor_.isValid())
        {
            LXQtPalette_->setColor(QPalette::Text, textColor_);
            color = textColor_;
            color.setAlpha(130);
            LXQtPalette_->setColor(QPalette::Disabled, QPalette::Text, color);
        }
        if (highlightedTextColor_.isValid())
        {
            LXQtPalette_->setColor(QPalette::HighlightedText, highlightedTextColor_);
            color = highlightedTextColor_;
            color.setAlpha(130);
            LXQtPalette_->setColor(QPalette::Disabled, QPalette::HighlightedText, color);
        }
        if (linkColor_.isValid())
            LXQtPalette_->setColor(QPalette::Link, linkColor_);
        if (linkVisitedColor_.isValid())
            LXQtPalette_->setColor(QPalette::LinkVisited, linkVisitedColor_);
        if (tooltipBaseCol_.isValid())
            LXQtPalette_->setColor(QPalette::ToolTipBase, tooltipBaseCol_);
        if (tooltipTextCol_.isValid())
            LXQtPalette_->setColor(QPalette::ToolTipText, tooltipTextCol_);
    }
    settings.endGroup();

    // load Qt settings
    settings.beginGroup(QLatin1String("Qt"));

    // widget style
    style_ = settings.value(QLatin1String("style"), QLatin1String("fusion")).toString();

    // SystemFont
    fontStr_ = settings.value(QLatin1String("font")).toString();

    if(!fontStr_.isEmpty()) {
        if(font_.fromString(fontStr_)) {
            if(qobject_cast<QApplication *>(QCoreApplication::instance())) {
                QApplication::setFont(font_); // it seems that we need to do this manually.
            }
        }
    }

    // FixedFont
    fixedFontStr_ = settings.value(QLatin1String("fixedFont")).toString();
    if(!fixedFontStr_.isEmpty()) {
        fixedFont_.fromString(fixedFontStr_);
    }

    // mouse
    doubleClickInterval_ = settings.value(QLatin1String("doubleClickInterval"));
    wheelScrollLines_ = settings.value(QLatin1String("wheelScrollLines"));

    // keyboard
    cursorFlashTime_ = settings.value(QLatin1String("cursorFlashTime"));
    settings.endGroup();

    // mouse cursor
    QSettings sessionSettings(QSettings::UserScope, QLatin1String("lxqt"), QLatin1String("session"));
    sessionSettings.beginGroup(QStringLiteral("Mouse"));
    mouseCursorTheme_ = sessionSettings.value(QLatin1String("cursor_theme"));
    int curSize = sessionSettings.value(QLatin1String("cursor_size"), 16).toInt();
    mouseCursorSize_ = QSize(curSize, curSize);
    sessionSettings.endGroup();
}

// this is called whenever the config file is changed.
void LXQtPlatformTheme::onSettingsChanged() {
    // D*mn! yet another Qt 5.4 regression!!!
    // See the bug report: https://github.com/lxqt/lxqt/issues/441
    // Since Qt 5.4, QSettings uses QSaveFile to save the config files.
    // https://github.com/qtproject/qtbase/commit/8d15068911d7c0ba05732e2796aaa7a90e34a6a1#diff-e691c0405f02f3478f4f50a27bdaecde
    // QSaveFile will save the content to a new temp file, and replace the old file later.
    // Hence the existing config file is not changed. Instead, it's deleted and then replaced.
    // This new behaviour unfortunately breaks QFileSystemWatcher.
    // After file deletion, we can no longer receive any new change notifications.
    // The most ridiculous thing is, QFileSystemWatcher does not provide a
    // way for us to know if a file is deleted. WT*?
    // Luckily, I found a workaround: If the file path no longer exists
    // in the watcher's files(), this file is deleted.
    bool file_deleted = !settingsWatcher_->files().contains(settingsFile_);
    if(file_deleted) // if our config file is already deleted, reinstall a new watcher
        settingsWatcher_->addPath(settingsFile_);

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

    auto app = qobject_cast<QApplication *>(QCoreApplication::instance());

    if(app && (style_ != oldStyle || paletteChanged_)) // the widget style or palette is changed
    {
        // ask Qt to apply the new style
        QApplication::setStyle(style_);
        // Qt 5.15 needs this and it's safe otherwise
        if(LXQtPalette_ != nullptr)
        {
            QApplication::setPalette(*LXQtPalette_);
            // the app should be polished because the style may have an internal palette
            QApplication::style()->polish(app);
        }
    }

    if(iconTheme_ != oldIconTheme) { // the icon theme is changed
        XdgIconLoader::instance()->updateSystemTheme(); // this is a private internal API of Qt.
    }
    XdgIconLoader::instance()->setFollowColorScheme(iconFollowColorScheme_);

    // if font is changed
    if(oldFont != fontStr_ || oldFixedFont != fixedFontStr_){
        // FIXME: to my knowledge there is no way to ask Qt to reload the fonts.
        // Should we call QApplication::setFont() to override the font?
        // This does not work with QSS, though.
        //
        // After reading the source code of Qt, I think the right method to call
        // here is QApplicationPrivate::setSystemFont(). However, this is an
        // internal API and should not be used. Let's call QApplication::setFont()
        // instead since it approximately does the same thing.
        // Internally, QApplication will emit QEvent::ApplicationFontChange so
        // all of the widgets will update their fonts.
        // FIXME: should we call the internal API: QApplicationPrivate::setFont() instead?
        // QGtkStyle does this internally.
        fixedFont_.fromString(fixedFontStr_); // FIXME: how to set this to the app?
        if(font_.fromString(fontStr_)) {
            if(app) {
                QApplication::setFont(font_);
            }
        }
    }

    if(app) {
        QApplication::setWheelScrollLines(wheelScrollLines_.toInt());

        // emit a ThemeChange event to all widgets
        const auto widgets = QApplication::allWidgets();
        for(QWidget* const widget : widgets) {
            // Qt5 added a QEvent::ThemeChange event.
            QEvent event(QEvent::ThemeChange);
            QApplication::sendEvent(widget, &event);
        }
    }
}

bool LXQtPlatformTheme::usePlatformNativeDialog(DialogType type) const {
    if(type == FileDialog
       && (qobject_cast<QApplication *>(QCoreApplication::instance()) != nullptr)) { // QML may not have qApp
        // use our own file dialog
        return true;
    }
    return false;
}


QPlatformDialogHelper *LXQtPlatformTheme::createPlatformDialogHelper(DialogType type) const {
    if(type == FileDialog
       && (qobject_cast<QApplication *>(QCoreApplication::instance()) != nullptr)) { // QML may not have qApp
        // use our own file dialog provided by libfm

        // When a process has this environment set, that means glib event loop integration is disabled.
        // In this case, libfm-qt just won't work. So let's disable the file dialog helper and return nullptr.
        if(QString::fromLocal8Bit(qgetenv("QT_NO_GLIB")) == QLatin1String("1")) {
            return nullptr;
        }

        // The createFileDialogHelper() method is dynamically loaded from libfm-qt on demand
        if(createFileDialogHelper == nullptr) {
            // try to dynamically load versioned libfm-qt.so
            QLibrary libfmQtLibrary{QLatin1String(LIB_FM_QT_SONAME)};
            libfmQtLibrary.load();
            if(!libfmQtLibrary.isLoaded()) {
                return nullptr;
            }

            // try to resolve the symbol to get the function pointer
            createFileDialogHelper = reinterpret_cast<CreateFileDialogHelperFunc>(libfmQtLibrary.resolve("createFileDialogHelper"));
            if(createFileDialogHelper == nullptr) {
                return nullptr;
            }
        }

        // create a new file dialog helper provided by libfm
        return createFileDialogHelper();
    }
    return nullptr;
}

const QPalette *LXQtPlatformTheme::palette(Palette type) const {
    if(type == QPlatformTheme::SystemPalette) {
        if(LXQtPalette_ != nullptr)
            return LXQtPalette_;
    }
    return nullptr;
}

const QFont *LXQtPlatformTheme::font(Font type) const {
    if(type == SystemFont && !fontStr_.isEmpty()) {
        // NOTE: for some reason, this is not called by Qt when program startup.
        // So we do QApplication::setFont() manually.
        return &font_;
    }
    else if(type == FixedFont && !fixedFontStr_.isEmpty()) {
        return &fixedFont_;
    }
    return QPlatformTheme::font(type);
}

QVariant LXQtPlatformTheme::themeHint(ThemeHint hint) const {
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
        return QVariant(toolBarIconSize_);
    case ItemViewActivateItemOnSingleClick:
        return QVariant(singleClickActivate_);
    case SystemIconThemeName:
        return iconTheme_;
    case SystemIconFallbackThemeName:
        return QLatin1String("hicolor");
    case IconThemeSearchPaths:
        return xdgIconThemePaths();
    case StyleNames:
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
        return QVariant(static_cast<int>(HoverEffect));
    case SpellCheckUnderlineStyle:
        break;
    case IconPixmapSizes:
        break;
    case PasswordMaskCharacter:
        break;
    case DialogSnapToDefaultButton:
        break;
    case ContextMenuOnMouseRelease:
        break;
    case MousePressAndHoldInterval:
        break;
    case MouseDoubleClickDistance:
        break;
    case WheelScrollLines:
        return wheelScrollLines_;
    case QPlatformTheme::ShowShortcutsInContextMenus:
        return QVariant(true);
    case MouseCursorTheme:
        return mouseCursorTheme_;
    case MouseCursorSize:
        return QVariant(mouseCursorSize_);
    default:
        break;
    }
    return QPlatformTheme::themeHint(hint);
}

QIconEngine *LXQtPlatformTheme::createIconEngine(const QString &iconName) const
{
    return new XdgIconLoaderEngine(iconName);
}

// Helper to return the icon theme paths from XDG.
QStringList LXQtPlatformTheme::xdgIconThemePaths() const
{
    QStringList paths;
    QStringList xdgDirs;

    // Add home directory first in search path
    const QFileInfo homeIconDir(QDir::homePath() + QStringLiteral("/.icons"));
    if (homeIconDir.isDir())
        paths.prepend(homeIconDir.absoluteFilePath());

    QString xdgDataHome = QFile::decodeName(qgetenv("XDG_DATA_HOME"));
    if (xdgDataHome.isEmpty())
        xdgDataHome = QDir::homePath() + QLatin1String("/.local/share");
    xdgDirs.append(xdgDataHome);

    QString xdgDataDirs = QFile::decodeName(qgetenv("XDG_DATA_DIRS"));
    if (xdgDataDirs.isEmpty())
        xdgDataDirs = QLatin1String("/usr/local/share/:/usr/share/");
    xdgDirs.append(xdgDataDirs);

    for (const auto &s: std::as_const(xdgDirs)) {
        const QStringList r = s.split(QLatin1Char(':'), Qt::SkipEmptyParts);
        for (const auto& xdgDir: r) {
            const QFileInfo xdgIconsDir(xdgDir + QStringLiteral("/icons"));
            if (xdgIconsDir.isDir())
                paths.append(xdgIconsDir.absoluteFilePath());
        }
    }
    paths.removeDuplicates();
    return paths;
}
