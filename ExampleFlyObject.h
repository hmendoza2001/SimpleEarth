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

#ifndef EXAMPLE_FLY_OBJECT_H
#define EXAMPLE_FLY_OBJECT_H

#include <QThread>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This example shows you how easy it is to emplace an object in the world that
 * has both an icon and a model associated to it and that flies around the
 * equator. This class inherits from QThread to update the object's position on
 * a different thread (like you would do in a real world application). This
 * class takes some shortcuts for illustration purposes. Please refer to
 * ExampleExpirableObject for a class that could better serve as a template for
 * more reralistic applications.
 *
 * Note: In order to load the model you must include the Assimp library
 * dependency.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class ExampleFlyObject : public QThread
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    ExampleFlyObject();
    ~ExampleFlyObject();

    void run();//OVERRIDE
};

#endif//EXAMPLE_FLY_OBJECT_H
