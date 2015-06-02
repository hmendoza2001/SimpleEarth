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

#include <QMessageBox>
#include "PathWindow.h"
#include "ui_PathWindow.h"
#include "MainWindow.h"
#include "PathTool.h"
#include "ToolMgr.h"
#include "WorldObjectMgr.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Connects signals and slots.
 *
 * @param parent Handle to parent widget
 */
PathWindow::PathWindow(QWidget *parent) : QDialog(parent), ui(new Ui::PathWindow)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  ui->setupUi(this);

  connect(ui->color, SIGNAL(colorSelected()), this, SLOT(onColorSelected()));
  connect(this, SIGNAL(accepted()), this, SLOT(onAccept()));
  connect(this, SIGNAL(rejected()), this, SLOT(onReject()));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
PathWindow::~PathWindow()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  delete ui;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Initializes path name text editor and calls the init function for Path Tool
 * WorldObject.
 */
void PathWindow::initialize()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //get number of current paths to append to current name
  WorldObject* worldObject;
  WorldObjectMgr* worldObjectMgr = WorldObjectMgr::getInstance();
  int currentNumberOfPaths = 0;
  for (int i = 0; i < worldObjectMgr->getNumberOfObjects(); i++)
  {
    worldObject = worldObjectMgr->getWorldObject(i);
    if (worldObject != NULL && !worldObject->getHasExpired() &&
        worldObject->getGroup() == WorldObject::PATH)
    {
      currentNumberOfPaths++;
    }
  }
  QString pathName = "Path" + QString::number(currentNumberOfPaths+1);
  ui->name->setText(pathName);

  //get handle to PathTool
  PathTool* pathTool = (PathTool*)ToolMgr::getInstance()->getTool("Path");
  pathTool->initialize();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Modifies the current path color.
 */
void PathWindow::onColorSelected()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QColor qColor = ui->color->getCurrentColor();
  SimpleColor color;
  color.red = qColor.redF();
  color.green = qColor.greenF();
  color.blue = qColor.blueF();
  color.alpha = 1.0f;
  PathTool* pathTool = (PathTool*)ToolMgr::getInstance()->getTool("Path");
  pathTool->setColor(color);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the user clicks the OK button. Calls the Path Tool
 * to attempt to add the path.
 */
void PathWindow::onAccept()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  PathTool* pathTool = (PathTool*)ToolMgr::getInstance()->getTool("Path");

  //set current name
  pathTool->setName(ui->name->text());

  if (!pathTool->addCurrent())
  {
    QMessageBox::warning(this, "Warning", "Name already used. Please choose another name.");
    this->show();
  }
  else
  {
    closeEvent(NULL);
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the user clicks the Cancel button. Calls the
 * closeEvent function to cleanup after itself.
 */
void PathWindow::onReject()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  closeEvent(NULL);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR Qdialog. Gets callled whenever we close this dialog. This
 * mimics the functionality as if the user had clicked (deselect) the path tool
 * button.
 *
 * @param event Qt's close event (not used but necessary for override)
 */
void PathWindow::closeEvent(QCloseEvent*)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  MainWindow* mainWindow = MainWindow::getInstance();
  mainWindow->getToolButton(MainWindow::PATH_TOOL_BUTTON)->setChecked(false);
  mainWindow->onPathTool();
}
