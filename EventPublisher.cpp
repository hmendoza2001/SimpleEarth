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

#include "EventPublisher.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor.
 */
EventPublisher::EventPublisher()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
EventPublisher::~EventPublisher()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Adds an EventListener to the list.
 *
 * @param listener Handle to Event Listener object
 */
void EventPublisher::addListener(EventListener* listener)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mListeners.append(listener);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Removes the given listener instance from our list of listeners. Code is
 * mutexed as to prevent removing a listener in the middle of a publish event.
 *
 * @param listener Handle to Event Listener object
 */
void EventPublisher::removeListener(EventListener* listener)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mMutex.lock();

  //find and delete instance
  for (int i = 0; i < mListeners.size(); i++)
  {
    if (mListeners[i] == listener)
    {
      EventListener* temp = mListeners.takeAt(i);//remove from list
      delete temp;//release memory
      break;
    }
  }

  mMutex.unlock();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Publishes generic event. The first string in the list denotes unique event
 * type. Code is mutexed as to prevent removing a listener in the middle of a
 * publish event.
 *
 * @param event String list representing event type and arguments
 */
void EventPublisher::publishEvent(const QStringList& event)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mMutex.lock();

  for (int i = 0; i < mListeners.size(); i++)
  {
    mListeners[i]->onEvent(event);
  }

  mMutex.unlock();
}
