/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org/
 *
 * Copyright: 2015 LXQt team
 * Authors:
 *   Paulo Lieuthier <paulolieuthier@gmail.com>
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
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef LXQTSYSTEMTRAYICON_H
#define LXQTSYSTEMTRAYICON_H

#include <qpa/qplatformmenu.h>
#include <qpa/qplatformsystemtrayicon.h>

#include "statusnotifieritem/statusnotifieritem.h"

class SystemTrayMenuItem;
class QAction;
class QMenu;

class SystemTrayMenu : public QPlatformMenu
{
    Q_OBJECT
public:
    SystemTrayMenu();
    ~SystemTrayMenu() override;
    void insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before) override;
    QPlatformMenuItem *menuItemAt(int position) const override;
    QPlatformMenuItem *menuItemForTag(quintptr tag) const override;
    void removeMenuItem(QPlatformMenuItem *menuItem) override;
    void setEnabled(bool enabled) override;
    void setIcon(const QIcon &icon) override;
    void setTag(quintptr tag) override;
    void setText(const QString &text) override;
    void setVisible(bool visible) override;
    void syncMenuItem(QPlatformMenuItem *menuItem) override;
    void syncSeparatorsCollapsible(bool enable) override;
    quintptr tag() const override;
    QPlatformMenuItem *createMenuItem() const override;

    QMenu *menu() const;

private:
    quintptr m_tag;
    QPointer<QMenu> m_menu;
    QList<SystemTrayMenuItem*> m_items;
};

class SystemTrayMenuItem : public QPlatformMenuItem
{
    Q_OBJECT
public:
    SystemTrayMenuItem();
    ~SystemTrayMenuItem() override;
    void setCheckable(bool checkable) override;
    void setChecked(bool isChecked) override;
    void setEnabled(bool enabled) override;
    void setFont(const QFont &font) override;
    void setIcon(const QIcon &icon) override;
    void setIsSeparator(bool isSeparator) override;
    void setMenu(QPlatformMenu *menu) override;
    void setRole(MenuRole role) override;
    void setShortcut(const QKeySequence &shortcut) override;
    void setTag(quintptr tag) override;
    void setText(const QString &text) override;
    void setVisible(bool isVisible) override;
    quintptr tag() const override;
    void setIconSize(int size) override;

    QAction *action() const;

private:
    quintptr m_tag;
    QAction *m_action;
};

class LXQtSystemTrayIcon : public QPlatformSystemTrayIcon
{
public:
    LXQtSystemTrayIcon();
    ~LXQtSystemTrayIcon() override;

    void init() override;
    void cleanup() override;
    void updateIcon(const QIcon &icon) override;
    void updateToolTip(const QString &tooltip) override;
    void updateMenu(QPlatformMenu *menu) override;
    QRect geometry() const override;
    void showMessage(const QString &title, const QString &msg,
                     const QIcon &icon, MessageIcon iconType, int secs) override;

    bool isSystemTrayAvailable() const override;
    bool supportsMessages() const override;

    QPlatformMenu *createMenu() const override;

private:
    StatusNotifierItem *mSni;
};

#endif
