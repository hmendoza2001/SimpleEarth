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

#include "Hud.h"
#include "globals.h"
#include "WorldObjectMgr.h"
#include "Utilities.h"
#include "Camera.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Initializes attributes.
 */
Hud::Hud()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mPainter = NULL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
Hud::~Hud()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the handle to the painter object. This handle is used to invoke Qt's
 * OpenGL text rendering capabilities.
 *
 * @param painter Handle to Qt painter object
 */
void Hud::setPainter(QPainter* painter)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mPainter = painter;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Renders the Heads Up Display (HUD). Currently, the HUD renders the camera's
 * position on the upper left corner of the screen and also renders all the
 * track labels.
 */
void Hud::renderHud()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //set semi-transparent black rectangle
  mPainter->setBrush(QBrush(QColor(0,0,0,100)));
  mPainter->drawRect(12,10,195,68);

  mPainter->setPen(Qt::white);
  mPainter->setFont(QFont("Arial", 13));

  GeodeticPosition cameraPosition = Camera::getInstance()->getGeodeticPosition();
  QString latStr = "Lat: " + Utilities::decimalDegreesToDMS(cameraPosition.latitude, true);
  QString lonStr = "Lon: " + Utilities::decimalDegreesToDMS(cameraPosition.longitude, false);

  //display camera altitude in Km or m
  float altitudeKm = cameraPosition.altitude;
  QString altStr;
  if (altitudeKm < 1.0f)
  {
    int altitudeMeters = altitudeKm * 1000.0f;
    altStr = "Alt: " + QString::number(altitudeMeters) + "m";
  }
  else
  {
    altStr = "Alt: " + QString::number(altitudeKm, 'f', 1) + "Km";
  }

  mPainter->drawText(20, 30, latStr);
  mPainter->drawText(20, 50, lonStr);
  mPainter->drawText(20, 70, altStr);

  //render world object labels
  renderWorldObjectLabels();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Renders the labels for all world objects that have the showLabel flag set to
 * true.
 */
void Hud::renderWorldObjectLabels()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  WorldObjectMgr* worldObjectMgr = WorldObjectMgr::getInstance();
  Camera* camera = Camera::getInstance();
  WorldObject* worldObject = NULL;
  SimpleVector screenLocation;
  QString label;

  ScreenCoordinates screenSize = Camera::getInstance()->getScreenSize();

  for (int i=0; i<worldObjectMgr->getNumberOfObjects(); i++)
  {
    worldObject = worldObjectMgr->getWorldObject(i);
    if (worldObject != NULL && !worldObject->getHasExpired())
    {
      if (worldObjectMgr->getShowLabels())
      {
        screenLocation = worldObject->getScreenLocation();
        screenLocation.y = (float)screenSize.y - screenLocation.y;//reverse Y

        //only draw the label if world object is not being obscured by the earth
        //and if projection is not negative (behind the camera, which means screenZ
        //is below 1.0)
        if (!Utilities::checkObscure(camera->getGeodeticPosition(), worldObject->getGeodeticPosition()) &&
           screenLocation.z < 1.0f)
        {
          label = worldObject->getLabel();

          //set pen color
          SimpleColor color = worldObject->getColor();
          int red = color.red * 255.0;
          int green = color.green * 255.0;
          int blue = color.blue * 255.0;
          mPainter->setPen(QColor(red, green, blue));

          mPainter->drawText(screenLocation.x+20.0, screenLocation.y, label);
        }
      }
    }
  }
}
