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

#ifndef COLOR_SELECT_WIDGET_H
#define COLOR_SELECT_WIDGET_H

#include <QLineEdit>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the functionality behind a color selection widget.
 * This widget is a read-only LineEdit that brings up a ColorDialog when
 * double clicked and emits a signal once a color has been selected;
 *
 * @version 1.1
 * @author Hector Mendoza
 */
class ColorSelectWidget : public QLineEdit
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Q_OBJECT

  public:
    ColorSelectWidget(QWidget *parent = 0);
    ~ColorSelectWidget();

    const QColor& getCurrentColor() const;
    void setColor(const QColor& color);
    void mouseDoubleClickEvent(QMouseEvent* e);//OVERRIDE

  signals:
    void colorSelected();

  private:
    QColor mColor;
};

#endif//COLOR_SELECT_WIDGET_H
