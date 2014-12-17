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

#include "WorldObject.h"
#include "WorldObjectMgr.h"
#include "PathTool.h"
#include "MeasuringTool.h"
#include "Utilities.h"

//Singleton implementation
WorldObjectMgr* WorldObjectMgr::mInstance = NULL;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Initializes attributes. Connects signals and slots.
 */
WorldObjectMgr::WorldObjectMgr()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //set timer to check for expired tracks
  mExpireTimer = new QTimer(0);
  connect(mExpireTimer, SIGNAL(timeout()), this, SLOT(onExpireTimer()));
  mExpireTimer->start(1000);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
WorldObjectMgr::~WorldObjectMgr()
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
WorldObjectMgr* WorldObjectMgr::getInstance()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mInstance == NULL)
  {
    mInstance = new WorldObjectMgr();
  }

  return mInstance;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Adds a world object to the list. WorldObjectMgr keeps track of objects by
 * unique name, therefore this method checks if the given world object's name is
 * already on the list before adding it.
 *
 * @param object Handle to world object
 * @return True if world object got added to the list, false if object was
 * already in the list
 */
bool WorldObjectMgr::addWorldObject(WorldObject* object)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  bool returnValue = false;

  //only add entity if it is not already in the list
  if (findWorldObject(object->getName()) == -1)
  {
    mWorldObjectList.append(object);
    returnValue = true;
  }

  return returnValue;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the world object at the given index. Returns NULL if given index is
 * outside the list boundaries.
 *
 * @param index Index of world object we want to get
 * @return Handle to world object
 */
WorldObject* WorldObjectMgr::getWorldObject(int index)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mWorldObjectList[index];
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the world object on the list that matches the given name if there is
 * one, NULL otherwise.
 *
 * @param name Name of world object
 * @return Handle to world object
 */
WorldObject* WorldObjectMgr::getWorldObject(const QString& name)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  WorldObject* returnWorldObject = NULL;

  int index = findWorldObject(name);
  if (index != -1)
  {
    returnWorldObject = mWorldObjectList[index];
  }

  return returnWorldObject;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Removes the world object at the given index. This method is thread-safe.
 *
 * @param index Index of the world object we want to delete
 */
void WorldObjectMgr::removeWorldObject(int index)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mMutex.lock();

  if (index < mWorldObjectList.size())
  {
    WorldObject* object = mWorldObjectList.takeAt(index);//remove from list
    delete object;
  }

  mMutex.unlock();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Removes the world object with the given name from the list. This method is
 * thread-safe.

 * @param name Name of the world object we want to delete
 */
void WorldObjectMgr::removeWorldObject(const QString& name)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mMutex.lock();

  int index = findWorldObject(name);
  if (index != -1)
  {
    WorldObject* object = mWorldObjectList.takeAt(index);//remove from list
    delete object;
  }

  mMutex.unlock();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Calls the render method on all entities on the list if they have the
 * isVisible flag set to true. This method is thread-safe.
 */
void WorldObjectMgr::renderObjects()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mMutex.lock();

  int i = 0;
  SimpleVector screenLocation;

  //compute screen location and render all meshes first
  for (i=0; i<mWorldObjectList.size(); i++)
  {
    if (mWorldObjectList[i]->getIsVisible())
    {
      //compute and set screen location
      screenLocation = Utilities::worldToScreen(mWorldObjectList[i]->getPosition());
      mWorldObjectList[i]->setScreenLocation(screenLocation);

      mWorldObjectList[i]->renderMesh();
    }
  }

  //render icons last
  for (i=0; i<mWorldObjectList.size(); i++)
  {
    if (mWorldObjectList[i]->getIsVisible())
    {
      mWorldObjectList[i]->renderIcon();
    }
  }

  mMutex.unlock();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the number of world objects in the list.
 *
 * @return Number of world objects in the list
 */
int WorldObjectMgr::getNumberOfObjects()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mWorldObjectList.size();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the index of the world object with the given name if it is found.
 * Returns -1 otherwise.
 *
 * @param name Name of world object
 * @return Index of found entity, -1 otherwise
 */
int WorldObjectMgr::findWorldObject(const QString& name)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  int index = -1;

  for (int i=0; i<mWorldObjectList.size(); i++)
  {
    if (name == mWorldObjectList[i]->getName())
    {
      index = i;
      break;
    }
  }

  return index;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called every second to loop through the expirable tracks and
 * decrements their expiration timer. If a timer reaches 0, it sets the
 * hasExpired flag for the object (serves as a warning that this object should
 * no longer be used). It deletes any entities that have reached a timer of -2.
 */
void WorldObjectMgr::onExpireTimer()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  int expirationTime = 0;

  for (int i=0; i<mWorldObjectList.size(); i++)
  {
    if (mWorldObjectList[i] != NULL && mWorldObjectList[i]->getIsExpirable())
    {
      expirationTime = mWorldObjectList[i]->getExpirationTime();
      expirationTime--;
      mWorldObjectList[i]->setExpirationTime(expirationTime);

      //when time expires set has expired flag to true to let object users
      //have a warning they should no longer use this object
      if (expirationTime == 0)
      {
          mWorldObjectList[i]->setHasExpired(true);
      }
      else if (expirationTime <= -2)
      {
        removeWorldObject(i);
        //rewind counter since we just deleted one entity from the list
        i--;
      }
    }
  }
}
