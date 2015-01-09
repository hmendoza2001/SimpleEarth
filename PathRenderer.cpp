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
#include "PathRenderer.h"
#include "Utilities.h"
#include "WorldObject.h"
#include "Camera.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Calls parent's overloaded constructor.
 */
PathRenderer::PathRenderer() : MeshRenderer()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
PathRenderer::~PathRenderer()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mPoints.clear();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR MeshRenderer. Builds the geometry and renders path.
 */
void PathRenderer::render()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mPoints.size() > 0)
  {
    SimpleVector pointXYZ = mPoints[0];
    GeodeticPosition pointGeodetic = Utilities::xyzToGeodetic(pointXYZ);

    //only render if first point is not being obscured by the earth
    if (!Utilities::checkObscure(Camera::getInstance()->getGeodeticPosition(), pointGeodetic))
    {
      SimpleColor color = mWorldObject->getColor();
      glColor4f(color.red, color.green, color.blue, 1.0f);

      glDisable(GL_DEPTH_TEST);
      glLineWidth(2.0f);
      glBegin(GL_LINE_STRIP);
      for (int i = 0; i < mPoints.size(); i++)
      {
        glVertex3f(mPoints[i].x, mPoints[i].y, mPoints[i].z);
      }
      glEnd();
      glLineWidth(1.0f);
      glEnable(GL_DEPTH_TEST);
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Adds a point to the path.
 *
 * @param point New point in XYZ coordinates
 */
void PathRenderer::addPoint(SimpleVector point)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mPoints.append(point);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns a handle to the point list.
 *
 * @return Handle to point list
 */
QList<SimpleVector>* PathRenderer::getPoints()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return &mPoints;
}
