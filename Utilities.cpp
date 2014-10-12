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

#include <QStringList>
#include <QGLWidget>
#include <GL/glu.h>
#include "math.h"
#include "Utilities.h"
#include "Constants.h"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Converts the given XYZ position to geodetic. Our XYZ coordinates are very
 * similar to ECEF coordinates, with center of the earth being (0,0), positive
 * x-axis passing through the prime meridian and the equator, and positive
 * z-axis passing through the north pole. However we model the earth as a
 * sphere, hence the XYZ position relative to the surface of the earth will not
 * be the same as in true ECEF coordinates. You can use ecefToGeodetic for that.
 *
 * @param xyzPosition XYZ position
 * @return Geodetic position
 */
GeodeticPosition Utilities::xyzToGeodetic(SimpleVector xyzPosition)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  GeodeticPosition geodeticPosition;

  double radius = sqrt(xyzPosition.x*xyzPosition.x + xyzPosition.y*xyzPosition.y + xyzPosition.z*xyzPosition.z);
  double theta = acos(xyzPosition.z / radius);
  double phi = atan2(xyzPosition.y, xyzPosition.x);

  geodeticPosition.latitude = ((theta * Constants::RADIANS_TO_DEGREES) - 90.0) * -1;
  geodeticPosition.longitude = phi * Constants::RADIANS_TO_DEGREES;
  geodeticPosition.altitude = radius - Constants::EARTH_MEAN_RADIUS;

  return geodeticPosition;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Converts the given geodetic position to XYZ. Our XYZ coordinates are very
 * similar to ECEF coordinates, with center of the earth being (0,0), positive
 * x-axis passing through the prime meridian and the equator, and positive
 * z-axis passing through the north pole. However we model the earth as a
 * sphere, hence the XYZ position relative to the surface of the earth will not
 * be the same as in true ECEF coordinates. You can use ecefToGeodetic for that.
 *
 * @param geodeticPosition Geodetic position
 * @return XYZ position
 */
SimpleVector Utilities::geodeticToXYZ(GeodeticPosition geodeticPosition)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  SimpleVector xyzPosition;

  double theta = (geodeticPosition.latitude - 90.0) * -Constants::DEGREES_TO_RADIANS;
  double phi = geodeticPosition.longitude * Constants::DEGREES_TO_RADIANS;
  double radius = Constants::EARTH_MEAN_RADIUS + geodeticPosition.altitude;

  xyzPosition.x = radius * sin(theta) * cos(phi);
  xyzPosition.y = radius * sin(theta) * sin(phi);
  xyzPosition.z = radius * cos(theta);

  return xyzPosition;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Converts the given string in the format [D]DD:MM:SS.S(N/S/E/W) e.g.
 * 106:00:00.0W, into decimal degrees.
 *
 * @param dmsString String in the format specified above
 * @return Decimal degrees
 */
double Utilities::dmsToDecimalDegrees(QString dmsString)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  double decimalDegrees = 0.0;
  double sign = 1.0;
  QStringList split;

  //split string
  split = dmsString.split(":");

  //check that we have the right amount of tokens
  if (split.size() < 3)
  {
    return 0.0;
  }

  //add degrees and minutes
  decimalDegrees += split[0].toDouble();//degrees
  decimalDegrees += split[1].toDouble()/60.0;//minutes

  //first check is the last split contains S or W
  //to compute the sign
  if (split[2].contains("S") || split[2].contains("W"))
  {
    sign = -1.0;
  }

  //remove letter from string
  split[2] = split[2].remove("N");
  split[2] = split[2].remove("S");
  split[2] = split[2].remove("E");
  split[2] = split[2].remove("W");

  //now that the letter has been removed, compute and add seconds
  decimalDegrees += split[2].toDouble()/3600.0;//seconds

  //finally compute sign
  decimalDegrees *= sign;

  return decimalDegrees;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Converts decimal degrees to a string in the format [D]DD:MM:SS.S(N/S/E/W)
 * e.g. 106:00:00.0W.
 *
 * @param decimalDegrees Decimal degrees
 * @param isLatitude Flag to know if input/output is latitude or longitude.
 *                   True means latitude.
 * @return String in the format specified above
 */
QString Utilities::decimalDegreesToDMS(double decimalDegrees, bool isLatitude)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QString returnStr;

  //calculate latitude in degrees minutes seconds and display
  int degrees = decimalDegrees;//clamp
  double minutes = (fabs(decimalDegrees) - fabs((double)degrees)) * 60.0;
  int minutesInt = minutes;//clamp;
  double seconds = (minutes - (double)minutesInt) * 60.0;//clamp;

  QString postfix;
  if (isLatitude)
  {
    postfix = "N";
  }
  else
  {
    postfix = "E";
  }

  if (decimalDegrees < 0.0)
  {
    degrees = -degrees;

    if (isLatitude)
    {
      postfix = "S";
    }
    else
    {
      postfix = "W";
    }
  }

  returnStr = QString::number(degrees) + ":" +
              QString::number(minutesInt) + ":" +
              QString::number(seconds, 'f', 1) + postfix;

  return returnStr;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Converts the given Earth Centered Earth Fixed (ECEF) coordinates to geodetic.
 *
 * @param ecefPosition XYZ vector representing ECEF coordinates
 * @return Geodetic position
 */
GeodeticPosition Utilities::ecefToGeodetic(SimpleVector ecefPosition)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  GeodeticPosition geodeticPosition;

  double x = ecefPosition.x;
  double y = ecefPosition.y;
  double z = ecefPosition.z;
  double a = 6378137.0; //semi major axis
  double b = 6356752.3142; //semi minor axis

  double eSquared; //first eccentricity squared
  double rSubN; //radius of the curvature of the prime vertical
  double ePrimeSquared;//second eccentricity squared
  double W = sqrt((x*x + y*y));

  eSquared = (a*a - b*b) / (a*a);
  ePrimeSquared = (a*a - b*b) / (b*b);

  //get the longitude.
  if (x >= 0 )
  {
    geodeticPosition.longitude = atan(y/x);
  }
  else if (x < 0 && y >= 0)
  {
    geodeticPosition.longitude = atan(y/x) + Constants::PI;
  }
  else
  {
    geodeticPosition.longitude = atan(y/x) - Constants::PI;
  }
  geodeticPosition.longitude = geodeticPosition.longitude * Constants::RADIANS_TO_DEGREES;

  //longitude calculation done. Now calculate latitude.
  //NOTE: The handbook mentions using the calculated phi (latitude) value to recalculate B
  //using tan B = (1-f) tan phi and then performing the entire calculation again to get more accurate values.
  //However, for terrestrial applications, one iteration is accurate to .1 millimeter on the surface  of the
  //earth (Rapp, 1984, p.124), so one iteration is enough for our purposes
  double tanBZero = (a * z) / (b * W);
  double BZero = atan((tanBZero));
  double tanPhi = (z + (ePrimeSquared * b * (pow(sin(BZero), 3))) ) /(W - (a * eSquared * (pow(cos(BZero), 3))));
  double phi = atan(tanPhi);
  geodeticPosition.latitude = phi;
  geodeticPosition.latitude = geodeticPosition.latitude * Constants::RADIANS_TO_DEGREES;

  //latitude done, now get the elevation. Note: The handbook states that near the poles, it is preferable to use
  //h = (Z / sin phi ) - rSubN + (eSquared * rSubN). Our applications are never near the poles, so this formula
  //was left unimplemented.
  rSubN = (a*a) / sqrt(((a*a) * (cos(phi) * cos(phi)) + ((b*b) * (sin(phi) * sin(phi)))));

  geodeticPosition.altitude = (W / cos(phi)) - rSubN;

  return geodeticPosition;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Converts the given string representing UTM coordinates (not to be confused
 * with MGRS, which is derived from UTM) in the format "{zone} {hemisphere}
 * {easting} {northing}" e.g. "54 N 388599 3964370", to geodetic.
 *
 * @param utmString String in the format specified above
 * @return Geodetic position
 */
GeodeticPosition Utilities::utmToGeodetic(QString utmString)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QStringList utmTokens = utmString.split(" ");
  int zone = utmTokens[0].toInt();
  QString hemisphere = utmTokens[1];
  double easting = utmTokens[2].toDouble();
  double northing = utmTokens[3].toDouble();

  double latitude = 0.0;
  double longitude = 0.0;

  if (hemisphere == "S")
  {
    northing = 10000000.0 - northing;
  }

  double a = 6378137.0;
  double e = 0.081819191;
  double k0 = 0.9996;
  double e1sq = 0.006739497;
  double arc = northing / k0;

  double mu = arc / (a * (1.0 - pow(e, 2) / 4.0 - 3.0 * pow(e, 4) / 64.0 - 5.0 * pow(e, 6) / 256.0));
  double ei = (1.0 - pow((1.0 - e * e), (1.0 / 2.0))) / (1.0 + pow((1.0 - e * e), (1.0 / 2.0)));
  double ca = 3.0 * ei / 2.0 - 27.0 * pow(ei, 3) / 32.0;
  double cb = 21.0 * pow(ei, 2) / 16.0 - 55.0 * pow(ei, 4) / 32.0;
  double cc = 151.0 * pow(ei, 3) / 96.0;
  double cd = 1097.0 * pow(ei, 4) / 512.0;
  double phi1 = mu + ca * sin(2.0 * mu) + cb * sin(4.0 * mu) + cc * sin(6.0 * mu) + cd * sin(8.0 * mu);
  double n0 = a / pow((1.0 - pow((e * sin(phi1)), 2)), (1.0 / 2.0));
  double r0 = a * (1.0 - e * e) / pow((1.0 - pow((e * sin(phi1)), 2)), (3.0 / 2.0));
  double fact1 = n0 * tan(phi1) / r0;
  double _a1 = 500000.0 - easting;
  double dd0 = _a1 / (n0 * k0);
  double fact2 = dd0 * dd0 / 2.0;
  double t0 = pow(tan(phi1), 2);
  double Q0 = e1sq * pow(cos(phi1), 2);
  double fact3 = (5.0 + 3.0 * t0 + 10.0 * Q0 - 4.0 * Q0 * Q0 - 9.0 * e1sq) * pow(dd0, 4) / 24.0;
  double fact4 = (61.0 + 90.0 * t0 + 298.0 * Q0 + 45.0 * t0 * t0 - 252.0 * e1sq - 3.0 * Q0 * Q0) * pow(dd0, 6) / 720.0;
  double lof1 = _a1 / (n0 * k0);
  double lof2 = (1.0 + 2.0 * t0 + Q0) * pow(dd0, 3) / 6.0;
  double lof3 = (5.0 - 2.0 * Q0 + 28.0 * t0 - 3.0 * pow(Q0, 2) + 8.0 * e1sq + 24.0 * pow(t0, 2)) * pow(dd0, 5) / 120.0;
  double _a2 = (lof1 - lof2 + lof3) / cos(phi1);
  double _a3 = _a2 * 180.0 / Constants::PI;

  latitude = 180.0 * (phi1 - fact1 * (fact2 + fact3 + fact4)) / Constants::PI;
  double zoneCM;
  if (zone > 0)
  {
    zoneCM = 6.0 * zone - 183.0;
  }
  else
  {
    zoneCM = 3.0;
  }

  longitude = zoneCM - _a3;
  if (hemisphere == "S")
  {
    latitude = -latitude;
  }

  GeodeticPosition geodeticPos;
  geodeticPos.latitude = latitude;
  geodeticPos.longitude = longitude;
  geodeticPos.altitude = 0.0;
  return geodeticPos;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns true if earth is obscuring the given object position based on given
 * camera position.
 *
 * @param cameraPosition Geodetic position of camera
 * @param objectPosition Geodetic position of object
 * @return True if earth is obscuring the given object position
 */
bool Utilities::checkObscure(GeodeticPosition cameraPosition, GeodeticPosition objectPosition)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  bool returnValue = true;

  //get obscure angle depending on camera altitude
  float obscureAngle = 0.0f;
  if (cameraPosition.altitude < 5000.0)
  {
    obscureAngle = 65.0f;
  }
  else if (cameraPosition.altitude < 10000.0)
  {
    obscureAngle = 75.0f;
  }
  else
  {
    obscureAngle = 90.0f;
  }

  //check latitude difference first
  float latPosDifference = fabs(cameraPosition.latitude - objectPosition.latitude);
  if (latPosDifference >= obscureAngle)
  {
    return true;
  }

  //get upper and lower visibility limits
  float cameraLongPlusObscure = cameraPosition.longitude + obscureAngle;
  float cameraLongMinusObscure = cameraPosition.longitude - obscureAngle;

  //check if limits go beyond -180/180 boundaries and wrap values
  bool objectInsideWrap = false;
  if (cameraLongPlusObscure > 180.0f)
  {
    float cameraPosWrap = cameraLongPlusObscure - 360.0f;
    if (objectPosition.longitude < 0.0f && objectPosition.longitude < cameraPosWrap)
    {
      objectInsideWrap = true;
    }
  }

  if (cameraLongMinusObscure < -180.0f)
  {
    float cameraPosWrap = cameraLongMinusObscure + 360.0f;
    if (objectPosition.longitude > 0.0f && objectPosition.longitude > cameraPosWrap)
    {
      objectInsideWrap = true;
    }
  }

  //finally, check if track is obscured by earth
  if ( (objectPosition.longitude < cameraLongPlusObscure &&
        objectPosition.longitude > cameraLongMinusObscure) ||
       objectInsideWrap )
  {
    returnValue = false;
  }

  return returnValue;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * WARNING: This method is supposed to get called from within OpenGL rendering
 * context. Reads the image file specifid by file path and returns the
 * corresponding OpenGL texture handle.
 *
 * @param image Pointer to QImage
 * @return The OpenGL texture ID
 */
unsigned int Utilities::imageToTexture(QImage* image)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
    //QImage image;
    QImage glFormatImage;
    unsigned int textureID;

    //convert to OpenGL format
    glFormatImage = QGLWidget::convertToGLFormat(*image);

    //create texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, glFormatImage.width(), glFormatImage.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, glFormatImage.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    return textureID;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * WARNING: This method is supposed to get called from within OpenGL rendering
 * context. Converts the given screen coordinates to world coordinates.
 *
 * @param screenPosition Screen coordinates
 * @return World coordinates
 */
SimpleVector Utilities::screenToWorld(ScreenCoordinates screenPosition)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  SimpleVector returnValue;

  //get model-view and viewport
  GLdouble model[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, model);
  GLdouble projection[16];
  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  GLint view[4];
  glGetIntegerv(GL_VIEWPORT, view);
  GLdouble x,y,z;
  float winX, winY, winZ;

  winX = screenPosition.x;
  winY = view[3] - screenPosition.y;

  //IMPORTANT NOTE: You gotta be careful, if you give a glDouble to read pixels
  //for the output it won't populate the least significant bits which always
  //gives you a 0.0
  glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

  //get current projected coordinates
  gluUnProject(winX, winY, winZ, model, projection, view, &x, &y, &z);

  returnValue.x = x;
  returnValue.y = y;
  returnValue.z = z;

  return returnValue;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * WARNING: This method is supposed to get called from within OpenGL rendering
 * context. Converts the given world coordinates to screen coordinates.
 *
 * @param worldPosition Woorld coordinates
 * @return Screen coordiates (z value is used to determine if object lies
 *  behind the camera)
 */
SimpleVector Utilities::worldToScreen(SimpleVector worldPosition)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  SimpleVector returnValue;

  //get model-view and viewport
  GLdouble model[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, model);
  GLdouble projection[16];
  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  GLint view[4];
  glGetIntegerv(GL_VIEWPORT, view);
  GLdouble screenX, screenY, screenZ;

  //get current projected location
  gluProject(worldPosition.x, worldPosition.y, worldPosition.z, model,
             projection, view, &screenX, &screenY, &screenZ);

  returnValue.x = screenX;
  returnValue.y = screenY;
  returnValue.z = screenZ;//z coordinate is used to know if icon lies behind the camera

  return returnValue;
}
