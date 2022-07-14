/*
 *  The Simple Earth Project
 *  Copyright (C) 2022 HueSoft LLC
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

#include "MeasuringTool.h"
#include "Utilities.h"
#include "math.h"
#include "Constants.h"
#include "Camera.h"
#include "MeasuringWindow.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Calls parent constructor with ID of "Measuring" and given
 * parameter values.
 *
 * @param toolButton Handle to corresponding toolbar button
 * @param dialog Handle to corresponding dialog
 */
MeasuringTool::MeasuringTool(QToolButton* toolButton, QDialog* dialog) : Tool("Measuring", toolButton, dialog)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mStartPoint.x = 0.0;
  mStartPoint.y = 0.0;
  mStartPoint.z = 0.0;
  mEndPoint.z = 0.0;
  mEndPoint.z = 0.0;
  mEndPoint.z = 0.0;
  mCalculateStartPoint = false;
  mCalculateEndPoint = false;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
MeasuringTool::~MeasuringTool()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR WorldObject. Builds the geometry to render a measuring line.
 */
void MeasuringTool::render()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mCalculateStartPoint || mCalculateEndPoint)
  {
    SimpleVector worldCoordinate;

    //get screen coordinates for points
    if (mCalculateStartPoint)
    {
      worldCoordinate = Utilities::screenToWorld(mStartScreenCordinates);
      mStartPoint = worldCoordinate;
      mEndPoint = worldCoordinate;
      mCalculateStartPoint = false;
    }
    else
    {
      worldCoordinate = Utilities::screenToWorld(mEndScreenCordinates);
      mEndPoint = worldCoordinate;
      mCalculateEndPoint = false;
    }

    //calculate great circle distance from start to end point
    GeodeticPosition startPointGeo = Utilities::xyzToGeodetic(mStartPoint);
    GeodeticPosition endPointGeo = Utilities::xyzToGeodetic(mEndPoint);
    float greatCircleDistance = Constants::EARTH_MEAN_RADIUS * acos(
      sin(startPointGeo.latitude * Constants::DEGREES_TO_RADIANS) *
      sin(endPointGeo.latitude * Constants::DEGREES_TO_RADIANS) +
      (cos(startPointGeo.latitude * Constants::DEGREES_TO_RADIANS) *
       cos(endPointGeo.latitude * Constants::DEGREES_TO_RADIANS) *
       cos(fabs((startPointGeo.longitude * Constants::DEGREES_TO_RADIANS) -
       (endPointGeo.longitude * Constants::DEGREES_TO_RADIANS)))));

    MeasuringWindow* measuringWindow = (MeasuringWindow*)mDialog;
    measuringWindow->setMeasurement(greatCircleDistance);
  }

  //only render if start point is not being obscured by the earth
  GeodeticPosition startPointGeo = Utilities::xyzToGeodetic(mStartPoint);
  if (!Utilities::checkObscure(Camera::getInstance()->getGeodeticPosition(), startPointGeo))
  {
    if (!(mStartPoint.x == 0 && mStartPoint.y == 0 && mStartPoint.z == 0))
    {
      glDisable(GL_DEPTH_TEST);

      //draw end points
      glColor4f(1.0f,0.5f,0.0f,1.0f);
      glPointSize(6.0f);
      glBegin(GL_POINTS);
      glVertex3f(mStartPoint.x, mStartPoint.y, mStartPoint.z);
      glVertex3f(mEndPoint.x, mEndPoint.y, mEndPoint.z);
      glEnd();
      glPointSize(1.0f);

      //draw line
      glColor4f(0.0f,1.0f,1.0f,1.0f);
      glLineWidth(2.0f);
      glBegin(GL_LINES);
      glVertex3f(mStartPoint.x, mStartPoint.y, mStartPoint.z);
      glVertex3f(mEndPoint.x, mEndPoint.y, mEndPoint.z);
      glEnd();
      glLineWidth(1.0f);

      glEnable(GL_DEPTH_TEST);
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Called on a right mouse button press event when the measuring tools is
 * selected. Stores screen coordinates of current mouse position and sets the
 * flag to compute start point position on render loop.
 *
 * @param event Qt's mouse event
 */
void MeasuringTool::onMousePressEvent(QMouseEvent* event)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mStartScreenCordinates.x = event->x();
  mStartScreenCordinates.y = event->y();
  mCalculateStartPoint = true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Called on a right mouse button release event when the measuring tools is
 * selected. Stores screen coordinates of current mouse position and sets the
 * flag to compute end point position on render loop.
 *
 * @param event Qt's mouse event
 */
void MeasuringTool::onMouseReleaseEvent(QMouseEvent* event)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mEndScreenCordinates.x = event->x();
  mEndScreenCordinates.y = event->y();
  mCalculateEndPoint = true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Called on a right mouse button move event when the measuring tools is
 * selected. Basically performs same functionality as mouse release event,
 * however we duplicate functionality here since it is not good coding to
 * call the mouse release slot ourselves.
 *
 * @param event Qt's mouse event
 */
void MeasuringTool::onMouseMoveEvent(QMouseEvent* event)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mEndScreenCordinates.x = event->x();
  mEndScreenCordinates.y = event->y();
  mCalculateEndPoint = true;
}
