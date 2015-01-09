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

#include <QUrl>
#include <QDir>
#include <QFile>
#include <QNetworkRequest>
#include "SatelliteImageDownloader.h"
#include "math.h"
#include "Camera.h"
#include "Earth.h"
#include "ElevationMgr.h"
#include "Constants.h"

#ifdef USING_PROJ4
#include <proj_api.h>
//declare projection variables
static projPJ mercatorProjection;
static projPJ latlonProjection;
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Initializes attributes. Connects signals and slots.
 */
SatelliteImageDownloader::SatelliteImageDownloader()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mWebDownloadEnabled = true;
  mIsRunning = false;
  mLatitudeLookupTableAlreadyGenerated = false;
  mImageryType = "h";
  mImageryFileExtension = ".jpeg";
  mElevationMode = false;

  connect(this, SIGNAL(sendNetworkRequest(QString)), this, SLOT(onNetworkRequest(QString)));
  connect(&mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetworkReply(QNetworkReply*)));

#ifdef USING_PROJ4
  //initialize projection variables
  mercatorProjection = pj_init_plus("+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +no_defs");
  latlonProjection = pj_init_plus("+proj=latlong +ellps=WGS84");
#endif

  generateInverseLatitudeLookupTable();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
SatelliteImageDownloader::~SatelliteImageDownloader()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR QThread. This method runs on a separate thread. Calls the main
 * class method downloadTiles() approx 10 times a second.
 */
void SatelliteImageDownloader::run()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mIsRunning = true;
  while (mIsRunning)
  {
    downloadTiles();
    msleep(100);
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Gracefully stops the download thread.
 */
void SatelliteImageDownloader::stop()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mIsRunning = false;
  msleep(200);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the flag that enables/disables the download of tiles from the web.
 *
 * @param value Set to true/false to enable/disable web download
 */
void SatelliteImageDownloader::setWebDownloadEnabled(bool value)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mWebDownloadEnabled = value;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the imagery type that should be downloaded. Refer to ImageryType enum
 * for valid values.
 *
 * @param imageryType Refer to ImageryType for valid values.
 */
void SatelliteImageDownloader::setImageryType(int imageryType)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (imageryType == MAP)
  {
    mImageryType = "r";
    mImageryFileExtension = ".png";
  }
  else if (imageryType == SATELLITE)
  {
    mImageryType = "a";
    mImageryFileExtension = ".jpeg";
  }
  else if (imageryType == SATELLITE_WITH_ROADS)
  {
    mImageryType = "h";
    mImageryFileExtension = ".jpeg";
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the value of flag that determines if tiles will be rendered in elevation
 * mode which means that a single image layer should be downloaded. We also
 * set Earth's rendering accordingly.
 *
 * @param value New value for elevation mode flag
 */
void SatelliteImageDownloader::setElevationMode(bool value)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mElevationMode = value;
  Earth::getInstance()->setElevationMode(mElevationMode);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Called whenever we emit a sendNetworkRequest signal. This is done to
 * synchronize the network request with the main Qt thread since Qt does not
 * allow for network requests to be done on a different thread.
 *
 * @param url String representing the URL
 */
void SatelliteImageDownloader::onNetworkRequest(const QString& url)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mNetworkAccessManager.get(QNetworkRequest(QUrl(url)));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called whenever a network/web reply arrives.
 *
 * @param networkReply Handle to Qt's network reply object
 */
void SatelliteImageDownloader::onNetworkReply(QNetworkReply* networkReply)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //ignore all network replies after stopped
  if (mIsRunning)
  {
    //get file name from url
    QString url = networkReply->url().toString();
    QString lastSection = url.section("/", 4, 4);
    QString imageFormat = mImageryFileExtension + "?g=15";
    QString fileExtension;
    if (mImageryFileExtension == ".jpeg")
    {
      fileExtension = ".jpg";
    }
    else
    {
      fileExtension = ".png";
    }
    QString fileName = lastSection.remove(imageFormat) + fileExtension;

    //find where to place tile
    int tileIndex = 0;
    for (tileIndex = 0; tileIndex < mTiles.size(); tileIndex++)
    {
      if (mTiles[tileIndex].imageFile == fileName)
      {
        break;
      }
    }

    if (networkReply->error() == QNetworkReply::NoError)
    {
      int bufferSize = networkReply->bytesAvailable();
      char* tempDataBuffer = new char[bufferSize];
      networkReply->read(tempDataBuffer, bufferSize);
      //load image onto local database
      QImage image;
      if (!image.loadFromData((uchar*)tempDataBuffer, bufferSize, fileExtension.toStdString().c_str()))
      {
        printf("SatelliteImageDownloader.cpp: Error loading image.\n");
        delete [] tempDataBuffer;
        return;
      }
      delete [] tempDataBuffer;

      GeodeticPosition southWest;
      GeodeticPosition northEast;
      southWest.latitude = mTiles[tileIndex].minLatDeg;
      southWest.longitude = mTiles[tileIndex].minLonDeg;
      southWest.altitude = 0.0f;
      northEast.latitude = mTiles[tileIndex].maxLatDeg;
      northEast.longitude = mTiles[tileIndex].maxLonDeg;
      northEast.altitude = 0.0f;

      //get visible altitude and draw priority
      float visibleAltitude = 0.0f;
      int drawPriority = 0;
      findVisibleAltitudeAndDrawPriority(mTiles[tileIndex].zoomLevel, visibleAltitude, drawPriority);

      //add Earth map
      Earth::getInstance()->addMap(southWest, northEast, visibleAltitude, drawPriority, image);
      mTiles[tileIndex].alreadyLoaded = true;
    }
    else
    {
      printf("SatelliteImageDownloader.cpp: Network error.\n");
    }

    //image is no longer in request
    mTiles[tileIndex].underRequest = false;
  }//end of if (mIsRunning)
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This is the main method in this class. This method is responsible for
 * requesting satellite images from the web.
 */
void SatelliteImageDownloader::downloadTiles()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //the first thing we do is get the camera location
  GeodeticPosition cameraPosition = Camera::getInstance()->getGeodeticPosition();

  //then we calculate height above terrain for the camera
  double heightAboveTerrain = cameraPosition.altitude -
    ElevationMgr::getInstance()->getElevation(cameraPosition.latitude, cameraPosition.longitude);

  //we calculate the zoom level for this height
  int currentZoomLevel = findZoomLevelFromHAT(heightAboveTerrain);

  if (currentZoomLevel != 0)
  {
    //*****
    //find row and column for the tile directly underneath the camera

    //finding the column is straight forward since longitude has a 1 to
    //1 relationship in both plate carree and mercator projections
    double tileWidthDeg = 360.0/pow(2.0, (double)currentZoomLevel);
    double normalizedLongitudeDeg = cameraPosition.longitude + 180.0;//normalize longitude from [-180,180] to [0,360]
    int centerColumn = normalizedLongitudeDeg/tileWidthDeg;

    //finding the row requires a mercator projection
    //IMPORTANT NOTE: Due to the projection, if we were to use just
    //the current latitude for the camera, we would not get the tile
    //that is directly underneath the camera. For this we need to
    //find the inverse of the projection, but since lat-lon to mercator
    //and mercator to lat-lon aren't exactly the inverse of each other,
    //what we do is query a pre-built table to get the inverse
    double mercatorY = findInverseLatitude(cameraPosition.latitude * Constants::DEGREES_TO_RADIANS);

#ifdef USING_PROJ4
    double dummy = 0.0;
    pj_transform(latlonProjection, mercatorProjection, 1, 1, &mercatorY, &dummy, NULL);
#endif

    //once we have the projected Y, we compare it to the Earth's
    //meridional circumference to get the right row
    double tileHeightMeters = (Constants::EARTH_EQUATORIAL_CIRCUMFERENCE/2.0)/pow(2.0, (double)currentZoomLevel);
    double distanceFromUpperLeftOrigin = (Constants::EARTH_EQUATORIAL_CIRCUMFERENCE/4.0) - mercatorY;
    int centerRow = distanceFromUpperLeftOrigin/tileHeightMeters;
    //*****

    //we have the right row and column now, so we try
    //to download a 5x5 tile region
    int i,j;
    int databaseIndex = 0;
    int column = centerColumn - 2;
    int row = centerRow - 2;
    QString fileName;
    QString url;
    bool tileFound = false;
    for (i = 0; i < 5; i++)
    {
      for (j = 0; j < 5; j++)
      {
        //find the file name
        url = getUrl(column, row, currentZoomLevel, fileName);

        //try to find tile in local database
        tileFound = false;
        for (databaseIndex = 0; databaseIndex < mTiles.size(); databaseIndex++)
        {
          if (mTiles[databaseIndex].imageFile == fileName)
          {
            tileFound = true;
            break;
          }
        }

        if (tileFound)
        {
          //check if already loaded, if not, request tile again
          if (mTiles[databaseIndex].alreadyLoaded == false)
          {
            if (mWebDownloadEnabled)
            {
              if (mTiles[databaseIndex].underRequest == false)
              {
                mTiles[databaseIndex].underRequest = true;
                emit sendNetworkRequest(url);
              }
            }
          }
        }
        else//tile not found in local database
        {
          //create tile
          Tile tile;
          findTileLocation(row, column, tileWidthDeg, tileHeightMeters, tile);
          tile.imageFile = fileName;
          tile.zoomLevel = currentZoomLevel;

          if (mWebDownloadEnabled)
          {
            tile.underRequest = true;
            emit sendNetworkRequest(url);
          }

          //store tile in database
          mTiles.append(tile);
        }
        //go to next column
        column++;
      }//end of inner for loop

      //reset column
      column = centerColumn - 2;
      //go to next row
      row++;
    }//end of outter for loop
  }//end of if (currentZoomLevel != 0)
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Given the row, column, width and height of a tile, determines the min and
 * max lat-lon location for that tile.
 *
 * @param row Given row for the tile
 * @param column Given column for the tile
 * @param tileWidthDeg The width of the tile in degrees
 * @param tileHeightMeters The height of the tile in mercator meters
 * @param tile Returned structure with corresponding min and max lat lon in radians
 */
void SatelliteImageDownloader::findTileLocation(int row, int column, double tileWidthDeg, double tileHeightMeters, Tile& tile)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  tile.minLonDeg =  -180.0 + tileWidthDeg*(double)column;
  tile.maxLonDeg = tile.minLonDeg + tileWidthDeg;

  double rowPlusOne = (double)row + 1.0;
  double minMercatorY = (Constants::EARTH_EQUATORIAL_CIRCUMFERENCE/4.0) - (rowPlusOne*tileHeightMeters);
  double maxMercatorY = minMercatorY + tileHeightMeters;
  double newMinLatRad = minMercatorY*2.0;
  double newMaxLatRad = maxMercatorY*2.0;

#ifdef USING_PROJ4
  double dummy = 0.0;
  pj_transform(mercatorProjection, latlonProjection, 1, 1, &dummy, &newMinLatRad, NULL);
  pj_transform(mercatorProjection, latlonProjection, 1, 1, &dummy, &newMaxLatRad, NULL);
#endif

  tile.minLatDeg = newMinLatRad * Constants::RADIANS_TO_DEGREES;
  tile.maxLatDeg = newMaxLatRad * Constants::RADIANS_TO_DEGREES;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Calculates an appropriate imagery zoom level for the given HAT (Height Above
 * Terrain).
 *
 * @param heightAboveTerrain Height above terrain in KM
 * @return The appropriate imagery zoom level for this HAT
 */
int SatelliteImageDownloader::findZoomLevelFromHAT(float heightAboveTerrain)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //if elevation mode is on, we only return a single image layer of reasonable
  //zoom level for large areas
  if (mElevationMode)
  {
    if (heightAboveTerrain > 0.0f && heightAboveTerrain <= 50.0f)
      return 14;
  }
  else
  {
    if (heightAboveTerrain > 800.0f)
      return 0;
    if (heightAboveTerrain > 220.0f && heightAboveTerrain <= 800.0f)
      return 9;
    if (heightAboveTerrain > 140.0f && heightAboveTerrain <= 220.0f)
      return 10;
    if (heightAboveTerrain > 68.0f && heightAboveTerrain <= 140.0f)
      return 11;
    if (heightAboveTerrain > 36.0f && heightAboveTerrain <= 68.0f)
      return 12;
    if (heightAboveTerrain > 18.0f && heightAboveTerrain <= 36.0f)
      return 13;
    if (heightAboveTerrain > 10.0f && heightAboveTerrain <= 18.0f)
      return 14;
    if (heightAboveTerrain > 5.0f && heightAboveTerrain <= 10.0f)
      return 15;
    if (heightAboveTerrain > 2.5f && heightAboveTerrain <= 5.0f)
      return 16;
    if (heightAboveTerrain > 1.2f && heightAboveTerrain <= 2.5f)
      return 17;
    if (heightAboveTerrain > 0.0f && heightAboveTerrain <= 1.2f)
      return 18;
  }

  return 0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the altitude at which a map tile should be visible (this is kind of
 * the inverse of the findZoomLevelFromHAT function) and the draw priority for
 * the tile. If the priority is lower, the tile will be drawn first.
 *
 * @param zoomLevel Integer specifying zoom level
 * @param visibleAltitude Integer specifying zoom level
 * @param drawPriority Integer specifying zoom level
 */
void SatelliteImageDownloader::findVisibleAltitudeAndDrawPriority(int zoomLevel, float& visibleAltitude, int& drawPriority)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //if elevation mode is on, we only return a single image layer of reasonable
  //zoom level for large areas
  if (mElevationMode)
  {
    if (zoomLevel == 14)
    {
      visibleAltitude = 50.0f;
      drawPriority = 0;
    }
  }
  else
  {
    if (zoomLevel < 9)
    {
      visibleAltitude = 1200.0f;
      drawPriority = 0;
    }
    else if (zoomLevel == 9)
    {
      visibleAltitude = 800.0f;
      drawPriority = 1;
    }
    else if (zoomLevel == 10)
    {
      visibleAltitude = 220.0f;
      drawPriority = 2;
    }
    else if (zoomLevel == 11)
    {
      visibleAltitude = 140.0f;
      drawPriority = 3;
    }
    else if (zoomLevel == 12)
    {
      visibleAltitude = 68.0f;
      drawPriority = 4;
    }
    else if (zoomLevel == 13)
    {
      visibleAltitude = 36.0f;
      drawPriority = 5;
    }
    else if (zoomLevel == 14)
    {
      visibleAltitude = 18.0f;
      drawPriority = 6;
    }
    else if (zoomLevel == 15)
    {
      visibleAltitude = 10.0f;
      drawPriority = 7;
    }
    else if (zoomLevel == 16)
    {
      visibleAltitude = 5.0f;
      drawPriority = 8;
    }
    else if (zoomLevel == 17)
    {
      visibleAltitude = 2.5f;
      drawPriority = 9;
    }
    else if (zoomLevel == 18)
    {
      visibleAltitude = 1.2f;
      drawPriority = 10;
    }
  }

  //the visible altitude value must include terrain elevation
  //in order to show terrain at appropriate altitude
  GeodeticPosition cameraPosition = Camera::getInstance()->getGeodeticPosition();
  double terrainHeight = ElevationMgr::getInstance()->getElevation(cameraPosition.latitude, cameraPosition.longitude);
  visibleAltitude += terrainHeight;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Since mercator Y to latitude and latitude to mercator Y projections do not
 * work as exactly inverse of each other near the poles, we generate an inverse
 * lookup table instead that is used to get the exact mercator location of the
 * tile that is directly underneath the camera.
 */
void SatelliteImageDownloader::generateInverseLatitudeLookupTable()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (!mLatitudeLookupTableAlreadyGenerated)
  {
    mLatitudeLookupTableAlreadyGenerated = true;

    //generate lookup table
    double transformedLatitude = 0.0;
    double dummy = 0.0;
    double currentLatitudeDeg = 0.0;
    int i=0;
    double newLatRad = 0.0;
    LatitudeInversionElement element;

    //get latitude from 0 to 30 every half point
    for (i = 0; i < 60; i++)
    {
      transformedLatitude = currentLatitudeDeg * Constants::DEGREES_TO_RADIANS;

#ifdef USING_PROJ4
      pj_transform(latlonProjection, mercatorProjection, 1, 1, &transformedLatitude, &dummy, NULL);
      newLatRad = transformedLatitude*2.0;
      pj_transform(mercatorProjection, latlonProjection, 1, 1, &dummy, &newLatRad, NULL);
#endif

      element.inverseLatitude = currentLatitudeDeg;
      element.currentLatitude = newLatRad * Constants::RADIANS_TO_DEGREES;
      mLatInversionTable.append(element);

      currentLatitudeDeg += 0.5;
    }

    //get latitude from 30 to 90 every point
    for (i = 0; i <= 60; i++)
    {
      transformedLatitude = currentLatitudeDeg * Constants::DEGREES_TO_RADIANS;

#ifdef USING_PROJ4
      pj_transform(latlonProjection, mercatorProjection, 1, 1, &transformedLatitude, &dummy, NULL);
      newLatRad = transformedLatitude*2.0;
      pj_transform(mercatorProjection, latlonProjection, 1, 1, &dummy, &newLatRad, NULL);
#endif

      element.inverseLatitude = currentLatitudeDeg;
      element.currentLatitude = newLatRad * Constants::RADIANS_TO_DEGREES;
      mLatInversionTable.append(element);

      currentLatitudeDeg += 1.0;
    }
  }//end of if (!mLatitudeLookupTableAlreadyGenerated)
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Performs linear search to find the mercator inverse of a latitude.
 *
 * @param latitudeRad Latitude in radians
 */
double SatelliteImageDownloader::findInverseLatitude(double latitudeRad)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  double latitudeDeg = fabs(latitudeRad * Constants::RADIANS_TO_DEGREES);
  double negativeMultiplier  = 0.0;

  if (latitudeRad < 0.0)
  {
    negativeMultiplier = -1.0;
  }
  else
  {
    negativeMultiplier = 1.0;
  }

  if (latitudeDeg == 90.0)
  {
    return negativeMultiplier * mLatInversionTable[mLatInversionTable.size()-1].inverseLatitude;
  }

  for (int i = 0; i < mLatInversionTable.size()-1; i++)
  {
    if (latitudeDeg == mLatInversionTable[i].currentLatitude)
    {
      return negativeMultiplier * mLatInversionTable[i].inverseLatitude;
    }

    //perform linear interpolation to get value in-between
    if ((latitudeDeg > mLatInversionTable[i].currentLatitude) && (latitudeDeg < mLatInversionTable[i+1].currentLatitude))
    {
      double slope = (mLatInversionTable[i+1].inverseLatitude - mLatInversionTable[i].inverseLatitude) /
        (mLatInversionTable[i+1].currentLatitude - mLatInversionTable[i].currentLatitude);
      double yIntersect = mLatInversionTable[i].inverseLatitude - slope*mLatInversionTable[i].currentLatitude;
      double output = slope*latitudeDeg + yIntersect;
      return negativeMultiplier * output * Constants::DEGREES_TO_RADIANS;
    }
  }

  return 0.0f;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This function is a slight modification of a function from Casey Chesnut's
 * Virtual Earth plugin for World Wind
 * (brains-n-brawn.com/default.aspx?vDir=veworldwind). Returns the appropriate
 * URL from the given row, column, and zoom level.
 *
 * @param column Column in mercator projection
 * @param row Row in mercator projection
 * @param zoomLevel Zoom level for this tile
 * @param fileName Image file name
 * @return String representing the URL that will get us the appropriate image
 */
QString SatelliteImageDownloader::getUrl(int column, int row, int zoomLevel, QString& fileName)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QString quadKey = tileToQuadKey(column, row, zoomLevel);

  QString url = "http://" + mImageryType + quadKey[quadKey.length() - 1] + ".ortho.tiles.virtualearth.net/tiles/" +
    mImageryType + quadKey + mImageryFileExtension + "?g=15";

  QString fileExtension;
  if (mImageryFileExtension == ".jpeg")
  {
    fileExtension = ".jpg";
  }
  else
  {
    fileExtension = ".png";
  }
  fileName = mImageryType + quadKey + fileExtension;

  return url;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This function is a slight modification of a function from Casey Chesnut's
 * Virtual Earth plugin for World Wind
 * (brains-n-brawn.com/default.aspx?vDir=veworldwind). Converts row, column and
 * zoom level to a string used in the URL query.
 *
 * @param column Column in mercator projection for given zoom level
 * @param row Row in mercator projection for given zoom level
 * @param zoomLevel Zoom level
 * @return String used in the URL query for this particular image
 */
QString SatelliteImageDownloader::tileToQuadKey(int column, int row, int zoomLevel)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QString quad = "";
  int mask = 0;
  int cell = 0;
  for (int i = zoomLevel; i > 0; i--)
  {
    mask = 1 << (i - 1);
    cell = 0;
    if ((column & mask) != 0)
    {
      cell++;
    }
    if ((row & mask) != 0)
    {
      cell += 2;
    }
    quad += QString::number(cell);
  }
  return quad;
}
