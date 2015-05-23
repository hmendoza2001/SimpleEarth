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
#include "VolumeTool.h"
#include "WorldObjectMgr.h"
#include "Utilities.h"
#include "MainWindow.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Calls parent constructor with an ID of "Volume" and with the
 * given parameter values. Initializes attributes.
 *
 * @param toolButton Handle to corresponding toolbar button
 * @param dialog Handle to corresponding dialog
 */
VolumeTool::VolumeTool(QToolButton* toolButton, QDialog* dialog) : Tool("Volume", toolButton, dialog)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mWorldObject = new WorldObject();
  mVolumeRenderer = new VolumeRenderer();
  mWorldObject->setMeshRenderer(mVolumeRenderer);
  mCalculateClickPosition = false;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
VolumeTool::~VolumeTool()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  delete mWorldObject;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR Tool. Builds the geometry to render a temporary volume. After
 * the user is done editting this volume, the system will call the addCurrent
 * method.
 */
void VolumeTool::render()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //given that we need to calculate click(pick) position on OpenGL render
  //thread this flag is set to true on the onMouseReleaseEvent method
  if (mCalculateClickPosition)
  {
    //convert click coordinate to world coordinate
    SimpleVector worldCoordinate = Utilities::screenToWorld(mClickCordinates);

    //reset calculate flag
    mCalculateClickPosition = false;

    //update volume position
    setPosition(worldCoordinate);

    //update volume window
    MainWindow::getInstance()->getVolumeWindow()->setPosition(worldCoordinate);
  }

  //render temporary volume
  mWorldObject->renderMesh();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Adds current volume to WorldObjectMgr's list. Note how we copy all the data
 * from temporary volume.
 *
 * @return true if add was a success, false if there is a naming conflict (a
 * world object with that name already exists)
 */
bool VolumeTool::addCurrent()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  bool returnValue = true;

  WorldObject* worldObject = new WorldObject;
  VolumeRenderer* volumeRenderer = new VolumeRenderer();
  volumeRenderer->setType(mVolumeRenderer->getType());

  //copy all attributes
  worldObject->copy(mWorldObject);
  //set volume name and label
  worldObject->setName(mWorldObject->getName());
  //set volume as a group
  worldObject->setGroup(WorldObject::VOLUME);
  worldObject->setMeshRenderer(volumeRenderer);

  //if addWorldObject returns false, then the add failed because
  //there already was an object with that name in the list,
  //so we need to delete the object we just created
  if (!WorldObjectMgr::getInstance()->addWorldObject(worldObject))
  {
    //note: the volume renderer that we associated with this world
    //object will get deleted on the world object's destructor
    delete worldObject;
    returnValue = false;
  }

  return returnValue;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This method is called when the user finishes editing a volume. We copy all
 * attributes of current rendered object into the given world object.
 *
 * @param worldObject Handle to world object we are editing
 */
void VolumeTool::editCurrent(WorldObject* worldObject)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  worldObject->copy(mWorldObject);
  //set world object group since mWorldObject is not part of volume group
  worldObject->setGroup(WorldObject::VOLUME);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Called on a right mouse button release event when the volume tools is
 * selected. Saves click coordinates and sets the flag to calculate mouse click
 * position on the OpenGL render cycle.
 *
 * @param event Qt's mouse event
 */
void VolumeTool::onMouseReleaseEvent(QMouseEvent* event)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mClickCordinates.x = event->x();
  mClickCordinates.y = event->y();
  mCalculateClickPosition = true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the name for the current volume.
 *
 * @param name New name for current volume
 */
void VolumeTool::setName(const QString& name)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mWorldObject->setName(name);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the volume type in VolumeRenderer for the current volume.
 *
 * @param type New type for the current volume
 */
void VolumeTool::setType(int type)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mVolumeRenderer->setType(type);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the position for the current volume.
 *
 * @param position New position for the current volume
 */
void VolumeTool::setPosition(const SimpleVector& position)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mWorldObject->setPosition(position);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the rotation for the current volume.
 *
 * @param rotation New rotation for the current volume
 */
void VolumeTool::setRotation(const SimpleVector& rotation)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mWorldObject->setRotation(rotation);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the scale for the current volume.
 *
 * @param scale New scale for the current volume
 */
void VolumeTool::setScale(const SimpleVector& scale)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mWorldObject->setScale(scale);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the color for the current volume.
 *
 * @param color New color for the current volume
 */
void VolumeTool::setColor(const SimpleColor& color)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mWorldObject->setColor(color);
}
