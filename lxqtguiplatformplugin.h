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

#include "qguiplatformplugin_p.h"
#include <QtGlobal>
#include <lxqt/LxQtSettings>

class LxQtGuiPlatformPlugin : public QGuiPlatformPlugin {
  Q_OBJECT
public:
  explicit LxQtGuiPlatformPlugin();
  virtual ~LxQtGuiPlatformPlugin();

  virtual QStringList keys() const { return QStringList() << QLatin1String("lxqt"); }
  virtual QString styleName();
  virtual QPalette palette();
  virtual QString systemIconThemeName();
  // virtual QStringList iconThemeSearchPaths();
  // virtual QIcon fileSystemIcon(const QFileInfo& info);

  virtual int platformHint(PlatformHint hint);

  /*
  virtual void fileDialogDelete(QFileDialog *) {}
  virtual bool fileDialogSetVisible(QFileDialog *, bool) { return false; }
  virtual QDialog::DialogCode fileDialogResultCode(QFileDialog *) { return QDialog::Rejected; }
  virtual void fileDialogSetDirectory(QFileDialog *, const QString &) {}
  virtual QString fileDialogDirectory(const QFileDialog *) const { return QString(); }
  virtual void fileDialogSelectFile(QFileDialog *, const QString &) {}
  virtual QStringList fileDialogSelectedFiles(const QFileDialog *) const { return QStringList(); }
  virtual void fileDialogSetFilter(QFileDialog *) {}
  virtual void fileDialogSetNameFilters(QFileDialog *, const QStringList &) {}
  virtual void fileDialogSelectNameFilter(QFileDialog *, const QString &) {}
  virtual QString fileDialogSelectedNameFilter(const QFileDialog *) const { return QString(); }

  virtual void colorDialogDelete(QColorDialog *) {}
  virtual bool colorDialogSetVisible(QColorDialog *, bool) { return false; }
  virtual void colorDialogSetCurrentColor(QColorDialog *, const QColor &) {}
  */

private Q_SLOTS:
  void onIconThemeChanged();
  void onSettingsChanged();
};
