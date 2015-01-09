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

#ifndef PATH_RENDERER_H
#define PATH_RENDERER_H

#include "MeshRenderer.h"
#include "globals.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the functionality to render a path. A path is a line
 * that connects multiple segments. This class inherits from MeshRenderer. Even
 * though a path is not really a "mesh", this is done for orthogonality
 * purposes.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class PathRenderer : public MeshRenderer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    PathRenderer();
    ~PathRenderer();

    void render();//OVERRIDE
    void addPoint(SimpleVector point);
    QList<SimpleVector>* getPoints();

  private:
    QList<SimpleVector> mPoints;
};

#endif//PATH_RENDERER_H
