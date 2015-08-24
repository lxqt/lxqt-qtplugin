/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org/
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


#ifndef STATUS_NOTIFIER_ITEM_H
#define STATUS_NOTIFIER_ITEM_H

#include <QObject>
#include <QIcon>
#include <QMenu>

#include "dbustypes.h"

class StatusNotifierItemAdaptor;

class StatusNotifierItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString Title READ title)
    Q_PROPERTY(QString Id READ id)
    Q_PROPERTY(QString Status READ status)
    Q_PROPERTY(QDBusObjectPath Menu READ menu)

    Q_PROPERTY(QString IconName READ iconName)
    Q_PROPERTY(IconPixmapList IconPixmap READ iconPixmap)

    Q_PROPERTY(QString OverlayIconName READ overlayIconName)
    Q_PROPERTY(IconPixmapList OverlayIconPixmap READ overlayIconPixmap)

    Q_PROPERTY(QString AttentionIconName READ attentionIconName)
    Q_PROPERTY(IconPixmapList AttentionIconPixmap READ attentionIconPixmap)

    Q_PROPERTY(ToolTip ToolTip READ toolTip)

public:
    StatusNotifierItem(QString id, QObject *parent = nullptr);
    ~StatusNotifierItem();

    QString id() const
    { return mId; }

    QString title() const
    { return mTitle; }
    void setTitle(const QString &title);

    QString status() const
    { return mStatus; }
    void setStatus(const QString &status);

    QDBusObjectPath menu() const
    { return mMenuPath; }
    void setMenuPath(const QString &path);

    QString iconName() const
    { return mIconName; }
    void setIconByName(const QString &name);

    IconPixmapList iconPixmap() const
    { return mIcon; }
    void setIconByPixmap(const QIcon &icon);

    QString overlayIconName() const
    { return mOverlayIconName; }
    void setOverlayIconByName(const QString &name);

    IconPixmapList overlayIconPixmap() const
    { return mOverlayIcon; }
    void setOverlayIconByPixmap(const QIcon &icon);

    QString attentionIconName() const
    { return mAttentionIconName; }
    void setAttentionIconByName(const QString &name);

    IconPixmapList attentionIconPixmap() const
    { return mAttentionIcon; }
    void setAttentionIconByPixmap(const QIcon &icon);

    QString toolTipTitle() const
    { return mTooltipTitle; }
    void setToolTipTitle(const QString &title);

    QString toolTipSubTitle() const
    { return mTooltipSubtitle; }
    void setToolTipSubTitle(const QString &subTitle);

    QString toolTipIconName() const
    { return mTooltipIconName; }
    void setToolTipIconByName(const QString &name);

    IconPixmapList toolTipIconPixmap() const
    { return mTooltipIcon; }
    void setToolTipIconByPixmap(const QIcon &icon);

    ToolTip toolTip() const
    {
        ToolTip tt;
        tt.title = mTooltipTitle;
        tt.description = mTooltipSubtitle;
        tt.iconName = mTooltipIconName;
        tt.iconPixmap = mTooltipIcon;
        return tt;
    }

    void setContextMenu(QMenu *menu);

signals:
    void activateRequested(const QPoint &pos);
    void secondaryActivateRequested(const QPoint &pos);

public slots:
    void Activate(int x, int y);
    void ContextMenu(int x, int y);
    void Scroll(int delta, const QString &orientation);
    void SecondaryActivate(int x, int y);

    void showMessage(const QString &title, const QString &msg, const QString &iconName, int secs);

private:
    StatusNotifierItemAdaptor *mAdaptor;

    QString mService;
    QString mId;
    QString mTitle;
    QString mStatus;

    // icons
    QString mIconName, mOverlayIconName, mAttentionIconName;
    IconPixmapList mIcon, mOverlayIcon, mAttentionIcon;

    // tooltip
    QString mTooltipTitle, mTooltipSubtitle, mTooltipIconName;
    IconPixmapList mTooltipIcon;

    // menu
    QMenu *mMenu;
    QDBusObjectPath mMenuPath;

    static int mServiceCounter;

    IconPixmapList iconToPixmapList(const QIcon &icon);
};

#endif
