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

#include <QFile>
#include <QTextStream>
#include "PlacesWindow.h"
#include "ui_PlacesWindow.h"
#include "Camera.h"
#include "NewPlaceDialog.h"
#include "MainWindow.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Connects signals and slots.
 *
 * @param parent Handle to parent widget
 */
PlacesWindow::PlacesWindow(QWidget *parent) : QDialog(parent), ui(new Ui::PlacesWindow)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  ui->setupUi(this);

  connect(ui->addButton, SIGNAL(clicked()), this, SLOT(onAddButton()));
  connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButton()));
  connect(ui->placesList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onItemDoubleClicked(QListWidgetItem*)));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
PlacesWindow::~PlacesWindow()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  delete ui;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Reads Places.txt file and loads the listed places.
 */
void PlacesWindow::loadPlaces()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QFile file("Places.txt");

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    printf("Error loading places file.\n");
  }
  else
  {
    QStringList split;
    QTextStream in(&file);
    QString line;

    //read first line to determine which type of file it is
    line = in.readLine();
    if (!line.contains("PLACES"))
    {
      printf("Places file has invalid format.\n");
      return;
    }

    Place place;

    while (!in.atEnd())
    {
      line = in.readLine();

      if (line.contains("PlaceName="))
      {
        split = line.split("=");
        place.name = split[1];
      }
      else if (line.contains("PlaceLatitude="))
      {
        split = line.split("=");
        place.position.latitude = split[1].toDouble();
      }
      else if (line.contains("PlaceLongitude="))
      {
        split = line.split("=");
        place.position.longitude = split[1].toDouble();
      }
      else if (line.contains("PlaceAltitude="))
      {
        split = line.split("=");
        place.position.altitude = split[1].toDouble();
      }
      else if (line.contains("ENDPLACE"))
      {
        mPlacesList.append(place);
        ui->placesList->addItem(place.name);
      }
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Saves places list to the Places.txt file. This method gets called during
 * system exit.
 */
void PlacesWindow::savePlaces()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QFile file("Places.txt");
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    printf("Error saving places file.\n");
    return;
  }

  QTextStream out(&file);
  out.setRealNumberNotation(QTextStream::FixedNotation);
  out.setRealNumberPrecision(6);
  out << "PLACES" << "\n";
  for (int i = 0; i < mPlacesList.size(); i++)
  {
    out << "PlaceName=" << mPlacesList[i].name << "\n";
    out << "PlaceLatitude=" << mPlacesList[i].position.latitude << "\n";
    out << "PlaceLongitude=" << mPlacesList[i].position.longitude << "\n";
    out << "PlaceAltitude=" << mPlacesList[i].position.altitude << "\n";
    out << "ENDPLACE" << "\n";
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the add button is pressed. Adds current camera
 * position to the list of places.
 */
void PlacesWindow::onAddButton()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Place place;
  NewPlaceDialog dialog(this);
  place.name = dialog.getNewPlaceName();

  if (!place.name.isEmpty())
  {
    place.position = Camera::getInstance()->getGeodeticPosition();
    mPlacesList.append(place);
    ui->placesList->addItem(place.name);
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the delete button is pressed. Removes selected
 * place from the list of places.
 */
void PlacesWindow::onDeleteButton()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  int index = ui->placesList->currentRow();
  mPlacesList.removeAt(index);
  QListWidgetItem* deleteItem = ui->placesList->takeItem(index);
  delete deleteItem;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when an item in the list is double clicked. Calls method
 * in Camera to navigate to selected place.
 *
 * @param item Handle to double clicked item
 */
void PlacesWindow::onItemDoubleClicked(QListWidgetItem*)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //first set camera in orbit mode before navigation
  MainWindow::getInstance()->setNavigationModeToOrbit();
  int index = ui->placesList->currentRow();
  Camera::getInstance()->moveByDestinationPoint(mPlacesList[index].position);
}
