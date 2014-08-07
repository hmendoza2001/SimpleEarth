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

#ifndef SHAPE_RENDERER_H
#define SHAPE_RENDERER_H

#include "MeshRenderer.h"
#include "globals.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the functionality to render 2D and 3D shapes. A shape
 * is basically a set of points (ground points) that are similar in nature to
 * those defined in an ESRI shapefile, but with possible associated height data
 * (height points). If the shape does have height data associated with it, the
 * 3D volume is achieved by extruding the original shape.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class ShapeRenderer : public MeshRenderer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    ShapeRenderer();
    ~ShapeRenderer();

    bool getHasHeight();
    void setHasHeight(bool value);
    void addGroundPoint(SimpleVector point);
    void addHeightPoint(SimpleVector point);

    void render();//OVERRIDE

  private:
    void createVertices();

    bool mHasHeight;
    int mDisplayListIndex;
    QList<SimpleVector> mGroundPointList;
    QList<SimpleVector> mHeightPointList;
};

#endif//SHAPE_RENDERER_H
