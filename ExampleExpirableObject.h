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

#ifndef EXAMPLE_EXPIRABLE_OBJECT_H
#define EXAMPLE_EXPIRABLE_OBJECT_H

#include <QThread>
#include "WorldObject.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This example is a slight modification of ExampleFlyObject. ExampleFlyObject
 * is more of an illustration than a realistic scenario on how you should update
 * object positions. This example shows you how to use expirable objects and
 * how to update their position as if you were getting a position update from
 * a real source (e.g. GPS). This class is better suited to serve as a template
 * for your own SimpleEarth application.
 *
 * @version 1.1
 * @author Hector Mendoza
 */
class ExampleExpirableObject : public QThread
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    ExampleExpirableObject();
    ~ExampleExpirableObject();

    void run();//OVERRIDE

  private:
    void setPositionAndOrientation(WorldObject* worldObject);
};

#endif//EXAMPLE_EXPIRABLE_OBJECT_H
