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

#include "ExampleHelloWorld.h"
#include "WorldObjectManager.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor.
 */
ExampleHelloWorld::ExampleHelloWorld()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
ExampleHelloWorld::~ExampleHelloWorld()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Adds a world object with a Hello World! label at a specific geodetic
 * location. When you run, make sure you enable "Labels" from the View menu in
 * order to see the label.
 */
void ExampleHelloWorld::helloWorld()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //instantiate world object and set label
  WorldObject* worldObject = new WorldObject();
  worldObject->setLabel("Hello World!");

  //set world object position
  GeodeticPosition position;
  position.latitude = 0.0;//in decimal degrees
  position.longitude = 0.0;
  position.altitude = 1.0;//in Km
  worldObject->setGeodeticPosition(position);

  //add world object to manager so that it gets rendered
  WorldObjectManager::getInstance()->addWorldObject(worldObject);
}
