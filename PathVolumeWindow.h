/*
 *  The Simple Earth Project
 *  Copyright (C) 2016 HueSoft LLC
 *  Author: Hector Mendoza, hector.mendoza@huesoftllc.com
 *
 *  This file is part of the Simple Earth Project.
 *
 *  The Simple Earth Project is free software: you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, either version
 *  3 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program. If not, see
 *  <http://www.gnu.org/licenses/>.
 */

#ifndef PATH_VOLUME_WINDOW_H
#define PATH_VOLUME_WINDOW_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <QPoint>

namespace Ui
{
  class PathVolumeWindow;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the functionality behind the Paths and Volumes
 * window. Refer to PathVolumeWindow.ui for a description of the GUI layout and
 * widget variable names. This window displays a tree that contains all paths
 * and volumes. The user is able to hide/show, delete and bring up object
 * properties from this window.
 *
 * @version 1.1
 * @author Hector Mendoza
 */
class PathVolumeWindow : public QDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Q_OBJECT

  public:
    PathVolumeWindow(QWidget *parent = 0);
    ~PathVolumeWindow();

    void initialize();

  public slots:
    void onItemChanged(QTreeWidgetItem* item, int column);
    void onContextMenu(QPoint point);
    void onItemPressed(QTreeWidgetItem* item, int column);

  private:
    Ui::PathVolumeWindow *ui;
    bool mShowContextMenu;
    QTreeWidgetItem* mCurrentTreeItem;
};

#endif//PATH_VOLUME_WINDOW_H
