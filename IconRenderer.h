/*
 *  The Simple Earth Project
 *  Copyright (C) 2016 HueSoft LLC
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

#ifndef ICON_RENDERER_H
#define ICON_RENDERER_H

#include <QImage>
#include "globals.h"

class WorldObject;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the functionality to render an icon (wich is just
 * a 2D image representation of an object).
 *
 * @version 1.1
 * @author Hector Mendoza
 */
class IconRenderer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    IconRenderer(const QString& filePath = "");
    ~IconRenderer();

    void render();

    //get methods
    WorldObject* getWorldObject();
    QString getFilePath();
    int getTexture();
    float getDepth();

    //set methods
    void setWorldObject(WorldObject* worldObject);
    void setFilePath(const QString& filePath);
    void setTexture(int texture);
    void setDepth(float depth);

  private:
    WorldObject* mWorldObject;
    QString mFilePath;
    int mTexture;
    float mDepth;
};

#endif//ICON_RENDERER_H
