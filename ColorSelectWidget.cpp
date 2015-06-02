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

#include <QColorDialog>
#include "ColorSelectWidget.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Initializes attributes.
 */
ColorSelectWidget::ColorSelectWidget(QWidget *parent) : QLineEdit(parent)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mColor.setRedF(1.0f);
  mColor.setGreenF(1.0f);
  mColor.setBlueF(1.0f);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
ColorSelectWidget::~ColorSelectWidget()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the currently selected color.
 *
 * @return Currently selected color
 */
const QColor& ColorSelectWidget::getCurrentColor() const
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mColor;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the current color for this widget.
 *
 * @param color New color
 */
void ColorSelectWidget::setColor(const QColor& color)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //set current color
  mColor = color;

  //change widget background
  QPalette palette = this->palette();
  palette.setColor(QPalette::Base, mColor);
  this->setPalette(palette);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR QWidget. Gets called when this widget is double clicked. Emits
 * signal when a color has been selected.
 *
 * @param e Qt's mouse event handle (not used but necessary for override)
 */
void ColorSelectWidget::mouseDoubleClickEvent(QMouseEvent*)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QColor color;

  //display Qt color selection widget
  color = QColorDialog::getColor(mColor);

  //change color background only if the user actually
  //selected a color (as opposed to clicking cancel)
  if (color.isValid())
  {
    mColor = color;
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, mColor);
    this->setPalette(palette);

    emit colorSelected();
  }
}
