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

#ifndef ELEVATION_MGR_H
#define ELEVATION_MGR_H

#include <QString>
#include "globals.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Singleton class that encapsulates the functionality to get an elevation
 * point. This class uses the GDAL library (see gdal.org) to load digital
 * terrain elevation data. The main methods are loadElevationDatabase and
 * getElevation. The latter takes a geodetic position and returns the
 * corresponding elevation in Km.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class ElevationMgr
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    static ElevationMgr* getInstance();
    ~ElevationMgr();

    void loadElevationDatabase(const QString& filePath);
    float getElevation(double latitude, double longitude);

  private:
    ElevationMgr();//private due to Singleton implementation

    static ElevationMgr* mInstance;
    float* mDatabase;
    bool mDatabaseIsLoaded;
    GeodeticPosition mDatabaseOrigin;
    float mAreaSizeXDegrees;
    float mAreaSizeYDegrees;
    int mDatabaseWidth;
    int mDatabaseHeight;
};

#endif//ELEVATION_MGR_H
