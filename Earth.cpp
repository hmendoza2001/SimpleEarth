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

#include <QtOpenGL>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include "Earth.h"
#include "globals.h"
#include "Constants.h"
#include "Camera.h"
#include "Utilities.h"
#include "ElevationManager.h"

Earth* Earth::mInstance = NULL;//Singleton implementation
static Camera* camera = NULL;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Initializes attributes.
 */
Earth::Earth()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mEarthTextureHandle = 0;
  mStarTextureHandle = 0;
  mElevationMode = false;
  mNumberOfTileSubdivisions = 1;
  camera = Camera::getInstance();

  for (int i = 0; i < 2; i++)
  {
    mDisplayLists[i] = 0;
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
Earth::~Earth()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mInstance = NULL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Singleton pattern implementation. Returns the single instance of this class.
 *
 * @return The single instance of this class
 */
Earth* Earth::getInstance()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mInstance == NULL)
  {
    mInstance = new Earth();
  }

  return mInstance;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Adds a map tile to the Earth's geometry.
 *
 * @param southWest Map's sothwest geodetic position
 * @param northEast Map's northeast geodetic position
 * @param visibleAltitude Altitude at which the map becomes visible
 * @param drawPriority The drawing priority, lower number means it gets drawn
 *        first
 * @param textureFile File name for the texture
 */
void Earth::addMap(const GeodeticPosition& southWest, const GeodeticPosition& northEast,
                   float visibleAltitude, int drawPriority, const QImage& image)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Map map;
  map.southWest = southWest;
  map.northEast = northEast;
  map.visibleAltitude = visibleAltitude;
  map.drawPriority = drawPriority;
  map.texture = Utilities::imageToTexture(image);
  mMapList.append(map);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Reads the given maps file in order to add any custom maps to the list of maps
 * that get rendered. See Map structure for a description of the map elements
 * that are read from file.
 *
 * @filename Maps file name
 */
void Earth::readMapsFile(const QString& filename)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    printf("Earth: Error reading maps file.");
  }
  else
  {
    Map map;
    QStringList split;
    QString line;
    QTextStream in(&file);
    while (!in.atEnd())
    {
      line = in.readLine();

      if (line.contains("#"))
      {
        continue;
      }
      else if (line.contains("SWLat="))
      {
        split = line.split("=");
        map.southWest.latitude = Utilities::dmsToDecimalDegrees(split[1]);
      }
      else if (line.contains("SWLon="))
      {
        split = line.split("=");
        map.southWest.longitude = Utilities::dmsToDecimalDegrees(split[1]);
      }
      else if (line.contains("SWAlt="))
      {
        split = line.split("=");
        map.southWest.altitude = split[1].toFloat();
      }
      else if (line.contains("NELat="))
      {
        split = line.split("=");
        map.northEast.latitude = Utilities::dmsToDecimalDegrees(split[1]);
      }
      else if (line.contains("NELon="))
      {
        split = line.split("=");
        map.northEast.longitude = Utilities::dmsToDecimalDegrees(split[1]);
      }
      else if (line.contains("NEAlt="))
      {
        split = line.split("=");
        map.northEast.altitude = split[1].toFloat();
      }
      else if (line.contains("VisibleAltitude="))
      {
        split = line.split("=");
        map.visibleAltitude = split[1].toFloat();
      }
      else if (line.contains("DrawPriority="))
      {
        split = line.split("=");
        map.drawPriority = split[1].toInt();
      }
      else if (line.contains("Image="))
      {
        split = line.split("=");
        //load image
        QImage image;
        if (!image.load(split[1]))
        {
          printf("Earth.cpp: Error loading image in maps file.\n");
          break;
        }
        map.texture = Utilities::imageToTexture(image);
      }
      else if (line.contains("ENDMAP"))
      {
        mMapList.append(map);
      }
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Builds the appropriate geometry and renders the Blue Marble globe, the star
 * dome and the map list.
 */
void Earth::render()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  renderStars();
  renderEarth();
  renderMaps();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the earth texture, in this case the Blue Marble imagery.
 *
 * @param handle OpenGL handle to texture
 */
void Earth::setEarthTexture(unsigned int handle)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mEarthTextureHandle = handle;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the star dome texture.
 *
 * @param handle OpenGL handle to texture
 */
void Earth::setStarTexture(unsigned int handle)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mStarTextureHandle = handle;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the value of flag that determines if maps are being rendered in
 * elevation mode which means that a single image layer is assumed (no disabling
 * of GL_DEPTH to avoid z-fighting) and the tiles are subdivided for higher
 * terrain resolution.
 *
 * @param value New value for elevation mode flag
 */
void Earth::setElevationMode(bool value)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (value)
  {
    mNumberOfTileSubdivisions = 8;
  }
  else
  {
    mNumberOfTileSubdivisions = 1;
  }

  mElevationMode = value;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Renders the stars dome around the camera.
 */
void Earth::renderStars()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  glCullFace(GL_FRONT);
  glDisable(GL_DEPTH_TEST);
  glPushMatrix();

  //draw dome around camera position
  SimpleVector cameraPosition = camera->getPosition();
  glTranslatef(cameraPosition.x, cameraPosition.y, cameraPosition.z);

  if (mDisplayLists[0] == 0)
  {
    mDisplayLists[0] = glGenLists(1);
    glNewList(mDisplayLists[0], GL_COMPILE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mStarTextureHandle);
    createSphereGeometry(2000.0);
    glDisable(GL_TEXTURE_2D);

    glEndList();
  }
  else
  {
    glCallList(mDisplayLists[0]);
  }

  glPopMatrix();
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_BACK);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Computes and renders the geometry for the Earth.
 */
void Earth::renderEarth()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mDisplayLists[1] == 0)
  {
    mDisplayLists[1] = glGenLists(1);
    glNewList(mDisplayLists[1], GL_COMPILE);

    //set color to draw earth just in case texture was not found
    glColor3f(0.0f,0.5f,1.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mEarthTextureHandle);
    createSphereGeometry(Constants::EARTH_MEAN_RADIUS);
    glDisable(GL_TEXTURE_2D);

    glEndList();
  }
  else
  {
    glCallList(mDisplayLists[1]);
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Renders the maps in our map list that are within visible camera altitude and
 * within the viewable boundaries. We use the mNumberOfTileSubdivisions variable
 * to have a higher resolution on terrain for elevation database purposes.
 */
void Earth::renderMaps()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  int mapIndex, drawPriority;
  GeodeticPosition geoPosition[4];
  SimpleVector xyzPosition[4];

  //get rid of Z fighting by always drawing later maps on top
  //when not in elevation mode
  if (!mElevationMode)
  {
    glDepthFunc(GL_ALWAYS);
  }

  int subdivisionX = 0;
  int subdivisionY = 0;
  double subdivisionWidth = 0.0;
  double subdivisionHeight = 0.0;
  float textureWidth = 1.0f/(float)mNumberOfTileSubdivisions;
  float textureHeight = 1.0f/(float)mNumberOfTileSubdivisions;
  float texturePositionX = 0.0f;
  float texturePositionY = 0.0f;
  ElevationManager* elevationManager = ElevationManager::getInstance();
  GeodeticPosition cameraPosition = camera->getGeodeticPosition();

  //draw tiles by priority
  for (drawPriority = 0; drawPriority < 11; drawPriority++)
  {
    for (mapIndex = 0; mapIndex < mMapList.size(); mapIndex++)
    {
      //make sure tile is only rendered if within the right priority, altitude and area
      if (mMapList[mapIndex].drawPriority == drawPriority &&
          cameraPosition.altitude < mMapList[mapIndex].visibleAltitude &&
          mMapList[mapIndex].northEast.latitude < (cameraPosition.latitude + 3.0f) &&
          mMapList[mapIndex].southWest.latitude > (cameraPosition.latitude - 3.0f) &&
          mMapList[mapIndex].northEast.longitude < (cameraPosition.longitude + 3.0f) &&
          mMapList[mapIndex].southWest.longitude > (cameraPosition.longitude - 3.0f))
      {
        subdivisionWidth = (mMapList[mapIndex].northEast.longitude -
          mMapList[mapIndex].southWest.longitude) / (double)mNumberOfTileSubdivisions;

        subdivisionHeight = (mMapList[mapIndex].northEast.latitude -
          mMapList[mapIndex].southWest.latitude) / (double)mNumberOfTileSubdivisions;

        geoPosition[0] = mMapList[mapIndex].southWest;
        geoPosition[0].altitude = elevationManager->getElevation(geoPosition[0].latitude, geoPosition[0].longitude);

        texturePositionX = 0.0f;
        texturePositionY = 0.0f;

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, mMapList[mapIndex].texture);

        glBegin(GL_TRIANGLES);

        //draw tiles with subdivisions for higher terrain fidelity
        for (subdivisionY = 0; subdivisionY < mNumberOfTileSubdivisions; subdivisionY++)
        {
          for (subdivisionX = 0; subdivisionX < mNumberOfTileSubdivisions; subdivisionX++)
          {
            texturePositionX = (float)subdivisionX*textureWidth;
            texturePositionY = (float)subdivisionY*textureHeight;

            geoPosition[1].latitude = geoPosition[0].latitude;
            geoPosition[1].longitude = geoPosition[0].longitude + subdivisionWidth;
            geoPosition[1].altitude = elevationManager->getElevation(geoPosition[1].latitude, geoPosition[1].longitude);

            geoPosition[2].latitude = geoPosition[0].latitude + subdivisionHeight;
            geoPosition[2].longitude = geoPosition[0].longitude + subdivisionWidth;
            geoPosition[2].altitude = elevationManager->getElevation(geoPosition[2].latitude, geoPosition[2].longitude);

            geoPosition[3].latitude = geoPosition[0].latitude + subdivisionHeight;
            geoPosition[3].longitude = geoPosition[0].longitude;
            geoPosition[3].altitude = elevationManager->getElevation(geoPosition[3].latitude, geoPosition[3].longitude);

            xyzPosition[0] = Utilities::geodeticToXYZ(geoPosition[0]);
            xyzPosition[1] = Utilities::geodeticToXYZ(geoPosition[1]);
            xyzPosition[2] = Utilities::geodeticToXYZ(geoPosition[2]);
            xyzPosition[3] = Utilities::geodeticToXYZ(geoPosition[3]);

            //first triangle
            //equivalent to 0,0 of texture
            glTexCoord2f(texturePositionX, texturePositionY);
            glVertex3f(xyzPosition[0].x, xyzPosition[0].y, xyzPosition[0].z);

            //equivalent to 1,1 of texture
            glTexCoord2f(texturePositionX+textureWidth, texturePositionY+textureHeight);
            glVertex3f(xyzPosition[2].x, xyzPosition[2].y, xyzPosition[2].z);

            //equivalent to 0,1 of texture
            glTexCoord2f(texturePositionX, texturePositionY+textureHeight);
            glVertex3f(xyzPosition[3].x, xyzPosition[3].y, xyzPosition[3].z);

            //second triangle
            //equivalent to 0,0 of texture
            glTexCoord2f(texturePositionX, texturePositionY);
            glVertex3f(xyzPosition[0].x, xyzPosition[0].y, xyzPosition[0].z);

            //equivalent to 1,0 of texture
            glTexCoord2f(texturePositionX+textureWidth, texturePositionY);
            glVertex3f(xyzPosition[1].x, xyzPosition[1].y, xyzPosition[1].z);

            //equivalent to 1,1 of texture
            glTexCoord2f(texturePositionX+textureWidth, texturePositionY+textureHeight);
            glVertex3f(xyzPosition[2].x, xyzPosition[2].y, xyzPosition[2].z);

            //advance SW origin in X
            geoPosition[0].longitude += subdivisionWidth;
            geoPosition[0].altitude = elevationManager->getElevation(geoPosition[0].latitude, geoPosition[0].longitude);
          }

          //advance SW origin in Y and reset X
          geoPosition[0].latitude += subdivisionHeight;
          geoPosition[0].longitude = mMapList[mapIndex].southWest.longitude;
          geoPosition[0].altitude = elevationManager->getElevation(geoPosition[0].latitude, geoPosition[0].longitude);
        }

        glEnd();
        glDisable(GL_TEXTURE_2D);
      }
    }
  }

  if (!mElevationMode)
  {
    glDepthFunc(GL_LESS);
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Creates the spherical geometry used to render the Earth and star dome. See
 * en.wikipedia.org/wiki/Spherical_coordinates for a reference in spherical
 * coordinates.
 *
 * @param radius Radius of sphere geometry.
 */
void Earth::createSphereGeometry(double radius)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  int i,j;
  int numberOfMeridians = 36;
  int numberOfParallels = 18;
  double latitudeResolutionDeg = 180.0/(double)numberOfParallels;
  //theta is the angle between positive Z-axis and azimuth, similar to latitude
  //but latitude angle is between x-y plane and azimuth
  double theta1, theta2;
  //phi is analogous to longitude
  double phi1, phi2;
  double x1, y1, z1, x2, y2, z2, x3, y3, x4, y4;

  glBegin(GL_QUADS);

  //in mathematics, coordinates on the surface of a sphere
  //are calculated as follows:
  //x = r sin(theta) cos(phi)
  //y = r sin(theta) sin(phi)
  //z = r cos(theta)
  theta1 = latitudeResolutionDeg * Constants::DEGREES_TO_RADIANS;
  z1 = radius * cos(theta1);

  //draw north pole (notice how we come back to vertex 0,0,radius which is north pole)
  //the - 180 in phi comes from the fact that in OpenGL, texture coordinates 0,0 is the
  //lower left corner of the image, in equirectangular projection, that would be equivalent
  //to a longitude of -180
  for (i = 0; i < numberOfMeridians; i++)
  {
    phi1 = (((2.0 * Constants::PI)/(double)numberOfMeridians) * (double)i) - (180.0 * Constants::DEGREES_TO_RADIANS);
    phi2 = (((2.0 * Constants::PI)/(double)numberOfMeridians) * (double)(i+1)) - (180.0 * Constants::DEGREES_TO_RADIANS);
    x1 = radius * sin(theta1) * cos(phi1);
    y1 = radius * sin(theta1) * sin(phi1);
    x2 = radius * sin(theta1) * cos(phi2);
    y2 = radius * sin(theta1) * sin(phi2);

    glTexCoord2f((float)i/(float)numberOfMeridians, 1.0f);
    glVertex3f(0.0f, 0.0f, radius);

    glTexCoord2f((float)i/(float)numberOfMeridians, 1.0f - (1.0f/(float)numberOfParallels));
    glVertex3f(x1, y1, z1);

    glTexCoord2f((float)(i+1)/(float)numberOfMeridians, 1.0f - (1.0f/(float)numberOfParallels));
    glVertex3f(x2, y2, z1);

    glTexCoord2f((float)(i+1)/(float)numberOfMeridians, 1.0f);
    glVertex3f(0.0f, 0.0f, radius);
  }

  //draw south pole (notice how we come back to vertex 0,0,-radius and -z1)
  for (i = 0; i < numberOfMeridians; i++)
  {
    phi1 = (((2.0 * Constants::PI)/(double)numberOfMeridians) * (double)i) - (180.0 * Constants::DEGREES_TO_RADIANS);
    phi2 = (((2.0 * Constants::PI)/(double)numberOfMeridians) * (double)(i+1)) - (180.0 * Constants::DEGREES_TO_RADIANS);
    x1 = radius * sin(theta1) * cos(phi1);
    y1 = radius * sin(theta1) * sin(phi1);
    x2 = radius * sin(theta1) * cos(phi2);
    y2 = radius * sin(theta1) * sin(phi2);

    glTexCoord2f((float)i/(float)numberOfMeridians, 1.0f/(float)numberOfParallels);
    glVertex3f(x1, y1, -z1);

    glTexCoord2f((float)i/(float)numberOfMeridians, 0.0f);
    glVertex3f(0.0f, 0.0f, -radius);

    glTexCoord2f((float)(i+1)/(float)numberOfMeridians, 0.0f);
    glVertex3f(0.0f, 0.0f, -radius);

    glTexCoord2f((float)(i+1)/(float)numberOfMeridians, 1.0f/(float)numberOfParallels);
    glVertex3f(x2, y2, -z1);
  }

  int hemisphereParallels = numberOfParallels/2;

  //draw northern hemisphere
  for (j = 0; j < hemisphereParallels-1; j++)
  {
    theta1 = (latitudeResolutionDeg * Constants::DEGREES_TO_RADIANS) +
      ( ( (Constants::PI/2.0) / (double)hemisphereParallels) * j );

    theta2 = theta1 + (latitudeResolutionDeg * Constants::DEGREES_TO_RADIANS);
    z1 = radius * cos(theta1);
    z2 = radius * cos(theta2);

    for (i = 0; i < numberOfMeridians; i++)
    {
      phi1 = (((2.0 * Constants::PI)/(double)numberOfMeridians) * (double)i) - (180.0 * Constants::DEGREES_TO_RADIANS);
      phi2 = (((2.0 * Constants::PI)/(double)numberOfMeridians) * (double)(i+1)) - (180.0 * Constants::DEGREES_TO_RADIANS);
      x1 = radius * sin(theta1) * cos(phi1);
      y1 = radius * sin(theta1) * sin(phi1);
      x2 = radius * sin(theta1) * cos(phi2);
      y2 = radius * sin(theta1) * sin(phi2);

      x3 = radius * sin(theta2) * cos(phi1);
      y3 = radius * sin(theta2) * sin(phi1);
      x4 = radius * sin(theta2) * cos(phi2);
      y4 = radius * sin(theta2) * sin(phi2);

      glTexCoord2f((float)i/(float)numberOfMeridians, 1.0f - (float(j+1)/(float)numberOfParallels));
      glVertex3f(x1, y1, z1);

      glTexCoord2f((float)i/(float)numberOfMeridians, 1.0f - (float(j+2)/(float)numberOfParallels));
      glVertex3f(x3, y3, z2);

      glTexCoord2f((float)(i+1)/(float)numberOfMeridians, 1.0f - (float(j+2)/(float)numberOfParallels));
      glVertex3f(x4, y4, z2);

      glTexCoord2f((float)(i+1)/(float)numberOfMeridians, 1.0f - (float(j+1)/(float)numberOfParallels));
      glVertex3f(x2, y2, z1);
    }
  }

  //draw southern hemisphere
  for (j = 0; j < hemisphereParallels-1; j++)
  {
    theta1 = (latitudeResolutionDeg * Constants::DEGREES_TO_RADIANS) +
      ( ( (Constants::PI/2.0) / (float)hemisphereParallels) * j );

    theta2 = theta1 + (latitudeResolutionDeg * Constants::DEGREES_TO_RADIANS);
    z1 = radius * cos(theta1);
    z2 = radius * cos(theta2);

    for (i = 0; i < numberOfMeridians; i++)
    {
      phi1 = (((2.0 * Constants::PI)/(double)numberOfMeridians) * (double)i) - (180.0 * Constants::DEGREES_TO_RADIANS);
      phi2 = (((2.0 * Constants::PI)/(double)numberOfMeridians) * (double)(i+1)) - (180.0 * Constants::DEGREES_TO_RADIANS);
      x1 = radius * sin(theta1) * cos(phi1);
      y1 = radius * sin(theta1) * sin(phi1);
      x2 = radius * sin(theta1) * cos(phi2);
      y2 = radius * sin(theta1) * sin(phi2);

      x3 = radius * sin(theta2) * cos(phi1);
      y3 = radius * sin(theta2) * sin(phi1);
      x4 = radius * sin(theta2) * cos(phi2);
      y4 = radius * sin(theta2) * sin(phi2);

      glTexCoord2f((float)i/(float)numberOfMeridians, float(j+2)/(float)numberOfParallels);
      glVertex3f(x3, y3, -z2);

      glTexCoord2f((float)i/(float)numberOfMeridians, float(j+1)/(float)numberOfParallels);
      glVertex3f(x1, y1, -z1);

      glTexCoord2f((float)(i+1)/(float)numberOfMeridians, float(j+1)/(float)numberOfParallels);
      glVertex3f(x2, y2, -z1);

      glTexCoord2f((float)(i+1)/(float)numberOfMeridians, float(j+2)/(float)numberOfParallels);
      glVertex3f(x4, y4, -z2);
    }
  }

  glEnd();
}
