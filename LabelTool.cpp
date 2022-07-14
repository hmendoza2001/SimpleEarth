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

#include "LabelTool.h"
#include "WorldObjectManager.h"
#include "Utilities.h"
#include "LabelWindow.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Calls parent constructor with an ID of "Label" and with the
 * given parameter values. Initializes attributes.
 *
 * @param toolButton Handle to corresponding toolbar button
 * @param dialog Handle to corresponding tool dialog
 */
LabelTool::LabelTool(QToolButton* toolButton, QDialog* dialog) : Tool("Label", toolButton, dialog)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //Instantiate dummy label and add it to world object manager.
  //This is the label that will get rendered while the user is
  //editing the label's attributes
  mDummyLabel = new WorldObject();
  mDummyLabel->setIsVisible(false);
  mDummyLabel->setName("LabelToolDummy");
  mDummyLabel->setLabel("Label");
  WorldObjectManager::getInstance()->addWorldObject(mDummyLabel);

  mCalculateClickPosition = false;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
LabelTool::~LabelTool()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR Tool. Given that we need to calculate click(pick) position on
 * OpenGL render thread the this mCalculateClickPosition flag is set to true on
 * the onMouseReleaseEvent method. When calculation is done, this method updates
 * the label tool window with the selected(picked) coordinates. After the user
 * is done editting this label and hits the OK button, the system will call the
 * addCurrent method.
 */
void LabelTool::render()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mCalculateClickPosition)
  {
    //convert click coordinate to world coordinate
    SimpleVector worldCoordinate = Utilities::screenToWorld(mClickCordinates);

    //reset calculate flag
    mCalculateClickPosition = false;

    //update label position
    setPosition(worldCoordinate);

    //update label window
    LabelWindow* labelWindow = (LabelWindow*)mDialog;
    labelWindow->setPosition(worldCoordinate);
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Adds current volume to WorldObjectManager's list. Note how we copy all the
 * data from our dummy label.
 *
 * @return True if add was a success, false if there is a naming conflict (a
 * world object with that name already exists)
 */
bool LabelTool::addCurrent()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  bool returnValue = true;

  WorldObject* worldObject = new WorldObject;

  //copy all attributes
  worldObject->copy(mDummyLabel);
  //set object name with label's text
  worldObject->setName(mDummyLabel->getLabel());
  //set label as a group
  worldObject->setGroup(WorldObject::LABEL);

  //if addWorldObject returns false, then the add failed because
  //there was already an object with that name in the list,
  //so we need to delete the object we just created
  if (!WorldObjectManager::getInstance()->addWorldObject(worldObject))
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
 * Called on a right mouse button release event when the label tools is
 * selected. Saves click coordinates and sets the flag to calculate mouse click
 * position on the OpenGL render cycle.
 *
 * @param event Qt's mouse event
 */
void LabelTool::onMouseReleaseEvent(QMouseEvent* event)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mClickCordinates.x = event->x();
  mClickCordinates.y = event->y();
  mCalculateClickPosition = true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the current label's text.
 *
 * @param labelText New text for current(dummy) label
 */
void LabelTool::setLabelText(const QString& labelText)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mDummyLabel->setLabel(labelText);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the position for the current(dummy) label.
 *
 * @param position New position for the current label
 */
void LabelTool::setPosition(const SimpleVector& position)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mDummyLabel->setPosition(position);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the color for the current(dummy) label.
 *
 * @param color New color for the current label
 */
void LabelTool::setColor(const SimpleColor& color)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mDummyLabel->setColor(color);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the dummy lable's visibility so that we hide/show appropriately. Set
 * value to true to make label visible. Note that "View Labels" also has to be
 * enabled from the View menu in order to see any labels.
 *
 * @param value Set to true to make label visible
 */
void LabelTool::setDummyLabelVisible(bool value)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mDummyLabel->setIsVisible(value);
}
