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

#ifndef CONSTANTS_H
#define CONSTANTS_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates constant values that might otherwise create conflict
 * if defined by third party code used in conjunction with SimpleEarth. This
 * class acts as a namespace for these constants.
 *
 * @version 1.1
 * @author Hector Mendoza
 */
class Constants
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    static const double PI = 3.1415926535897932384626433832795;
    static const double DEGREES_TO_RADIANS = 0.01745329251994329576923690768489;
    static const double RADIANS_TO_DEGREES = 57.295779513082320876798154814105;
    static const double EARTH_MEAN_RADIUS = 6371.0;//in Km
    static const double CAMERA_INITIAL_ALTITUDE = 20050.0;//in Km
    static const double EARTH_EQUATORIAL_CIRCUMFERENCE = 40075017.0;//in meters
};

#endif//CONSTANTS_H
