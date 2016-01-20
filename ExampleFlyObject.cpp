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

#include "ExampleFlyObject.h"
#include "WorldObjectManager.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor.
 */
ExampleFlyObject::ExampleFlyObject()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
ExampleFlyObject::~ExampleFlyObject()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR QThread. This method runs on a separate thread. This method
 * shows you how easy it is to instantiate a world object that has both an icon
 * and a model associated to it and fly it around the equator.
 */
void ExampleFlyObject::run()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //instantiate world object
  WorldObject* worldObject = new WorldObject();

  //The World Object Manager keeps track of World Objects by unique name,
  //so it is important that we give a unique object name
  worldObject->setName("ExampleFlyObject");

  //load icon and model to be associated with object
  //Note: The model is a jeep ( no airplane :-( ) taken from the
  //samples that come with the Assimp library. The model has an
  //exaggerated scale (since model scale is in meters but our world scale
  //is in Km) which makes it easier to find for this test. Use the
  //setScale method for your own models if you want a realistic scale
  worldObject->loadIcon("images/fixedWingFriend.png");
  worldObject->loadModel("models/jeep1.3ds");

  //create and set initial object position and orientation
  GeodeticPosition position;
  position.latitude = 0.0;//in decimal degrees
  position.longitude = 0.0;
  position.altitude = 10.0;//in Km
  worldObject->setGeodeticPosition(position);

  SimpleVector rotation;
  rotation.x = -90.0;//in degrees
  rotation.y = 180.0;
  rotation.z = 0.0;
  worldObject->setRotation(rotation);

  //add object to manager so that it gets rendered
  WorldObjectManager::getInstance()->addWorldObject(worldObject);

  //update position in a 10 times per second loop
  while (true)
  {
    position.longitude += 0.01;
    //wrap longitude if necessary
    if (position.longitude > 180.0)
    {
      position.longitude -= 360.0;
    }
    worldObject->setGeodeticPosition(position);

    msleep(100);
  }
}
