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

#ifndef MEASURING_TOOL_H
#define MEASURING_TOOL_H

#include <QMouseEvent>

#include "Tool.h"
#include "globals.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the rendering and math functionality behind the
 * measuring tool. The measured distance is computed with the great circle
 * distance equation.
 *
 * @version 1.1
 * @author Hector Mendoza
 */
class MeasuringTool : public Tool
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    MeasuringTool(QToolButton* toolButton = 0, QDialog* dialog = 0);
    ~MeasuringTool();

    void render();//OVERRIDE

    void onMousePressEvent(QMouseEvent* event);
    void onMouseReleaseEvent(QMouseEvent* event);
    void onMouseMoveEvent(QMouseEvent* event);

  private:
    SimpleVector mStartPoint;
    SimpleVector mEndPoint;
    ScreenCoordinates mStartScreenCordinates;
    ScreenCoordinates mEndScreenCordinates;
    bool mCalculateStartPoint;
    bool mCalculateEndPoint;
};

#endif//MEASURING_TOOL_H
