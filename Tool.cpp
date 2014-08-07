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

#include "Tool.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Initializes attributes to given values.
 *
 * @param toolID String identifying the tool
 * @param toolButton Handle to corresponding toolbar button
 * @param dialog Handle to corresponding dialog
 */
Tool::Tool(QString toolID, QToolButton* toolButton, QDialog* dialog)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mToolID = toolID;
  mToolButton = toolButton;
  mDialog = dialog;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
Tool::~Tool()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the ID for this tool. The ID is the string that helps identify this
 * tool.
 *
 * @return String identifying this tool
 */
QString Tool::getToolID()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mToolID;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the handle to the tool button associated with this tool. A handle to
 * the tool button is necessary so that the Tool Manager can activate/deactivate
 * the button depending on which tool gets selected.
 *
 * @return Handle to corresponding toolbar button.
 */
QToolButton* Tool::getToolButton()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mToolButton;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the handle to the dialog associated with this tool. A handle to the
 * dialog is necessary so that the Tool Manager can open/close the dialog
 * depending on which tool gets selected.
 *
 * @return Handle to corresponding dialog for this tool.
 */
QDialog* Tool::getDialog()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mDialog;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the ID for this tool. The ID is the string that helps identify this
 * tool.
 *
 * @param toolID String identifying this tool
 */
void Tool::setToolID(QString toolID)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mToolID = toolID;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the handle to the tool button associated with this tool. A handle to the
 * tool button is necessary so that the Tool Manager can activate/deactivate the
 * button depending on which tool gets selected.
 *
 * @param toolButton New handle to corresponding toolbar button object
 */
void Tool::setToolButton(QToolButton* toolButton)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mToolButton = toolButton;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the handle to the dialog for this tool. A handle to the dialog is
 * necessary so that the Tool Manager can open/close the dialog depending on
 * which tool gets selected.
 *
 * @param dialog New handle to corresponding dialog object
 */
void Tool::setDialog(QDialog* dialog)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mDialog = dialog;
}
