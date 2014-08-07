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

#ifndef EARTH_H
#define EARTH_H

#include "globals.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Singleton class that encapsulates the functionality to render our precious
 * planet along with custom maps or tiles. These tiles can be loaded using a
 * configuration file read in readMapsFile, or by adding them at runtime using
 * the addMap function. The SatelliteImageDownloader class uses the latter to
 * add tiles downloaded from the web.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class Earth
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    struct Map
    {
      GeodeticPosition southWest;
      GeodeticPosition northEast;
      float visibleAltitude;
      int drawPriority;
      unsigned int texture;
    };

    static Earth* getInstance();
    ~Earth();

    void addMap(GeodeticPosition southWest, GeodeticPosition northEast, float visibleAltitude, int drawPriority, QImage& image);
    void readMapsFile(QString filename);
    void render();
    void setEarthTexture(unsigned int handle);
    void setStarTexture(unsigned int handle);
    void setElevationMode(bool value);

  private:
    Earth();//private due to Singleton implementation
    void renderStars();
    void renderEarth();
    void renderMaps();
    void createSphereGeometry(double radius);

    static Earth* mInstance;
    int mDisplayLists[2];
    unsigned int mEarthTextureHandle;
    unsigned int mStarTextureHandle;
    QList<Map> mMapList;//list of maps
    bool mElevationMode;
    int mNumberOfTileSubdivisions;
};

#endif//EARTH_H