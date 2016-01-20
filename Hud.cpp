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

#include "Hud.h"
#include "globals.h"
#include "WorldObjectManager.h"
#include "Utilities.h"
#include "Camera.h"

Hud* Hud::mInstance = NULL;//Singleton implementation

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Initializes attributes.
 */
Hud::Hud()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mShowHud = true;
  mShowLabels = false;
  mPainter = NULL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
Hud::~Hud()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mInstance = NULL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Singleton pattern implementation. Returns the single instance of this class.
 *
 * @return The single instance of this class
 */
Hud* Hud::getInstance()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mInstance == NULL)
  {
    mInstance = new Hud();
  }

  return mInstance;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns true if Heads Up Display (HUD) is shown.
 *
 * @return True if Heads Up Display (HUD) is shown, false otherwise
 */
bool Hud::getShowHud()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mShowHud;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns true if world object labels are shown.
 *
 * @return True if world object labels are shown, false otherwise
 */
bool Hud::getShowLabels()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mShowLabels;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the value of flag to show/hide Heads Up Display (HUD).
 *
 * @param show Set to true if you want the HUD to be shown
 */
void Hud::setShowHud(bool show)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mShowHud = show;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the value of flag to show/hide world object labels.
 *
 * @param show Set to true if you want world object labels to be shown
 */
void Hud::setShowLabels(bool show)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mShowLabels = show;
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
  mPainter->setBrush(QBrush(QColor(0,0,0,100)));
  mPainter->setFont(QFont("Arial", 13));

  if (mShowHud)
  {
    //draw semi-transparent black rectangle
    mPainter->drawRect(12,10,195,68);

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

    mPainter->setPen(Qt::white);
    mPainter->drawText(20, 30, latStr);
    mPainter->drawText(20, 50, lonStr);
    mPainter->drawText(20, 70, altStr);
  }

  if (mShowLabels)
  {
    //render world object labels
    renderWorldObjectLabels();
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Renders the labels for all world objects.
 */
void Hud::renderWorldObjectLabels()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  WorldObjectManager* worldObjectManager = WorldObjectManager::getInstance();
  Camera* camera = Camera::getInstance();
  WorldObject* worldObject = NULL;
  SimpleVector screenLocation;
  QString label;

  ScreenCoordinates screenSize = Camera::getInstance()->getScreenSize();

  for (int i = 0; i < worldObjectManager->getNumberOfObjects(); i++)
  {
    worldObject = worldObjectManager->getWorldObject(i);
    if (worldObject != NULL && !worldObject->getHasExpired())
    {
      screenLocation = worldObject->getScreenLocation();

      //only draw the label if world object is not being obscured by the earth
      //and if projection is not negative (not behind the camera, which means
      //screenZ is below 1.0)
      if (screenLocation.z < 1.0f && !Utilities::checkObscure(camera->getGeodeticPosition(),
          worldObject->getGeodeticPosition()))
      {
        label = worldObject->getLabel();

        //set pen color to world object color
        SimpleColor color = worldObject->getColor();
        int red = color.red * 255.0f;
        int green = color.green * 255.0f;
        int blue = color.blue * 255.0f;
        mPainter->setPen(QColor(red, green, blue));

        //reverse Y to screen coordinates
        screenLocation.y = (float)screenSize.y - screenLocation.y;

        mPainter->drawText((int)(screenLocation.x+20.0), (int)screenLocation.y, label);
      }
    }
  }
}
