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

#ifndef EVENT_PUBLISHER_H
#define EVENT_PUBLISHER_H

#include "EventListener.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class, along with EventListener implement the observer pattern. These
 * two classes provide a generic way to publish/subscribe any event because they
 * implement the event as a list of strings. First string in the list denotes a
 * unique event type followed by as many arguments that specific event might
 * have.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class EventPublisher
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    EventPublisher();
    virtual ~EventPublisher();

    void addListener(EventListener* listener);
    void publishEvent(const QStringList& event);

  private:
    QList<EventListener*> mListenerList;
};

#endif//EVENT_PUBLISHER_H
