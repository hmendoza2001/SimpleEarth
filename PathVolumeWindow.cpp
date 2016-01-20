/*
 *  The Simple Earth Project
 *  Copyright (C) 2016 HueSoft LLC
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

#include <QMenu>
#include <QList>
#include <QMessageBox>
#include "PathVolumeWindow.h"
#include "ui_PathVolumeWindow.h"
#include "WorldObjectManager.h"
#include "MainWindow.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Connects signals and slots.
 *
 * @param parent Handle to parent widget
 */
PathVolumeWindow::PathVolumeWindow(QWidget *parent) : QDialog(parent), ui(new Ui::PathVolumeWindow)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  ui->setupUi(this);

  mShowContextMenu = false;
  mCurrentTreeItem = NULL;

  connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onItemChanged(QTreeWidgetItem*,int)));
  connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenu(QPoint)));
  connect(ui->treeWidget, SIGNAL(itemPressed(QTreeWidgetItem*,int)), this, SLOT(onItemPressed(QTreeWidgetItem*,int)));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
PathVolumeWindow::~PathVolumeWindow()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  delete ui;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This function gets called when the user selects the menu option to display
 * this window. It gathers the relevant entities from WorldObjectManager and
 * populates the different trees.
 */
void PathVolumeWindow::initialize()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  WorldObject* worldObject = NULL;
  WorldObjectManager* worldObjectManager = WorldObjectManager::getInstance();
  QTreeWidgetItem* pathsTreeItem = ui->treeWidget->topLevelItem(0);
  QTreeWidgetItem* volumeTreeItem = ui->treeWidget->topLevelItem(1);
  int i=0;

  //remove all items in both lists in order to start with a fresh list
  QList<QTreeWidgetItem*> pathList = pathsTreeItem->takeChildren();
  for (i = 0; i < pathList.size(); i++)
  {
    delete pathList[i];
  }
  QList<QTreeWidgetItem*> volumeList = volumeTreeItem->takeChildren();
  for (i = 0; i < volumeList.size(); i++)
  {
    delete volumeList[i];
  }

  mCurrentTreeItem = NULL;

  for (i = 0; i < worldObjectManager->getNumberOfObjects(); i++)
  {
    worldObject = worldObjectManager->getWorldObject(i);
    if (worldObject != NULL && !worldObject->getHasExpired() &&
        (worldObject->getGroup() == WorldObject::PATH ||
         worldObject->getGroup() == WorldObject::VOLUME))
    {
      QTreeWidgetItem* item = new QTreeWidgetItem();
      item->setText(0, worldObject->getName());

      if (worldObject->getIsVisible())
      {
        item->setCheckState(0, Qt::Checked);
      }
      else
      {
        item->setCheckState(0, Qt::Unchecked);
      }

      //add to correct branch/group
      if (worldObject->getGroup() == WorldObject::PATH)
      {
        pathsTreeItem->addChild(item);
      }
      else if (worldObject->getGroup() == WorldObject::VOLUME)
      {
        volumeTreeItem->addChild(item);
      }
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. This item gets called when the user checks or unchecks an item in
 * the tree. It makes the object visible/invisible.
 *
 * @param item Which item in the tree was checked/unchecked
 * @param column Which column in the tree was changed
 */
void PathVolumeWindow::onItemChanged(QTreeWidgetItem* item, int column)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  WorldObject* worldObject = WorldObjectManager::getInstance()->getWorldObject(item->text(column));
  if (worldObject != NULL && !worldObject->getHasExpired())
  {
    if (item->checkState(column) == Qt::Checked)
    {
      worldObject->setIsVisible(true);
    }
    else
    {
      worldObject->setIsVisible(false);
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. This item gets called when the user right clicks on an item in the
 * tree. It pops up a custom menu. Once the option has been selected, it
 * performs the appropriate functionality.
 *
 * @param point two dimensional point relative to parent widget where the mouse
 * was clicked
 */
void PathVolumeWindow::onContextMenu(QPoint point)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //first check if context menu is ready for this item (see onItemPressed)
  if (mCurrentTreeItem == NULL)
  {
    return;
  }

  if (mShowContextMenu)
  {
    QList<QAction*> actions;
    actions.append(new QAction("Delete", this));

    //only show properties on volumes
    bool showProperties = false;
    QTreeWidgetItem* parent = mCurrentTreeItem->parent();
    if (parent->text(0) == "Volumes")
    {
      showProperties = true;
    }

    if (showProperties)
    {
      actions.append(new QAction("Properties", this));
    }

    //map point to global coordinates to display menu at the right place
    QPoint newPoint = this->mapToGlobal(point);
    QAction* action = QMenu::exec(actions, newPoint);

    if (action == actions[0])//delete action
    {
      int answer = QMessageBox::question(this, "Delete", "Are you sure you want to delete this item?", QMessageBox::Ok, QMessageBox::Cancel);
      if (answer == QMessageBox::Ok)
      {
        int index = parent->indexOfChild(mCurrentTreeItem);
        //remove object
        WorldObjectManager::getInstance()->removeWorldObject(mCurrentTreeItem->text(0));
        //remove tree item
        QTreeWidgetItem* deleteItem = parent->takeChild(index);
        delete deleteItem;
        mCurrentTreeItem = NULL;
      }
    }
    else if (showProperties)
    {
      if (action == actions[1])//properties action
      {
        MainWindow::getInstance()->displayVolumeEdit(mCurrentTreeItem->text(0));
      }
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. This item gets called when the user clicks on an item in the tree.
 * It determines if it is OK to show a context menu or not. If it is, it saves
 * a handle to the clicked item.
 *
 * @param item Handle to clicked item
 * @param column Which tree column is the clicked item in
 */
void PathVolumeWindow::onItemPressed(QTreeWidgetItem* item, int column)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //do not show options for parent nodes in the tree
  if (item->text(column) == "Paths" || item->text(column) == "Volumes")
  {
    mShowContextMenu = false;
  }
  else
  {
    mShowContextMenu = true;
    mCurrentTreeItem = item;
  }
}
