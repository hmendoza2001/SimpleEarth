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
#include "ShapeRenderer.h"
#include "Utilities.h"
#include "WorldObject.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Initializes attributes and calls parent's constructor.
 */
ShapeRenderer::ShapeRenderer() : MeshRenderer()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mHasHeight = false;
  mDisplayListIndex = 0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
ShapeRenderer::~ShapeRenderer()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the current value of the "has height" flag. This flag is set to true
 * only if there is height data associated with this shape.
 *
 * @return True if this shapefile has height data associated to it
 */
bool ShapeRenderer::getHasHeight()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mHasHeight;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the "has height" flag. This flag should only be set to true if there
 * is height data associated with this shape.
 *
 * @param value New value for has height flag
 */
void ShapeRenderer::setHasHeight(bool value)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mHasHeight = value;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Adds a ground point to the shape. Ground points are the default vertices of
 * a shape in case there is no height data associated with the shape.
 *
 * @param point New point in XYZ coordinates
 */
void ShapeRenderer::addGroundPoint(SimpleVector point)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mGroundPointList.append(point);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Adds a height point to the shape. Height points are used to "extrude" the
 * shape into 3 dimensions.
 *
 * @param point New point in XYZ coordinates
 */
void ShapeRenderer::addHeightPoint(SimpleVector point)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mHeightPointList.append(point);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Renders the shape by calling createVertices to render an outline and
 * semi-transparent walls (in case there is height data associated with this
 * shape).
 */
void ShapeRenderer::render()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mDisplayListIndex == 0)
  {
    mDisplayListIndex = glGenLists(1);
    glNewList(mDisplayListIndex, GL_COMPILE);

    SimpleColor color = mWorldObject->getColor();
    glColor4f(color.red, color.green, color.blue, 0.5f);

    glPointSize(3.0f);

    //draw outline
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_BACK, GL_LINE);
    glPolygonMode(GL_FRONT, GL_LINE);

    createVertices();

    //draw semi transparent polygons
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glPolygonMode(GL_BACK, GL_FILL);
    glPolygonMode(GL_FRONT, GL_FILL);

    createVertices();

    glDisable(GL_BLEND);

    glEndList();
  }
  else
  {
    glCallList(mDisplayListIndex);
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Builds the vertices for the shape. A basic shape is just a polygon, if there
 * is associated height data, we build walls around the polygon by extruding
 * it.
 */
void ShapeRenderer::createVertices()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //shape must have at least two points
  if (mGroundPointList.size() > 2)
  {
    //draw shape, if height data associated with it, draw it as the top of the
    //extruded polygon, for loop is in reverse order to draw counterclock wise
    //(for OpenGL requirements)
    glBegin(GL_POLYGON);
    for (int i=mGroundPointList.size()-1; i>=0; i--)
    {
      if (mHasHeight)
      {
        glVertex3f(mHeightPointList[i].x, mHeightPointList[i].y, mHeightPointList[i].z);
      }
      else
      {
        glVertex3f(mGroundPointList[i].x, mGroundPointList[i].y, mGroundPointList[i].z);
      }
    }
    glEnd();

    //check if there is associated height data
    if (mHasHeight)
    {
      //draw shape walls through extrusion
      for (int i=0; i<mHeightPointList.size()-1; i++)
      {
        glBegin(GL_QUADS);

        glVertex3f(mGroundPointList[i].x, mGroundPointList[i].y, mGroundPointList[i].z);
        glVertex3f(mHeightPointList[i].x, mHeightPointList[i].y, mHeightPointList[i].z);
        glVertex3f(mHeightPointList[i+1].x, mHeightPointList[i+1].y, mHeightPointList[i+1].z);
        glVertex3f(mGroundPointList[i+1].x, mGroundPointList[i+1].y, mGroundPointList[i+1].z);

        glEnd();
      }
    }
  }// end of if (mGroundPointList.size() > 2)
}
