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

#include "statusnotifieritem.h"
#include "statusnotifieritemadaptor.h"
#include <dbusmenu-qt5/dbusmenuexporter.h>

int StatusNotifierItem::mServiceCounter = 0;

StatusNotifierItem::StatusNotifierItem(QString id, QObject *parent)
    : QObject(parent),
    mAdaptor(new StatusNotifierItemAdaptor(this)),
    mService(QString("org.freedesktop.StatusNotifierItem-%1-%2")
             .arg(QCoreApplication::applicationPid())
             .arg(++mServiceCounter)),
    mId(id),
    mTitle("Test"),
    mStatus("Active"),
    mMenu(nullptr)
{
    // register service
    QDBusConnection::sessionBus().registerService(mService);
    QDBusConnection::sessionBus().registerObject("/StatusNotifierItem", this);

    // register to daemon
    QDBusInterface interface("org.kde.StatusNotifierWatcher",
                             "/StatusNotifierWatcher",
                             "org.kde.StatusNotifierWatcher",
                             QDBusConnection::sessionBus());
    interface.asyncCall("RegisterStatusNotifierItem", mService);
}

StatusNotifierItem::~StatusNotifierItem()
{
    QDBusConnection::sessionBus().unregisterObject("/StatusNotifierItem");
    QDBusConnection::sessionBus().unregisterService(mService);
    QDBusConnection::sessionBus().disconnectFromBus(mService);

    delete mMenu;
}

void StatusNotifierItem::setTitle(const QString &title)
{
    if (mTitle == title)
        return;

    mTitle = title;
    emit mAdaptor->NewTitle();
}

void StatusNotifierItem::setStatus(const QString &status)
{
    if (mStatus == status)
        return;

    mStatus = status;
    emit mAdaptor->NewStatus(mStatus);
}

void StatusNotifierItem::setMenuPath(const QString& path)
{
    mMenuPath.setPath(path);
}

void StatusNotifierItem::setIconByName(const QString &name)
{
    if (mIconName == name)
        return;

    mIconName = name;
    emit mAdaptor->NewIcon();
}

void StatusNotifierItem::setIconByPixmap(const QIcon &icon)
{
    // TODO: use icon cache key
//     if (mIcon == icon)
//         return;

    mIconName.clear();
    mIcon = iconToPixmapList(icon);
    emit mAdaptor->NewIcon();
}

void StatusNotifierItem::setOverlayIconByName(const QString &name)
{
    if (mOverlayIconName == name)
        return;

    mOverlayIconName = name;
    emit mAdaptor->NewOverlayIcon();
}

void StatusNotifierItem::setOverlayIconByPixmap(const QIcon &icon)
{
    // TODO: use icon cache key
//     if (mOverlayIcon == icon)
//         return;

    mOverlayIconName.clear();
    mOverlayIcon = iconToPixmapList(icon);
    emit mAdaptor->NewOverlayIcon();
}

void StatusNotifierItem::setAttentionIconByName(const QString &name)
{
    if (mAttentionIconName == name)
        return;

    mAttentionIconName = name;
    emit mAdaptor->NewAttentionIcon();
}

void StatusNotifierItem::setAttentionIconByPixmap(const QIcon &icon)
{
    // TODO: use icon cache key
//     if (mAttentionIcon == icon)
//         return;

    mAttentionIconName.clear();
    mAttentionIcon = iconToPixmapList(icon);
    emit mAdaptor->NewAttentionIcon();
}

void StatusNotifierItem::setToolTipTitle(const QString &title)
{
    if (mTooltipTitle == title)
        return;

    mTooltipTitle = title;
    emit mAdaptor->NewToolTip();
}

void StatusNotifierItem::setToolTipSubTitle(const QString &subTitle)
{
    if (mTooltipSubtitle == subTitle)
        return;

    mTooltipSubtitle = subTitle;
    emit mAdaptor->NewToolTip();
}

void StatusNotifierItem::setToolTipIconByName(const QString &name)
{
    if (mTooltipIconName == name)
        return;

    mTooltipIconName = name;
    emit mAdaptor->NewToolTip();
}

void StatusNotifierItem::setToolTipIconByPixmap(const QIcon &icon)
{
    // TODO: use icon cache key
//     if (mTooltipIcon == icon)
//         return;

    mTooltipIconName.clear();
    mTooltipIcon = iconToPixmapList(icon);
    emit mAdaptor->NewToolTip();
}

void StatusNotifierItem::setContextMenu(QMenu* menu)
{
    if (mMenu == menu)
        return;

    delete mMenu;
    mMenu = menu;
    mMenu->setParent(nullptr);

    setMenuPath("/MenuBar");
    new DBusMenuExporter(this->menu().path(), mMenu);
}

void StatusNotifierItem::Activate(int x, int y)
{
    if (mStatus == "NeedsAttention")
        mStatus = "Active";

    emit activateRequested(QPoint(x, y));
}

void StatusNotifierItem::ContextMenu(int x, int y)
{
    if (mMenu)
    {
        if (mMenu->isVisible())
            mMenu->popup(QPoint(x, y));
        else
            mMenu->hide();
    }
}

void StatusNotifierItem::Scroll(int delta, const QString &orientation)
{
    // TODO: finish this
}

void StatusNotifierItem::SecondaryActivate(int x, int y)
{
    if (mStatus == "NeedsAttention")
        mStatus = "Active";

    emit secondaryActivateRequested(QPoint(x, y));
}

void StatusNotifierItem::showMessage(const QString& title, const QString& msg, const QString& iconName, int secs)
{
    // TODO: finish this
}

IconPixmapList StatusNotifierItem::iconToPixmapList(const QIcon& icon)
{
    IconPixmapList pixmapList;

    // long live KDE!
    for (const QSize &size : icon.availableSizes())
    {
        QImage image = icon.pixmap(size).toImage();

        IconPixmap pix;
        pix.height = image.height();
        pix.width = image.width();

        if (image.format() != QImage::Format_ARGB32)
            image = image.convertToFormat(QImage::Format_ARGB32);

        pix.bytes = QByteArray((char *) image.bits(), image.byteCount());

        // swap to network byte order if we are little endian
        if (QSysInfo::ByteOrder == QSysInfo::LittleEndian)
        {
            quint32 *uintBuf = (quint32 *) pix.bytes.data();
            for (uint i = 0; i < pix.bytes.size() / sizeof(quint32); ++i)
            {
                *uintBuf = qToBigEndian(*uintBuf);
                ++uintBuf;
            }
        }

        pixmapList.append(pix);
    }

    return pixmapList;
}
