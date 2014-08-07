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

#ifndef TRACK_SELECTION_BROADCASTER_H
#define TRACK_SELECTION_BROADCASTER_H

#include <QList>
#include <QString>
#include "TrackSelectionListener.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Singleton class that implements a track selection broadcaster. This class,
 * along with TrackSelectionListener are an implementation of the observer
 * pattern. Since this class is a Singleton, any class can subscribe (add itself
 * as a listener) to track selections as long as it inherits from
 * TrackSelectionListener. The broadcastSelection method is called from the
 * GLWidget object which is the one that handles mouse clicks and determines if
 * a track has been selected.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class TrackSelectionBroadcaster
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    static TrackSelectionBroadcaster* getInstance();
    ~TrackSelectionBroadcaster();

    void addListener(TrackSelectionListener* listener);
    void broadcastSelection(QString trackName);

  private:
    TrackSelectionBroadcaster();//private due to Singleton implementation

    static TrackSelectionBroadcaster* mInstance;
    QList<TrackSelectionListener*> mListenerList;
};

#endif//TRACK_SELECTION_BROADCASTER_H
