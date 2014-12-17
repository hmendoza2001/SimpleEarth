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

#ifndef MODEL_RENDERER_H
#define MODEL_RENDERER_H

#include <QString>
#include "MeshRenderer.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the functionality to render a model whose data was
 * previously loaded by the Assimp library (assimp.sourceforge.net). It uses
 * the Assimp library to fetch vertex data. This class inherits from
 * MeshRenderer.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class ModelRenderer : public MeshRenderer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    ModelRenderer(const QString& filePath = "");
    ~ModelRenderer();

    void render();//OVERRIDE

    const QString& getFilePath() const;
    void setFilePath(const QString& filePath);

  private:
    void recursiveRender(void* scene, void* node, int texture);
    QString mFilePath;
};

#endif//MODEL_RENDERER_H
