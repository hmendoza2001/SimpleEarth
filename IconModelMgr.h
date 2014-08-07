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

#ifndef ICON_MODEL_MGR_H
#define ICON_MODEL_MGR_H

#include <QString>
#include <QList>
#include <QImage>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class handles the loading of images and models to avoid duplicity.
 * It loads images onto graphics card and keeps track of texture indices. It
 * also loads model data by using the Assimp library (assimp.sourceforge.net).
 * World Objetcs call loadIcon and loadModel during initialization time, then
 * they call getIcon and getModel during render time. This class is implemented
 * as a Singleton.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class IconModelMgr
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    struct Icon
    {
      QString filePath;
      QImage* image;
      int texture;
      float depth;
    };

    struct Model
    {
      QString filePath;
      void* modelData;
      QImage* image;
      int texture;
    };

    static IconModelMgr* getInstance();
    ~IconModelMgr();

    bool loadIcon(QString filePath);
    bool loadModel(QString filePath);
    bool getIcon(QString filePath, int& texture, float& depth);
    bool getModel(QString filePath, void*& modelData, int& texture);

  private:
    IconModelMgr();//private due to Singleton implementation

    static IconModelMgr* mInstance;
    QList<Icon> mIconList;
    QList<Model> mModelList;
};

#endif//ICON_MODEL_MGR_H
