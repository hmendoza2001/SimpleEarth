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
#include "VolumeWindow.h"
#include "ui_VolumeWindow.h"
#include "MainWindow.h"
#include "WorldObjectManager.h"
#include "VolumeTool.h"
#include "ToolManager.h"
#include "Camera.h"
#include "Utilities.h"
#include "VolumeRenderer.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Calls parent constructor and initializes attributes. Connects
 * signals and slots.
 *
 * @param parent Handle to parent widget
 */
VolumeWindow::VolumeWindow(QWidget *parent) : QDialog(parent), ui(new Ui::VolumeWindow)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  ui->setupUi(this);

  mInEditMode = false;
  mEditWorldObject = NULL;

  //set tracking on sliders on, so that valueChanged signal is triggered
  //while the user is interacting with slider
  ui->rotationXSlider->setTracking(true);
  ui->rotationYSlider->setTracking(true);
  ui->rotationZSlider->setTracking(true);

  connect(ui->typeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTypeChange(int)));
  connect(ui->color, SIGNAL(colorSelected()), this, SLOT(onColorSelected()));
  connect(ui->latitude, SIGNAL(editingFinished()), this, SLOT(onLatitudeChange()));
  connect(ui->longitude, SIGNAL(editingFinished()), this, SLOT(onLongitudeChange()));
  connect(ui->altitude, SIGNAL(editingFinished()), this, SLOT(onAltitudeChange()));
  connect(ui->rotationXSlider, SIGNAL(valueChanged(int)), this, SLOT(onRotationSliderMoved(int)));
  connect(ui->rotationYSlider, SIGNAL(valueChanged(int)), this, SLOT(onRotationSliderMoved(int)));
  connect(ui->rotationZSlider, SIGNAL(valueChanged(int)), this, SLOT(onRotationSliderMoved(int)));
  connect(ui->scaleXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onScaleChange(double)));
  connect(ui->scaleYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onScaleChange(double)));
  connect(ui->scaleZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onScaleChange(double)));
  connect(this, SIGNAL(accepted()), this, SLOT(onAccept()));
  connect(this, SIGNAL(rejected()), this, SLOT(onReject()));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor. Releases allocated resources.
 */
VolumeWindow::~VolumeWindow()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  delete ui;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This function gets called when the user clicks on the volume tool button. It
 * initializes the tool's data and the data displayed in the dialog.
 */
void VolumeWindow::initialize()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //get number of current volumes to append to current name
  WorldObject* worldObject;
  WorldObjectManager* worldObjectManager = WorldObjectManager::getInstance();
  int currentNumberOfVolumes = 0;
  for (int i = 0; i < worldObjectManager->getNumberOfObjects(); i++)
  {
    worldObject = worldObjectManager->getWorldObject(i);
    if (worldObject != NULL && !worldObject->getHasExpired() &&
        worldObject->getGroup() == WorldObject::VOLUME)
    {
      currentNumberOfVolumes++;
    }
  }
  QString volumeName = "Volume" + QString::number(currentNumberOfVolumes+1);
  ui->name->setText(volumeName);

  //get handle to VolumeTool
  VolumeTool* volumeTool = (VolumeTool*)ToolManager::getInstance()->getTool("Volume");

  //we emplace the current volume right below the camera, so we need
  //to get the camera position, we also populate the position fields
  mPosition = Camera::getInstance()->getGeodeticPosition();
  QString dmsLatitude = Utilities::decimalDegreesToDMS(mPosition.latitude, true);
  ui->latitude->setText(dmsLatitude);
  QString dmsLongitude = Utilities::decimalDegreesToDMS(mPosition.longitude, false);
  ui->longitude->setText(dmsLongitude);
  //remember to clamp position to the ground
  ui->altitude->setText("0.0");
  mPosition.altitude = 0.0f;

  //finally, we set volume rendering position
  SimpleVector xyzPosition = Utilities::geodeticToXYZ(mPosition);
  volumeTool->setPosition(xyzPosition);

  //initialize with no rotation
  SimpleVector rotation;
  rotation.x = 0.0f;
  rotation.y = 0.0f;
  rotation.z = 0.0f;
  ui->rotationXSlider->setValue(rotation.x);
  ui->rotationYSlider->setValue(rotation.y);
  ui->rotationZSlider->setValue(rotation.z);
  volumeTool->setRotation(rotation);

  //initialize to scale of 10 Km
  SimpleVector scale;
  scale.x = 1.0f;
  scale.y = 1.0f;
  scale.z = 1.0f;
  ui->scaleXSpinBox->setValue(scale.x);
  ui->scaleYSpinBox->setValue(scale.y);
  ui->scaleZSpinBox->setValue(scale.z);
  volumeTool->setScale(scale);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets up this window in volume edit mode. We fetch all the attributes from
 * the given volume name and populate the data accordingly. We also set the
 * appropriate rendering parameters.
 *
 * @param volumeName Name of the volume we are editing.
 */
void VolumeWindow::setupEdit(const QString& volumeName)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //set flag for cleanup at exit
  mInEditMode = true;

  ui->name->setText(volumeName);
  ui->name->setEnabled(false);//disable name fields
  ui->typeComboBox->setEnabled(false);//disable type field

  //get volume object
  mEditWorldObject = WorldObjectManager::getInstance()->getWorldObject(volumeName);
  if (mEditWorldObject != NULL)
  {
    VolumeRenderer* volumeRenderer = (VolumeRenderer*)mEditWorldObject->getMeshRenderer();

    //populate window fields
    int index = -1;
    if (volumeRenderer->getType() == VolumeRenderer::SPHERE)
    {
      index = 0;
    }
    else if (volumeRenderer->getType() == VolumeRenderer::CUBE)
    {
      index = 1;
    }
    else if (volumeRenderer->getType() == VolumeRenderer::PYRAMID)
    {
      index = 2;
    }
    ui->typeComboBox->setCurrentIndex(index);

    mPosition = mEditWorldObject->getGeodeticPosition();
    QString dmsLatitude = Utilities::decimalDegreesToDMS(mPosition.latitude, true);
    ui->latitude->setText(dmsLatitude);
    QString dmsLongitude = Utilities::decimalDegreesToDMS(mPosition.longitude, false);
    ui->longitude->setText(dmsLongitude);
    ui->altitude->setText(QString::number(mPosition.altitude));
    SimpleVector rotation = mEditWorldObject->getRotation();
    ui->rotationXSlider->setValue(rotation.x);
    ui->rotationYSlider->setValue(rotation.y);
    ui->rotationZSlider->setValue(rotation.z);
    SimpleVector scale = mEditWorldObject->getScale();
    ui->scaleXSpinBox->setValue(scale.x);
    ui->scaleYSpinBox->setValue(scale.y);
    ui->scaleZSpinBox->setValue(scale.z);
    QColor color;
    SimpleColor simpleColor = mEditWorldObject->getColor();
    color.setRedF(simpleColor.red);
    color.setGreenF(simpleColor.green);
    color.setBlueF(simpleColor.blue);
    ui->color->setColor(color);

    //hide real volume and show VolumeTool's copy
    mEditWorldObject->setIsVisible(false);

    //finally copy volume's properties to rendering object
    VolumeTool* volumeTool = (VolumeTool*)ToolManager::getInstance()->getTool("Volume");
    volumeTool->setType(volumeRenderer->getType());
    volumeTool->setColor(simpleColor);
    volumeTool->setPosition(mEditWorldObject->getPosition());
    volumeTool->setRotation(rotation);
    volumeTool->setScale(scale);
  }//end of if (mEditWorldObject != NULL)
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Updates the latitude, longitude and altitude fields in the window. This
 * method gets called back on a mouse pick from the tool.
 *
 * @param position XYZ position
 */
void VolumeWindow::setPosition(const SimpleVector& position)
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
 * Qt SLOT. Gets called when the user selects a different volume type. Sets the
 * corresponding changes in the tool.
 *
 * @param index Current combo box index selected
 */
void VolumeWindow::onTypeChange(int index)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  VolumeTool* volumeTool = (VolumeTool*)ToolManager::getInstance()->getTool("Volume");
  volumeTool->setType(index);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the user changes the volume color via the Color
 * Select Widget. Sets the corresponding change in the tool to modify the
 * current rendering color for the volume.
 */
void VolumeWindow::onColorSelected()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QColor qColor = ui->color->getCurrentColor();
  SimpleColor color;
  color.red = qColor.redF();
  color.green = qColor.greenF();
  color.blue = qColor.blueF();

  VolumeTool* volumeTool = (VolumeTool*)ToolManager::getInstance()->getTool("Volume");
  volumeTool->setColor(color);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the user changes the latitude in the dialog. Sets
 * the corresponding change in the tool.
 */
void VolumeWindow::onLatitudeChange()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QString latitudeStr = ui->latitude->text();
  mPosition.latitude = Utilities::dmsToDecimalDegrees(latitudeStr);
  SimpleVector xyzPosition = Utilities::geodeticToXYZ(mPosition);

  VolumeTool* volumeTool = (VolumeTool*)ToolManager::getInstance()->getTool("Volume");
  volumeTool->setPosition(xyzPosition);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the user changes the longitude in the dialog. Sets
 * the corresponding change in the tool.
 */
void VolumeWindow::onLongitudeChange()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QString longitudeStr = ui->longitude->text();
  mPosition.longitude = Utilities::dmsToDecimalDegrees(longitudeStr);
  SimpleVector xyzPosition = Utilities::geodeticToXYZ(mPosition);

  VolumeTool* volumeTool = (VolumeTool*)ToolManager::getInstance()->getTool("Volume");
  volumeTool->setPosition(xyzPosition);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the user changes the altitude in the dialog. Sets
 * the corresponding change in the tool.
 */
void VolumeWindow::onAltitudeChange()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QString altitudeStr = ui->altitude->text();
  mPosition.altitude = altitudeStr.toFloat();
  SimpleVector xyzPosition = Utilities::geodeticToXYZ(mPosition);

  VolumeTool* volumeTool = (VolumeTool*)ToolManager::getInstance()->getTool("Volume");
  volumeTool->setPosition(xyzPosition);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when either the X, Y or Z sliders for rotation are
 * moved. Sets the corresponding changes in the tool.
 *
 * @param value Current slider value (not used but necessary for override)
 */
void VolumeWindow::onRotationSliderMoved(int)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  SimpleVector rotation;
  rotation.x = ui->rotationXSlider->value();
  rotation.y = ui->rotationYSlider->value();
  rotation.z = ui->rotationZSlider->value();

  VolumeTool* volumeTool = (VolumeTool*)ToolManager::getInstance()->getTool("Volume");
  volumeTool->setRotation(rotation);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when either the X, Y or Z sliders for scaling are moved.
 * Sets the corresponding changes in the tool.
 *
 * @param value Current slider value (not used but necessary for override)
 */
void VolumeWindow::onScaleChange(double)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  SimpleVector scale;
  scale.x = ui->scaleXSpinBox->value();
  scale.y = ui->scaleYSpinBox->value();
  scale.z = ui->scaleZSpinBox->value();

  VolumeTool* volumeTool = (VolumeTool*)ToolManager::getInstance()->getTool("Volume");
  volumeTool->setScale(scale);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the user clicks the OK button. If in edit mode,
 * call aprorpiate method to finalize volume editing. Otherwise, it calls the
 * volume tool to attempt to add a volume, displays warning message if volume
 * name is already used.
 */
void VolumeWindow::onAccept()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  VolumeTool* volumeTool = (VolumeTool*)ToolManager::getInstance()->getTool("Volume");
  if (mInEditMode)
  {
    if (mEditWorldObject != NULL)
    {
      volumeTool->editCurrent(mEditWorldObject);
    }
    closeEvent(NULL);
  }
  else
  {
    //set current name
    volumeTool->setName(ui->name->text());

    if (!volumeTool->addCurrent())
    {
      QMessageBox::warning(this, "Warning", "Name already used. Please choose another name.");
      this->show();
    }
    else
    {
      closeEvent(NULL);
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the user clicks the Cancel button. Calls the
 * closeEvent function to cleanup after itself.
 */
void VolumeWindow::onReject()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  closeEvent(NULL);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR Qdialog. Gets callled whenever we close this dialog. This
 * mimics the functionality as if the user had clicked (deselect) the volume
 * tool button and performs some cleanup if we were in edit mode.
 *
 * @param event Qt's close event (not used but necessary for override)
 */
void VolumeWindow::closeEvent(QCloseEvent*)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  MainWindow* mainWindow = MainWindow::getInstance();
  mainWindow->getToolButton(MainWindow::VOLUME_TOOL_BUTTON)->setChecked(false);
  mainWindow->onVolumeTool();

  //cleanup if we were in edit mode
  if (mInEditMode)
  {
    mInEditMode = false;
    ui->name->setEnabled(true);//enable name field
    ui->typeComboBox->setEnabled(true);//enable type field

    if (mEditWorldObject != NULL)
    {
      mEditWorldObject->setIsVisible(true);//set true volume visible again
    }
  }
}
