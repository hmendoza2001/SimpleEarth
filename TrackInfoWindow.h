/*
 *  The Simple Earth Project
 *  Copyright (C) 2014 HueSoft LLC
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

#ifndef TRACK_INFO_WINDOW_H
#define TRACK_INFO_WINDOW_H

#include <QDialog>
#include <QTimer>
#include <QString>
#include <QListWidgetItem>

namespace Ui
{
  class TrackInfoWindow;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the functionality behind the Track List window. Refer
 * to TrackInfoWindow.ui for a description of the GUI layout and widget variable
 * names. This window shows a list of all tracks on the left hand side. On the
 * right hand side, it displays information about the selected track like
 * position and speed.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class TrackInfoWindow : public QDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Q_OBJECT

  public:
    TrackInfoWindow(QWidget *parent = 0);
    ~TrackInfoWindow();

    void initialize();
    void setSelectedName(QString name);

  public slots:
    void onUpdateTimer();
    void onItemClicked(QListWidgetItem* item);

  private:
    Ui::TrackInfoWindow *ui;
    QTimer* mUpdateTimer;
    QString mSelectedName;
};

#endif//TRACK_INFO_WINDOW_H
