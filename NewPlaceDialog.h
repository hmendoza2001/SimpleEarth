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

#ifndef NEW_PLACE_DIALOG_H
#define NEW_PLACE_DIALOG_H

#include <QDialog>
#include <QString>

namespace Ui
{
  class NewPlaceDialog;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the functionality behind the "Add Place" dialog which
 * adds a teleport place to the list. To see the dialog's layout and contents
 * please refer to NewPlaceDialog.ui. This is a simple dialog that just asks the
 * user to name the current camera position.
 *
 * @version 1.1
 * @author Hector Mendoza
 */
class NewPlaceDialog : public QDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Q_OBJECT

  public:
    NewPlaceDialog(QWidget *parent = 0);
    ~NewPlaceDialog();

    QString getNewPlaceName();

  private:
    Ui::NewPlaceDialog *ui;
};

#endif//NEW_PLACE_DIALOG_H
