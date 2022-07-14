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

#include "ToolManager.h"

//Singleton implementation
ToolManager* ToolManager::mInstance = NULL;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Initializes attributes.
 */
ToolManager::ToolManager()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mCurrentToolIndex = -1;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
ToolManager::~ToolManager()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //release all data
  for (int i = 0; i < mToolList.size(); i++)
  {
    delete mToolList[i];
  }
  mToolList.clear();

  mInstance = NULL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Singleton pattern implementation. Returns the single instance of this class.
 *
 * @return The single instance of this object.
 */
ToolManager* ToolManager::getInstance()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mInstance == NULL)
  {
    mInstance = new ToolManager();
  }

  return mInstance;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Adds a Tool to the tool list.
 *
 * @param tool New tool to add to the list
 */
void ToolManager::addTool(Tool* tool)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mToolList.append(tool);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the handle to the tool that corresponds to the given ID.
 *
 * @param toolID String identifying the tool
 * @return Handle to corresponding tool
 */
Tool* ToolManager::getTool(const QString& toolID)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Tool* returnValue = NULL;

  for (int i = 0; i < mToolList.size(); i++)
  {
    if (toolID == mToolList[i]->getToolID())
    {
      returnValue = mToolList[i];
      break;
    }
  }

  return returnValue;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Selects the tool with the given toolID as the current tool. If toolID is
 * empty string (or not found), all windows will be hidden and all the tool
 * buttons unchecked. Also no tool will be rendered.
 *
 * @param toolID String identifying tool
 */
void ToolManager::selectTool(const QString& toolID)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mCurrentToolIndex = -1;

  for (int i = 0; i < mToolList.size(); i++)
  {
    if (toolID == mToolList[i]->getToolID())
    {
      //show dialog and set current index for tool to be rendered
      //though there is no need to set tool button to checked if
      //action was triggered by user, it is here in case this method
      //is called programatically
      mCurrentToolIndex = i;
      mToolList[i]->getDialog()->show();
      mToolList[i]->getToolButton()->setChecked(true);
    }
    else
    {
      //uncheck all other tool buttons and hide their dialogs
      mToolList[i]->getDialog()->hide();
      mToolList[i]->getToolButton()->setChecked(false);
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Calls the render method for the currently selected tool.
 */
void ToolManager::renderSelectedTool()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mCurrentToolIndex >= 0 && mCurrentToolIndex < mToolList.size())
  {
    mToolList[mCurrentToolIndex]->render();
  }
}
