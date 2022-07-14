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

#include "TrackInfoWindow.h"
#include "ui_TrackInfoWindow.h"
#include "WorldObjectManager.h"
#include "Utilities.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Connects signals and slots. Sets a 1 second timer to update the
 * window.
 *
 * @param parent Handle to parent widget
 */
TrackInfoWindow::TrackInfoWindow(QWidget *parent) : QDialog(parent), ui(new Ui::TrackInfoWindow)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  ui->setupUi(this);
  mUpdateTimer = new QTimer(this);

  connect(ui->trackList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onItemClicked(QListWidgetItem*)));
  connect(mUpdateTimer, SIGNAL(timeout()), this, SLOT(onUpdateTimer()));

  mUpdateTimer->start(1000);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
TrackInfoWindow::~TrackInfoWindow()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  delete ui;
  delete mUpdateTimer;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This function gets called when the user selects the menu option to display
 * this window. It clears both lists and the current selection.
 */
void TrackInfoWindow::initialize()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mSelectedName = "";
  ui->trackList->clear();
  ui->descriptionList->clear();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the current selected track name.
 *
 * @param name New value for selected track name
 */
void TrackInfoWindow::setSelectedName(const QString& name)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mSelectedName = name;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called every second to update this window if displayed. It
 * clears both sides(lists). It gathers the relevant entities from WorldObjectManager
 * and populates the track list. If there is a selected track, it populates the
 * right hand side with relevant information.
 */
void TrackInfoWindow::onUpdateTimer()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (isVisible())
  {
    WorldObject* worldObject = NULL;
    WorldObjectManager* worldObjectManager = WorldObjectManager::getInstance();

    //clear both sides (lists)
    ui->trackList->clear();
    ui->descriptionList->clear();
    int i = 0;
    int j = 0;

    //display all tracks in WorldObject Manager
    for (i = 0; i < worldObjectManager->getNumberOfObjects(); i++)
    {
      worldObject = worldObjectManager->getWorldObject(i);
      if (worldObject != NULL && !worldObject->getHasExpired() && worldObject->getGroup()==WorldObject::TRACK)
      {
        //add item to list
        ui->trackList->addItem(worldObject->getName());

        //if there is a current selection, then display
        //relevant information on the right hand side
        if (mSelectedName == worldObject->getName())
        {
          GeodeticPosition geoPosition = worldObject->getGeodeticPosition();
          QString dmsLatitude = Utilities::decimalDegreesToDMS(geoPosition.latitude, true);
          QString dmsLongitude = Utilities::decimalDegreesToDMS(geoPosition.longitude, false);
          ui->descriptionList->addItem(worldObject->getName());
          ui->descriptionList->addItem("Lat: " + dmsLatitude);
          ui->descriptionList->addItem("Lon: " + dmsLongitude);
          ui->descriptionList->addItem("Alt: " + QString::number(geoPosition.altitude*1000.0f) + "m");
          ui->descriptionList->addItem("Speed: " + QString::number(worldObject->getSpeed()) + "Km/h");

          //add custom information
          QStringList customInfo = worldObject->getCustomInfo();
          for (j = 0; j < customInfo.length(); j++)
          {
            ui->descriptionList->addItem(customInfo[j]);
          }
        }
      }
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when an item in the track list is clicked. Sets the
 * current track selection.
 *
 * @param item Handle to list widget item clicked
 */
void TrackInfoWindow::onItemClicked(QListWidgetItem* item)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mSelectedName = item->text();
}
