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

#ifndef TRACK_SELECTION_LISTENER_H
#define TRACK_SELECTION_LISTENER_H

#include <QString>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Interface definition for a track selection listener. This class, along with
 * TrackSelectionBroadcaster are an implementation of the observer pattern. In
 * order to be able to listen for track selections, an object must inherit from
 * this class and subscribe to TrackSelectionBroadcaster which is a Singleton.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class TrackSelectionListener
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    virtual ~TrackSelectionListener();
    virtual void onTrackSelection(QString trackName) = 0;//forced override
};

#endif//TRACK_SELECTION_LISTENER_H
