/*
 *  The Simple Earth Project
 *  Copyright (C) 2022 HueSoft LLC
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

#ifndef PLACES_WINDOW_H
#define PLACES_WINDOW_H

#include <QDialog>
#include <QString>
#include <QList>
#include <QListWidgetItem>
#include "globals.h"

namespace Ui
{
  class PlacesWindow;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the functionality behind the Places Window. Refer
 * to PlacesWindow.ui for a description of the GUI layout and widget variable
 * names. This window shows a list of places the user has enetered. The current
 * list gets saved on exit and gets loaded at startup.
 *
 * @version 1.1
 * @author Hector Mendoza
 */
class PlacesWindow : public QDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Q_OBJECT

  public:
    struct Place
    {
      GeodeticPosition position;
      QString name;
    };

    PlacesWindow(QWidget *parent = 0);
    ~PlacesWindow();

    void loadPlaces();
    void savePlaces();

  public slots:
    void onAddButton();
    void onDeleteButton();
    void onItemDoubleClicked(QListWidgetItem* item);

  private:
    Ui::PlacesWindow *ui;
    QList<Place> mPlacesList;
};

#endif//PLACES_WINDOW_H
