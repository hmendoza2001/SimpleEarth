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

#ifndef VOLUME_WINDOW_H
#define VOLUME_WINDOW_H

#include <QDialog>
#include <QString>

#include "WorldObject.h"
#include "globals.h"

namespace Ui
{
  class VolumeWindow;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the functionality behind the Volume Tool window.
 * Refer to VolumeWindow.ui for a description of the GUI layout and widget
 * variable names.
 *
 * @version 1.1
 * @author Hector Mendoza
 */
class VolumeWindow : public QDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Q_OBJECT

  public:
    VolumeWindow(QWidget *parent = 0);
    ~VolumeWindow();

    void initialize();
    void setupEdit(const QString& volumeName);
    void setPosition(const SimpleVector& position);
    void closeEvent(QCloseEvent* event);//OVERRIDE
  
  public slots:
    void onTypeChange(int index);
    void onColorSelected();
    void onLatitudeChange();
    void onLongitudeChange();
    void onAltitudeChange();
    void onRotationSliderMoved(int);
    void onScaleChange(double);
    void onAccept();
    void onReject();

  private:
    Ui::VolumeWindow *ui;
    GeodeticPosition mPosition;
    bool mInEditMode;
    WorldObject* mEditWorldObject;
};

#endif//VOLUME_WINDOW_H
