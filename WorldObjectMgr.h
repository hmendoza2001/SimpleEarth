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

#ifndef WORLD_OBJECT_MGR_H
#define WORLD_OBJECT_MGR_H

#include <QWidget>
#include <QList>
#include <QTimer>
#include <QMutex>
#include "WorldObject.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the World Object Manager. The World Object Manager is
 * an implementation of the Facade and Singleton design patterns. It contains
 * the list of all World Objects in the scene. It loops through the list to call
 * each object's respective renderers (meshes and icons).
 *
 * This class inherits from QObject in order to listen for expire timer events.
 * It keeps track of objects whose timer has expired and deletes them.
 * Deallocation of World Objects should only be done through the
 * removeWorldObject method.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class WorldObjectMgr : public QWidget
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Q_OBJECT

  public:
    static WorldObjectMgr* getInstance();
    ~WorldObjectMgr();

    bool addWorldObject(WorldObject* object);
    WorldObject* getWorldObject(int index);
    WorldObject* getWorldObject(QString name);
    void removeWorldObject(int index);
    void removeWorldObject(QString name);
    void renderObjects();
    int getNumberOfObjects();
    int findWorldObject(QString name);

  public slots:
    void onExpireTimer();

  private://methods
    WorldObjectMgr();//private due to Singleton implementation

    static WorldObjectMgr* mInstance;
    QList<WorldObject*> mWorldObjectList;
    QTimer* mExpireTimer;
    QMutex mMutex;
};

#endif//WORLD_OBJECT_MGR_H
