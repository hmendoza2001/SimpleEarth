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

#ifndef LABEL_WINDOW_H
#define LABEL_WINDOW_H

#include <QDialog>

#include "globals.h"

namespace Ui
{
  class LabelWindow;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the labels window for the system which allows the
 * user to emplace a label in the globe. Refer to LabelWindow.ui for a
 * description of the GUI layout and widget variable names.
 *
 * @version 1.1
 * @author Hector Mendoza
 */
class LabelWindow : public QDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Q_OBJECT

  public:
    LabelWindow(QWidget *parent = 0);
    ~LabelWindow();

    void initialize();
    void setPosition(const SimpleVector& position);
    void closeEvent(QCloseEvent* event);//OVERRIDE

  public slots:
    void onLabelChange();
    void onColorSelected();
    void onLatitudeChange();
    void onLongitudeChange();
    void onAltitudeChange();
    void onAccept();
    void onReject();

  private:
    Ui::LabelWindow *ui;
    GeodeticPosition mPosition;
};

#endif//LABEL_WINDOW_H
