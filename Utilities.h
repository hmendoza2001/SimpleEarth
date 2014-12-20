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

#ifndef UTILITIES_H
#define UTILITIES_H

#include <QString>
#include <QImage>
#include "globals.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates utility functions, e.g. coordinate conversion
 * functions. All functions are static, so there is no need to instantiate the
 * class in order to use its functions.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class Utilities
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    static GeodeticPosition xyzToGeodetic(const SimpleVector& xyzPosition);
    static SimpleVector geodeticToXYZ(const GeodeticPosition& geodeticPosition);
    static double dmsToDecimalDegrees(const QString& dmsString);
    static QString decimalDegreesToDMS(double decimalDegrees, bool isLatitude);
    static GeodeticPosition ecefToGeodetic(const SimpleVector& ecefPosition);
    static GeodeticPosition utmToGeodetic(const QString& utmString);
    static bool checkObscure(const GeodeticPosition& cameraPosition, const GeodeticPosition& objectPosition);
    static unsigned int imageToTexture(const QImage& image);
    static SimpleVector screenToWorld(const ScreenCoordinates& screenPosition);
    static SimpleVector worldToScreen(const SimpleVector& worldPosition);
};

#endif//UTILITIES_H
