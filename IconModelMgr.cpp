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

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include "IconModelMgr.h"
#include "Utilities.h"

#ifdef USING_ASSIMP
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif

IconModelMgr* IconModelMgr::mInstance = NULL;//Singleton implementation

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor.
 */
IconModelMgr::IconModelMgr()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor. Releases model data if necessary.
 */
IconModelMgr::~IconModelMgr()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mInstance = NULL;
  int i = 0;

  //release icon/image data
  for (i = 0; i < mIconList.size(); i++)
  {
    delete mIconList[i].image;
  }
  mIconList.clear();

#ifdef USING_ASSIMP
  //release model data
  for (i = 0; i < mModelList.size(); i++)
  {
    const struct aiScene* scene = (const struct aiScene*)mModelList[i].modelData;
    aiReleaseImport(scene);
  }
  mModelList.clear();
#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Singleton pattern implementation. Returns the single instance of this class.
 *
 * @return The single instance of this class
 */
IconModelMgr* IconModelMgr::getInstance()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mInstance == NULL)
  {
    mInstance = new IconModelMgr();
  }

  return mInstance;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Checks if icon has been loaded already, loads icon into local database if not.
 *
 * @param filePath Path to icon/image file
 * @return True if image was loaded successfully
 */
bool IconModelMgr::loadIcon(const QString& filePath)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  bool returnValue = false;
  bool iconAlreadyLoaded = false;

  //check if image was already loaded
  for (int i = 0; i < mIconList.size(); i++)
  {
    if (mIconList[i].filePath == filePath)
    {
      iconAlreadyLoaded = true;
      returnValue = true;
      break;
    }
  }

  if (!iconAlreadyLoaded)
  {
    //load image
    QImage* image = new QImage();
    if (image->load(filePath))
    {
      //save icon in local database
      Icon icon;
      icon.filePath = filePath;
      icon.image = image;
      icon.texture = -1;
      icon.depth = (float)mIconList.size()/1000.0f;
      mIconList.append(icon);
      returnValue = true;
    }
    else
    {
      printf("IconModelMgr.cpp: Error loading image file.\n");
      delete image;
    }
  }

  return returnValue;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Checks if model has been loaded already, loads model into local database if
 * not. This method uses the Assimp library to load model (assimp.sourceforge.net).
 *
 * @param filePath Path to model file
 * @return True if model was loaded successfully
 */
bool IconModelMgr::loadModel(const QString& filePath)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  bool returnValue = false;
  bool modelAlreadyLoaded = false;

  //check if model was already loaded
  for (int i = 0; i < mModelList.size(); i++)
  {
    if (mModelList[i].filePath == filePath)
    {
      modelAlreadyLoaded = true;
      returnValue = true;
      break;
    }
  }

  if (!modelAlreadyLoaded)
  {
    //load model
#ifdef USING_ASSIMP
    const struct aiScene* scene = NULL;
    scene = aiImportFile(filePath.toStdString().c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
    if (scene != NULL)
    {
      returnValue = true;

      //save model in local database
      Model model;
      model.filePath = filePath;
      model.modelData = (void*)scene;
      model.texture = -1;

      //load texture as images first
      for (unsigned int materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++)
      {
        const aiMaterial* pMaterial = scene->mMaterials[materialIndex];
        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
          aiString relativePath;
          if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &relativePath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
          {
            //get full path from model file path
            QStringList stringList = model.filePath.split("/");

            QString directoryPath = "";
            //to get directory path we just ignore the last element in string list
            for (int stringListIndex = 0; stringListIndex < stringList.length()-1; stringListIndex++)
            {
              directoryPath += stringList[stringListIndex] + "/";
            }
            QString fullPath = directoryPath + relativePath.data;

            QImage* image = new QImage();
            if (!image->load(fullPath))
            {
              printf("IconModelMgr.cpp: Error loading texture for model.\n");
              delete image;
            }
            model.image = image;
          }
        }
      }

      mModelList.append(model);
    }//end of if (scene != NULL)
    else
    {
      printf("IconModelMgr.cpp: Error loading model data.\n");
    }
#endif
  }// end of if (!modelAlreadyLoaded)

  return returnValue;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * WARNING: This method is supposed to get called from within OpenGL context.
 * It returns the OpenGL texture index and texture depth for the given icon file.
 *
 * @param filePath Path to icon/image file
 * @param texture Texture index value returned
 * @param depth Depth value returned
 * @return True if texture was found
 */
bool IconModelMgr::getIcon(const QString& filePath, int& texture, float& depth)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  int returnValue = false;

  //find icon
  for (int i = 0; i < mIconList.size(); i++)
  {
    if (mIconList[i].filePath == filePath)
    {
      //load texture to graphics card if not done so already
      if (mIconList[i].texture == -1)
      {
        mIconList[i].texture = Utilities::imageToTexture(*mIconList[i].image);
      }
      texture = mIconList[i].texture;
      depth = mIconList[i].depth;
      returnValue = true;
      break;
    }
  }

  return returnValue;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * WARNING: This method is supposed to get called from within OpenGL context.
 * It returns the model data (previosuly loaded with Assimp) and OpenGL texture
 * index for the given model file.
 *
 * @param filePath Path to model file
 * @param modelData Model data returned (actual type is aiScene* but cast as void to ease dependency)
 * @param texture Texture index value returned
 * @return True if model was found
 */
bool IconModelMgr::getModel(const QString& filePath, void*& modelData, int& texture)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  int returnValue = false;

  //find model
  for (int i = 0; i < mModelList.size(); i++)
  {
    if (mModelList[i].filePath == filePath)
    {
      //load textures to graphics card if not done so already
      if (mModelList[i].texture == -1)
      {
        mModelList[i].texture = Utilities::imageToTexture(*mModelList[i].image);
      }
      texture = mModelList[i].texture;
      modelData = mModelList[i].modelData;
      returnValue = true;
      break;
    }
  }

  return returnValue;
}
