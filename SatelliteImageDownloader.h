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

#ifndef SATELLITE_IMAGE_DOWNLOADER_H
#define SATELLITE_IMAGE_DOWNLOADER_H

#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QList>
#include <QString>
#include <QImage>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class is responsible for downloading satellite imagery from Microsoft's
 * Bing Maps services. Please note this imagery is owned by Microsoft (MSFT) and
 * may only be used for non-commercial purposes. Any other use must be
 * coordinated with the Microsoft Bing Maps team. All imagery obtained using
 * this plug-in is copyrighted and may not be redistributed in any form.
 *
 * This class contains methods and ideas from Casey Chesnut's
 * (brains-n-brawn.com) Virtual Earth plugin for World Wind (refer to
 * worldwindcentral.com/wiki/Add-on:Virtual_Earth and
 * brains-n-brawn.com/default.aspx?vDir=veworldwind). The Virtual Earth plugin
 * for World Wind was made possible through the gracious generosity of Microsoft
 * and the beautiful coding efforts of Casey Chesnut. The methods "borrowed" are
 * identified in their respective headers.
 *
 * This class also uses the proj4 library to calculate mercator and
 * equirectangular projections and it inherits from QThread in order to execute
 * the download and image loading process on a separate thread.
 *
 * @version 1.1
 * @author Hector Mendoza
 */
class SatelliteImageDownloader : public QThread
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Q_OBJECT

  public:
    enum ImageryType
    {
      MAP,
      SATELLITE,
      SATELLITE_WITH_ROADS
    };

    struct LatitudeInversionElement
    {
      double currentLatitude;
      double inverseLatitude;
    };

    struct Tile
    {
      double minLatDeg;
      double minLonDeg;
      double maxLatDeg;
      double maxLonDeg;
      bool alreadyLoaded;
      bool underRequest;
      int zoomLevel;
      QString imageFile;
      QImage* image;
    };

    SatelliteImageDownloader();
    ~SatelliteImageDownloader();

    void run();//OVERRIDE
    void stop();
    void setWebDownloadEnabled(bool value);
    void setImageryType(int imageryType);
    void setElevationMode(bool value);

  public slots:
    void onNetworkRequest(const QString& url);
    void onNetworkReply(QNetworkReply* networkReply);

  signals:
    void sendNetworkRequest(QString url);//signal to sync request back to Qt's main thread
		
  private:
    void downloadTiles();
    void findTileLocation(int row, int column, double tileWidthDeg, double tileHeightMeters, Tile& tile);
    int findZoomLevelFromHAT(float heightAboveTerrain);
    void findVisibleAltitudeAndDrawPriority(int zoomLevel, float& visibleAltitude, int& drawPriority);
    void generateInverseLatitudeLookupTable();
    double findInverseLatitude(double latitudeRad);
    QString getUrl(int column, int row, int zoomLevel, QString& fileName);
    QString tileToQuadKey(int column, int row, int zoomLevel);

    bool mWebDownloadEnabled;
    bool mIsRunning;
    bool mLatitudeLookupTableAlreadyGenerated;
    bool mElevationMode;

    QNetworkAccessManager mNetworkAccessManager;
    QList<Tile> mTiles;
    QList<LatitudeInversionElement> mLatInversionTable;

    QString mImageryType;
    QString mImageryFileExtension;
};

#endif//SATELLITE_IMAGE_DOWNLOADER_H
