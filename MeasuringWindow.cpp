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

#include "MeasuringWindow.h"
#include "ui_MeasuringWindow.h"
#include "MainWindow.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Connects signals and slots.
 *
 * @param parent Handle to parent widget
 */
MeasuringWindow::MeasuringWindow(QWidget *parent) : QDialog(parent), ui(new Ui::MeasuringWindow)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  ui->setupUi(this);

  connect(ui->unitsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxChange(int)));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
MeasuringWindow::~MeasuringWindow()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  delete ui;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the measurement label.
 *
 * @param kilometers Measurement distance in kilometers
 */
void MeasuringWindow::setMeasurement(float kilometers)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //check if combo box is in kilometers or miles
  if (ui->unitsComboBox->currentIndex() == 0)
  {
    ui->lengthLabel->setText(QString::number(kilometers, 'f', 2));
  }
  else
  {
    //convert value to miles first
    float miles = kilometers / 1.6093f;
    ui->lengthLabel->setText(QString::number(miles, 'f', 2));
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Converts the current measurement label to the new units selected.
 *
 * @param index Current combo box index selected
 */
void MeasuringWindow::onComboBoxChange(int index)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (index == 0)
  {
    float kilometers = ui->lengthLabel->text().toFloat() * 1.6093f;
    ui->lengthLabel->setText(QString::number(kilometers, 'f', 2));
  }
  else
  {
    float miles = ui->lengthLabel->text().toFloat() / 1.6093f;
    ui->lengthLabel->setText(QString::number(miles, 'f', 2));
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR Qdialog. Gets callled whenever we close this dialog. This
 * mimics the functionality as if the user had pressed the measuring tool
 * button.
 *
 * @param event Qt's close event (not used but necessary for override)
 */
void MeasuringWindow::closeEvent(QCloseEvent*)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  MainWindow* mainWindow = MainWindow::getInstance();
  mainWindow->getToolButton(MainWindow::MEASURING_TOOL_BUTTON)->setChecked(false);
  mainWindow->onMeasuringTool();
}
