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

#ifndef GLOBALS_H
#define GLOBALS_H

//define modules to be used
//uncomment next line if you want to use elevation databases
//#define USING_GDAL
//uncomment next line if you want to use satellite imagery
//#define USING_PROJ4
//uncomment next line if you want to load 3D models
//#define USING_ASSIMP

/**
 * Generic 3-dimensional vector with x, y and z components. If vector is
 * used for position, then x, y and z are in Km.
 */
struct SimpleVector
{
  double x;
  double y;
  double z;
};

/**
 * A geodetic position is comprised of latitude and longitude in decimal
 * degrees, and altitude in Km.
 */
struct GeodeticPosition
{
  double latitude;
  double longitude;
  double altitude;
};

/**
 * 2-dimensional structure with x and y pixel components as integers.
 */
struct ScreenCoordinates
{
  int x;
  int y;
};

/**
 * Floating point RGBA format structure to store color.
 */
struct SimpleColor
{
  float red;
  float green;
  float blue;
  float alpha;
};

#endif//GLOBALS_H
