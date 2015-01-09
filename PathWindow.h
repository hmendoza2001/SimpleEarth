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

#ifndef PATH_WINDOW_H
#define PATH_WINDOW_H

#include <QDialog>

namespace Ui
{
  class PathWindow;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the Path Tool window for the system. Refer to
 * PathWindow.ui for a description of the GUI layout and widget variable
 * names.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class PathWindow : public QDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Q_OBJECT

  public:
    PathWindow(QWidget *parent = 0);
    ~PathWindow();

    void initialize();
    void closeEvent(QCloseEvent* event);//OVERRIDE

  public slots:
    void onColorSelected();
    void onAccept();
    void onReject();

  private:
    Ui::PathWindow *ui;
};

#endif//PATH_WINDOW_H
