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

#include <QMessageBox>

#include "LabelWindow.h"
#include "ui_LabelWindow.h"
#include "MainWindow.h"
#include "Utilities.h"
#include "ToolManager.h"
#include "Camera.h"
#include "LabelTool.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Calls parent constructor and connects signals and slots.
 *
 * @param parent Handle to parent widget
 */
LabelWindow::LabelWindow(QWidget *parent) : QDialog(parent), ui(new Ui::LabelWindow)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  ui->setupUi(this);

  //connect signals and slots
  connect(ui->color, SIGNAL(colorSelected()), this, SLOT(onColorSelected()));
  connect(ui->labelText, SIGNAL(editingFinished()), this, SLOT(onLabelChange()));
  connect(ui->latitude, SIGNAL(editingFinished()), this, SLOT(onLatitudeChange()));
  connect(ui->longitude, SIGNAL(editingFinished()), this, SLOT(onLongitudeChange()));
  connect(ui->altitude, SIGNAL(editingFinished()), this, SLOT(onAltitudeChange()));
  connect(this, SIGNAL(accepted()), this, SLOT(onAccept()));//OK button
  connect(this, SIGNAL(rejected()), this, SLOT(onReject()));//Cancel button
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor. Releases allocated resources.
 */
LabelWindow::~LabelWindow()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  delete ui;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This function gets called when the user clicks on the label tool button. It
 * initializes the tool's data and the data displayed in the dialog.
 */
void LabelWindow::initialize()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //initialize label text
  ui->labelText->setText("Label");

  //initialize tool
  LabelTool* labelTool = (LabelTool*)ToolManager::getInstance()->getTool("Label");
  labelTool->setLabelText("Label");
  labelTool->setDummyLabelVisible(true);

  //we emplace the current label right below the camera, so we need
  //to get the camera position, we also populate the position fields
  mPosition = Camera::getInstance()->getGeodeticPosition();
  QString dmsLatitude = Utilities::decimalDegreesToDMS(mPosition.latitude, true);
  ui->latitude->setText(dmsLatitude);
  QString dmsLongitude = Utilities::decimalDegreesToDMS(mPosition.longitude, false);
  ui->longitude->setText(dmsLongitude);
  //remember to clamp position to the ground
  ui->altitude->setText("0.0");
  mPosition.altitude = 0.0f;

  //finally, we set label rendering position by calling the tool
  SimpleVector xyzPosition = Utilities::geodeticToXYZ(mPosition);
  labelTool->setPosition(xyzPosition);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Updates the latitude, longitude and altitude fields in the window. This
 * method gets called back on a mouse pick from the tool object.
 *
 * @param position XYZ position
 */
void LabelWindow::setPosition(const SimpleVector& position)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mPosition = Utilities::xyzToGeodetic(position);

  QString dmsLatitude = Utilities::decimalDegreesToDMS(mPosition.latitude, true);
  ui->latitude->setText(dmsLatitude);

  QString dmsLongitude = Utilities::decimalDegreesToDMS(mPosition.longitude, false);
  ui->longitude->setText(dmsLongitude);

  QString strAltitude = QString::number(mPosition.altitude);
  ui->altitude->setText(strAltitude);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the user changes the label text in the dialog. Sets
 * the corresponding change in the tool.
 */
void LabelWindow::onLabelChange()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QString label = ui->labelText->text();
  LabelTool* labelTool = (LabelTool*)ToolManager::getInstance()->getTool("Label");
  labelTool->setLabelText(label);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the user changes the label color via the Color
 * Select Widget. Sets the corresponding change in the tool object to modify the
 * current rendering color for the label.
 */
void LabelWindow::onColorSelected()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QColor qColor = ui->color->getCurrentColor();
  SimpleColor color;
  color.red = qColor.redF();
  color.green = qColor.greenF();
  color.blue = qColor.blueF();

  LabelTool* labelTool = (LabelTool*)ToolManager::getInstance()->getTool("Label");
  labelTool->setColor(color);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the user changes the latitude in the dialog. Sets
 * the corresponding change in the tool object.
 */
void LabelWindow::onLatitudeChange()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QString latitudeStr = ui->latitude->text();
  mPosition.latitude = Utilities::dmsToDecimalDegrees(latitudeStr);
  SimpleVector xyzPosition = Utilities::geodeticToXYZ(mPosition);

  LabelTool* labelTool = (LabelTool*)ToolManager::getInstance()->getTool("Label");
  labelTool->setPosition(xyzPosition);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the user changes the longitude in the dialog. Sets
 * the corresponding change in the tool object.
 */
void LabelWindow::onLongitudeChange()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QString longitudeStr = ui->longitude->text();
  mPosition.longitude = Utilities::dmsToDecimalDegrees(longitudeStr);
  SimpleVector xyzPosition = Utilities::geodeticToXYZ(mPosition);

  LabelTool* labelTool = (LabelTool*)ToolManager::getInstance()->getTool("Label");
  labelTool->setPosition(xyzPosition);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the user changes the altitude in the dialog. Sets
 * the corresponding change in the tool object.
 */
void LabelWindow::onAltitudeChange()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QString altitudeStr = ui->altitude->text();
  mPosition.altitude = altitudeStr.toFloat();
  SimpleVector xyzPosition = Utilities::geodeticToXYZ(mPosition);

  LabelTool* labelTool = (LabelTool*)ToolManager::getInstance()->getTool("Label");
  labelTool->setPosition(xyzPosition);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the user clicks the OK button. Calls the tool to
 * attempt to add the label; displays warning message if add operation failed
 * e.g. because the label name is already used.
 */
void LabelWindow::onAccept()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  LabelTool* labelTool = (LabelTool*)ToolManager::getInstance()->getTool("Label");

  if (!labelTool->addCurrent())
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
void LabelWindow::onReject()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  closeEvent(NULL);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR Qdialog. Gets called whenever we close this dialog. This mimics
 * the functionality as if the user had clicked (deselect) the path tool button.
 * It performs cleanup like hiding the dummy label.
 *
 * @param event Qt's close event (not used but necessary for override)
 */
void LabelWindow::closeEvent(QCloseEvent*)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  LabelTool* labelTool = (LabelTool*)ToolManager::getInstance()->getTool("Label");

  //hide tool's dummy label
  labelTool->setDummyLabelVisible(false);

  MainWindow* mainWindow = MainWindow::getInstance();
  mainWindow->getToolButton(MainWindow::LABEL_TOOL_BUTTON)->setChecked(false);
  mainWindow->onLabelTool();
}
