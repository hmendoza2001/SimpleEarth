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

#include "ElevationManager.h"
#include "math.h"

#ifdef USING_GDAL
#include "gdal.h"
#include "cpl_conv.h"
#endif

//Singleton implementation
ElevationManager* ElevationManager::mInstance = NULL;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Initializes attributes.
 */
ElevationManager::ElevationManager()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mDatabase = NULL;
  mDatabaseIsLoaded = false;
  mDatabaseOrigin.latitude = 0.0f;
  mDatabaseOrigin.longitude = 0.0f;
  mAreaSizeXDegrees = 0.0f;
  mAreaSizeYDegrees = 0.0f;
  mDatabaseWidth = 0;
  mDatabaseHeight = 0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
ElevationManager::~ElevationManager()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  delete [] mDatabase;
  mDatabaseIsLoaded = false;
  mInstance = NULL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Singleton pattern implementation. Returns the single instance of this class.
 *
 * @return The single instance of this object.
 */
ElevationManager* ElevationManager::getInstance()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mInstance == 0)
  {
    mInstance = new ElevationManager();
  }

  return mInstance;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Loads the given elevation database file into memory for quick access to the
 * the data. Most of this method is taken from the GDAL API Tutorial, refer to
 * (gdal.org/gdal_tutorial.html).
 *
 * @param filePath The file path for the database.
 */
void ElevationManager::loadElevationDatabase(const QString& filePath)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
#ifndef USING_GDAL
  Q_UNUSED(filePath);
#else

  //clear previous database in case one was already loaded
  if (mDatabase != NULL)
  {
    delete [] mDatabase;
  }

  GDALDatasetH hDataset;
  GDALAllRegister();//register all drivers
  hDataset = GDALOpen(filePath.toStdString().c_str(), GA_ReadOnly);

  if (hDataset == NULL)
  {
    return;
  }

  //if we get this far, database will load fine
  mDatabaseIsLoaded = true;

  GDALDriverH hDriver;
  double adfGeoTransform[6];
  hDriver = GDALGetDatasetDriver(hDataset);

  qDebug("Driver: %s/%s", GDALGetDriverShortName(hDriver), GDALGetDriverLongName(hDriver));
  qDebug("Size is %dx%dx%d", GDALGetRasterXSize(hDataset), GDALGetRasterYSize(hDataset), GDALGetRasterCount(hDataset));

  if (GDALGetProjectionRef(hDataset) != NULL)
  {
    qDebug("Projection is %s", GDALGetProjectionRef(hDataset));
  }

  if (GDALGetGeoTransform(hDataset, adfGeoTransform) == CE_None)
  {
    qDebug("Origin = (%.6f,%.6f)", adfGeoTransform[0], adfGeoTransform[3]);
    qDebug("Pixel Size = (%.6f,%.6f)", adfGeoTransform[1], adfGeoTransform[5]);
    mDatabaseOrigin.longitude = adfGeoTransform[0];
    mDatabaseOrigin.latitude = adfGeoTransform[3];
    mAreaSizeXDegrees = adfGeoTransform[1];
    mAreaSizeYDegrees = adfGeoTransform[5];
  }

  GDALRasterBandH hBand;
  int nBlockXSize, nBlockYSize;
  int bGotMin, bGotMax;
  double adfMinMax[2];

  hBand = GDALGetRasterBand(hDataset, 1);
  GDALGetBlockSize(hBand, &nBlockXSize, &nBlockYSize);
  qDebug("Block=%dx%d Type=%s, ColorInterp=%s",
         nBlockXSize, nBlockYSize,
         GDALGetDataTypeName(GDALGetRasterDataType(hBand)),
         GDALGetColorInterpretationName(GDALGetRasterColorInterpretation(hBand)));

  adfMinMax[0] = GDALGetRasterMinimum(hBand, &bGotMin);
  adfMinMax[1] = GDALGetRasterMaximum(hBand, &bGotMax);
  if (!(bGotMin && bGotMax))
  {
    GDALComputeRasterMinMax(hBand, TRUE, adfMinMax);
    qDebug("Min=%.3fd, Max=%.3f", adfMinMax[0], adfMinMax[1]);
  }

  if (GDALGetOverviewCount(hBand) > 0)
  {
    qDebug("Band has %d overviews.", GDALGetOverviewCount(hBand));
  }

  if (GDALGetRasterColorTable(hBand) != NULL)
  {
    qDebug("Band has a color table with %d entries.", GDALGetColorEntryCount(GDALGetRasterColorTable(hBand)));
  }

  mDatabaseWidth = GDALGetRasterXSize(hDataset);
  mDatabaseHeight = GDALGetRasterYSize(hDataset);

  //allocate space for database
  mDatabase = new float[mDatabaseWidth*mDatabaseHeight];

  int width = 0;
  int height = 0;

  for (height = 0; height < mDatabaseHeight; height++)
  {
    for (width = 0; width < mDatabaseWidth; width++)
    {
      GDALRasterIO(hBand, GF_Read, width, height, 1, 1, &mDatabase[(height * mDatabaseWidth) + width], 1, 1, GDT_Float32, 0, 0);
    }
  }

  //close file
  GDALClose(hDataset);
#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the elevation of the given geodetic position in Km.
 *
 * @param latitude Latitude in decimal degrees
 * @param longitude Longitude in decimal degrees
 * @return Elevation of the given geodetic point in Km
 */
float ElevationManager::getElevation(double latitude, double longitude)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //return 0 if no database loaded
  if (!mDatabaseIsLoaded)
  {
    return 0.0f;
  }

  //convert latitude and longitude into database indeces
  int latitudeIndex = rint((latitude - mDatabaseOrigin.latitude) / mAreaSizeYDegrees);
  int longitudeIndex = rint((longitude - mDatabaseOrigin.longitude) / mAreaSizeXDegrees);

  //return 0 if elevation not inside database
  if (latitudeIndex < 0 || longitudeIndex < 0 ||
      latitudeIndex >= mDatabaseHeight || longitudeIndex >= mDatabaseWidth)
  {
    return 0.0f;
  }

  //return elevation in Km
  return mDatabase[(latitudeIndex * mDatabaseWidth) + longitudeIndex]/1000.0f;
}
