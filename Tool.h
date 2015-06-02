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

#ifndef TOOL_H
#define TOOL_H

#include <QString>
#include <QToolButton>
#include <QDialog>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Abstract class for Tool objects. A tool can be described as an object whose
 * functionality is related to a tool button on the main window. A tool object
 * contains handles to its corresponding tool button and dialog. Tool objects
 * are managed by the Tool Manager (refer to ToolManager.h).
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class Tool
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    Tool(const QString& toolID = "", QToolButton* toolButton = 0, QDialog* dialog = 0);
    virtual ~Tool();

    virtual void render() = 0;//forced override

    //get methods
    const QString& getToolID() const;
    QToolButton* getToolButton();
    QDialog* getDialog();

    //set methods
    void setToolID(const QString& toolID);
    void setToolButton(QToolButton* toolButton);
    void setDialog(QDialog* dialog);

  protected:
    QString mToolID;
    QToolButton* mToolButton;
    QDialog* mDialog;
};

#endif//TOOL_H
