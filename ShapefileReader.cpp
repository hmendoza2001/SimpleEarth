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

#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QMessageBox>
#include <QtEndian>
#include "ShapefileReader.h"
#include "Utilities.h"
#include "WorldObjectManager.h"
#include "ShapeRenderer.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Initializes attributes.
 */
ShapefileReader::ShapefileReader()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mHasHeightData = false;
  mNumberOfReadShapes = 0;
  mMinimumHeight = 0.065f;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
ShapefileReader::~ShapefileReader()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the minimum height filter. 3D shapes whose height is below the minimum
 * height are not displayed.
 *
 * @param value New minimum height value
 */
void ShapefileReader::setMinimumHeight(float value)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mMinimumHeight = value;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Get method for minimum height attribute. 3D shapes whose height is below the
 * minimum height are not displayed.
 *
 * @return Current minimum height filter
 */
float ShapefileReader::getMinimumHeight()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mMinimumHeight;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Main shapefile read function. This function reads the given ESRI Shapefile
 * and adds the appropriate geometry to WorldObject Manager. Please refer to ESRI
 * Shapefile Technical Description document for a description of file content.
 *
 * @param fileName Name of ESRI Shapefile
 */
void ShapefileReader::readFile(QString fileName)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QFile shapeFile(fileName);

  //the name of height data file (if existent) is the same
  //buth with a .csv extension
  QString heightDataFileName = fileName.remove("shp");
  heightDataFileName.append("csv");
  QFile heightDataFile(heightDataFileName);

  int i=0;
  qint8 dummy8;
  qint32 dummy32;
  qint64 dummy64;
  qint32 recordNumber = 0;
  qint32 recordLength = 0;
  qint32 shapeType = 0;
  qint32 numberOfParts = 0;
  qint32 numberOfPointsTotal = 0;
  qint32 numberOfIncludedPoints = 0;
  qint64 pointXInt = 0;
  qint64 pointYInt = 0;
  double* pointX = NULL;
  double* pointY = NULL;

  //attempt to open shape file
  if (!shapeFile.open(QIODevice::ReadOnly))
  {
    QMessageBox::critical(NULL, "Error", "Error opening shape file.");
    shapeFile.close();
    heightDataFile.close();
    return;
  }

  //check to see if there is a height data file associated with this shape
  if (heightDataFile.open(QIODevice::ReadOnly))
  {
    mHasHeightData = true;
  }
  else
  {
    mHasHeightData = false;
  }

  //construct stream now that the files are open
  QDataStream shapeFileStream(&shapeFile);
  QTextStream heightDataFileStream(&heightDataFile);

  //skip file header (first 100 bytes)
  for (i = 0; i < 100; i++)
  {
    shapeFileStream >> dummy8;
  }

  //read until end of file
  while (!shapeFileStream.atEnd())
  {
    shapeFileStream >> recordNumber;//big endian
    shapeFileStream >> recordLength;//big endian

    //get shape type (little endian from here on)
    shapeFileStream >> dummy32;
    //NOTE: Even though we use the function qToBigEndian,
    //the data is really in little endian, the purpose of
    //this function is just to swap the bytes so that we
    //can read them appropriately
    shapeType = qToBigEndian(dummy32);

    if (shapeType != 5)
    {
      printf("\n\nInvalid shape type:%d\n\n", shapeType);
      shapeFile.close();
      heightDataFile.close();
      return;
    }

    //skip bounding box (8*4 bytes)
    for (i = 0; i < 4; i++)
    {
      shapeFileStream >> dummy64;
    }

    //get number of parts and number of points
    shapeFileStream >> dummy32;
    numberOfParts = qToBigEndian(dummy32);
    shapeFileStream >> dummy32;
    numberOfPointsTotal = qToBigEndian(dummy32);

    //process parts
    //NOTE: For now we only support single part polygons.
    //Polygons with more than one part will still be read
    //but all other additional parts will be ignored.
    //We get the second part's index in order to know how
    //many points to include (up to index[1] points, refer
    //to ESRI Shapefile Technical Description)
    for (i = 0; i < numberOfParts; i++)
    {
      shapeFileStream >> dummy32;
      if (i == 1)
      {
        numberOfIncludedPoints = qToBigEndian(dummy32);
      }
    }
    if (numberOfParts == 1)
    {
      numberOfIncludedPoints = numberOfPointsTotal;
    }

    WorldObject* worldObject = NULL;
    ShapeRenderer* shapeRenderer = NULL;
    GeodeticPosition geoPos;
    GeodeticPosition geoPosWithHeight;
    SimpleVector pos;
    SimpleVector posWithHeight;
    float readHeight = 0.0f;

    //if there is height data associated with the shape,
    //filter the shape by minimum altitude
    if (mHasHeightData)
    {
      QString line = heightDataFileStream.readLine();
      //read data is in meters, convert to KM
      readHeight = line.toFloat()/1000.0f;

      if (readHeight > mMinimumHeight)
      {
        worldObject = new WorldObject();
        shapeRenderer = new ShapeRenderer();
      }
    }
    else
    {
      worldObject = new WorldObject();
      shapeRenderer = new ShapeRenderer();
    }

    //loop through points
    for (i = 0; i < numberOfPointsTotal; i++)
    {
      shapeFileStream >> pointXInt;
      shapeFileStream >> pointYInt;

      if (i < numberOfIncludedPoints)
      {
        pointXInt = qToBigEndian(pointXInt);
        pointX = (double*)&pointXInt;
        pointYInt = qToBigEndian(pointYInt);
        pointY = (double*)&pointYInt;

        //convert to geodetic
        QString utmString = "54 N " + QString::number(*pointX) + " " + QString::number(*pointY);
        geoPos = Utilities::utmToGeodetic(utmString);
        geoPos.altitude = 0.0f;
        //convert to XYZ
        pos = Utilities::geodeticToXYZ(geoPos);

        //if there is height data associated with the shape,
        //filter the shape by minimum altitude
        if (mHasHeightData)
        {
          if (readHeight > mMinimumHeight)
          {
            shapeRenderer->addGroundPoint(pos);

            shapeRenderer->setHasHeight(true);
            geoPosWithHeight = geoPos;
            geoPosWithHeight.altitude = readHeight;

            //convert to XYZ
            posWithHeight = Utilities::geodeticToXYZ(geoPosWithHeight);
            shapeRenderer->addHeightPoint(posWithHeight);
          }
        }
        else
        {
          shapeRenderer->addGroundPoint(pos);
        }
      }
    }

    //if there is height data associated with the shape,
    //filter the shape by minimum altitude
    if (mHasHeightData)
    {
      if (readHeight > mMinimumHeight)
      {
        worldObject->setName("Shape" + QString::number(mNumberOfReadShapes));
        worldObject->setMeshRenderer(shapeRenderer);
        WorldObjectManager::getInstance()->addWorldObject(worldObject);
        mNumberOfReadShapes++;
      }
    }
    else
    {
      worldObject->setName("Shape" + QString::number(mNumberOfReadShapes));
      worldObject->setMeshRenderer(shapeRenderer);
      WorldObjectManager::getInstance()->addWorldObject(worldObject);
      mNumberOfReadShapes++;
    }
  }//end of while (!in.atEnd())

  printf("Number of shapes %d\n", mNumberOfReadShapes);

  shapeFile.close();
  heightDataFile.close();
}
