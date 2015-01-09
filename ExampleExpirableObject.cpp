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

#include "ExampleExpirableObject.h"
#include "WorldObjectMgr.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor.
 */
ExampleExpirableObject::ExampleExpirableObject()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
ExampleExpirableObject::~ExampleExpirableObject()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR QThread. This method runs on a separate thread. In a real-life
 * scenario, you would probably receive object updates by polling a GPS device
 * or from the network. Therefore, you might want objects to disappear from the
 * screen once you stop receiving updates for them, let's say after 5 seconds of
 * not hearing from them for example. It turns out WorldObjectMgr makes it easy
 * to accomplish this with expirable objects.
 *
 * Additionally, on the ExampleFlyObject example, we were actually taking some
 * shortcuts that we probably shouldn't have. For example, we were directly
 * updating the object's position without fetching the object from
 * WorldObjectMgr which is a bad idea since memory is managed by the manager and
 * the object could be null or expired. This example takes things one step
 * further and is more realistic in the way we update the object's position and
 * orientation simulating an "external" source.
 */
void ExampleExpirableObject::run()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //instantiate world object
  //note: object's memory space is managed by WorldObjectMgr
  WorldObject* worldObject = new WorldObject();

  //the World Object Manager keeps track of World Objects by unique name,
  //so it is important that we give a unique object name
  QString name = "ExampleExpirableObject";
  worldObject->setName(name);

  //load icon and model to be associated with object
  //note: The model is a jeep ( there is no airplane in the samples :-( )
  //taken from the samples that come with the Assimp library. The model
  //has an exaggerated scale (since model scale is in meters but our world
  //scale is in Km) which makes it easier to find for this test. Use the
  //setScale method for your own models if you want a realistic scale
  worldObject->loadIcon("images/fixedWingFriend.png");
  worldObject->loadModel("models/jeep1.3ds");

  //set expirable flag and initial expiration time
  worldObject->setIsExpirable(true);
  worldObject->setExpirationTime(5);//in seconds

  //add object to manager so that it gets rendered
  WorldObjectMgr::getInstance()->addWorldObject(worldObject);

  //this loop will update the object's position 5 times. After that
  //the object will expire after 5 seconds and disappear from the
  //screen. Memory will be released by the world object manager too
  for (int i = 0; i < 5; i++)
  {
    worldObject = WorldObjectMgr::getInstance()->getWorldObject(name);
    if (worldObject != NULL && !worldObject->getHasExpired())
    {
      setPositionAndOrientation(worldObject);
    }
    msleep(2000);
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Updates the position and orietation for the given object with hardcoded
 * values for illustration purposes. This function simulates an "external" way
 * of getting a position update. You can imagine replacing the hardcoded values
 * of this function with a poll to a real GPS device or a request to some other
 * source.
 *
 * @param worldObject Handle to world object that we need to update
 */
void ExampleExpirableObject::setPositionAndOrientation(WorldObject* worldObject)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  GeodeticPosition geoPosition;
  geoPosition = worldObject->getGeodeticPosition();

  //set initial position and orientation if we have not done so
  if (geoPosition.altitude <= 0.0)
  {
    geoPosition.latitude = 0.0;//in decimal degrees
    geoPosition.longitude = 0.0;
    geoPosition.altitude = 10.0;//in Km

    SimpleVector orientation;
    orientation.x = -90.0;//in degrees
    orientation.y = 180.0;
    orientation.z = 0.0;

    worldObject->setRotation(orientation);
  }

  geoPosition.longitude += 0.1;
  //wrap longitude if necessary
  if (geoPosition.longitude > 180.0)
  {
    geoPosition.longitude -= 360.0;
  }

  worldObject->setGeodeticPosition(geoPosition);

  //since we got an update, remember to reset expiration time
  worldObject->setExpirationTime(5);
}
