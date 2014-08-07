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

#ifndef SHAPE_FILE_READER_H
#define SHAPE_FILE_READER_H

#include <QString>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the functionality to read ESRI Shapefiles. This
 * class determines if there is associated altitude/height data related to the
 * shape by attemtping to find a .csv file of the same name. If altitude/height
 * data exists, the shape becomes 3 dimensional (e.g. buildings).
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class ShapefileReader
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    ShapefileReader();
    ~ShapefileReader();

    void setMinimumHeight(float value);
    float getMinimumHeight();
    void readFile(QString fileName);

  private:
    bool mHasHeightData;
    int mNumberOfReadShapes;
    float mMinimumHeight;
};

#endif//SHAPE_FILE_READER_H
