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
#include "PathTool.h"
#include "PathRenderer.h"
#include "WorldObjectManager.h"
#include "Utilities.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Calls parent constructor with given parameter values and
 * initializes attributes.
 *
 * @param toolButton Handle to corresponding toolbar button
 * @param dialog Handle to corresponding dialog
 */
PathTool::PathTool(QToolButton* toolButton, QDialog* dialog) : Tool("Path", toolButton, dialog)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mCurrentNumberOfPoints = 0;
  mClickCordinates.x = 0;
  mClickCordinates.y = 0;
  mAddPoint = false;
  mColor.red = 1.0f;
  mColor.green = 1.0f;
  mColor.blue = 1.0f;
  mColor.alpha = 1.0f;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
PathTool::~PathTool()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This method is called to initialize attributes for path tool rendering.
 */
void PathTool::initialize()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mCurrentNumberOfPoints = 0;
  mAddPoint = false;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR Tool. Builds the geometry to render a temporary path for
 * the tool.
 */
void PathTool::render()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //check if we need a projection computation because we are adding a point
  if (mAddPoint && mCurrentNumberOfPoints < MAX_NUMBER_OF_POINTS)
  {
    //convert click coordinates and add point to our list
    mPoints[mCurrentNumberOfPoints] = Utilities::screenToWorld(mClickCordinates);

    mAddPoint = false;
    mCurrentNumberOfPoints++;
  }

  //render path
  glColor4f(mColor.red, mColor.green, mColor.blue, 1.0f);
  glDisable(GL_DEPTH_TEST);
  glLineWidth(2.0f);
  glBegin(GL_LINE_STRIP);
  for (int i = 0; i < mCurrentNumberOfPoints; i++)
  {
    glVertex3f(mPoints[i].x, mPoints[i].y, mPoints[i].z);
  }
  glEnd();
  glLineWidth(1.0f);
  glEnable(GL_DEPTH_TEST);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the name for this path.
 *
 * @param name New name for this path
 */
void PathTool::setName(const QString& name)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mName = name;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the color for this path.
 *
 * @param color New color for this path
 */
void PathTool::setColor(const SimpleColor& color)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mColor = color;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Adds current path to WorldObjectManager's list. Note how we copy all the local
 * to a new instance of a path world object.
 */
bool PathTool::addCurrent()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  bool returnValue = true;

  //only add path if it has more than 1 point
  if (mCurrentNumberOfPoints > 1)
  {
    WorldObject* worldObject = new WorldObject();
    worldObject->setName(mName);
    worldObject->setColor(mColor);
    worldObject->setGroup(WorldObject::PATH);

    //instantiate path renderer and associate it with world object
    PathRenderer* pathRenderer = new PathRenderer();
    for (int i = 0; i < mCurrentNumberOfPoints; i++)
    {
      pathRenderer->addPoint(mPoints[i]);
    }
    worldObject->setMeshRenderer(pathRenderer);

    //if object already exists (cannot add) perform cleanup
    if (!WorldObjectManager::getInstance()->addWorldObject(worldObject))
    {
      //note: the path renderer that we associated with this world
      //object will get deleted on the world object's destructor
      delete worldObject;
      returnValue = false;
    }
  }

  return returnValue;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Called on a right mouse button release event when the path tools is selected.
 *
 * @param event Qt's mouse event
 */
void PathTool::onMouseReleaseEvent(QMouseEvent* event)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mClickCordinates.x = event->x();
  mClickCordinates.y = event->y();
  mAddPoint = true;
}
