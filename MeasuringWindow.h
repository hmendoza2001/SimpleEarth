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

#ifndef MEASURING_WINDOW_H
#define MEASURING_WINDOW_H

#include <QDialog>

namespace Ui
{
  class MeasuringWindow;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the measuring window for the system. Refer to
 * MeasuringWindow.ui for a description of the GUI layout and widget variable
 * names.
 *
 * @version 1.1
 * @author Hector Mendoza
 */
class MeasuringWindow : public QDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Q_OBJECT

  public:
    MeasuringWindow(QWidget *parent = 0);
    ~MeasuringWindow();

    void setMeasurement(float kilometers);
    void closeEvent(QCloseEvent* event);//OVERRIDE

  public slots:
    void onComboBoxChange(int index);

  private:
    Ui::MeasuringWindow *ui;
};

#endif//MEASURING_WINDOW_H
