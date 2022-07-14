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

#include <QtOpenGL>
#include "ModelRenderer.h"
#include "IconModelManager.h"
#include "WorldObject.h"

#ifdef USING_ASSIMP
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Initializes attributes and calls parent constructor.
 */
ModelRenderer::ModelRenderer(const QString& filePath) : MeshRenderer()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mFilePath = filePath;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
ModelRenderer::~ModelRenderer()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR MeshRenderer. Sets appropriate transformations and calls
 * recursiveRender to render Assimp scene.
 */
void ModelRenderer::render()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  glPushMatrix();

  //first rotate and translate coordinate system to object
  //coordinate system
  SimpleVector position = mWorldObject->getPosition();
  GeodeticPosition geoPosition = mWorldObject->getGeodeticPosition();
  glRotatef(geoPosition.longitude - 180.0f, 0.0f, 0.0f, 1.0f);
  glRotatef(-1.0f * (90.0f - geoPosition.latitude), 0.0f, 1.0f, 0.0f);
  float radius = sqrt(position.x*position.x + position.y*position.y + position.z*position.z);
  glTranslatef(0.0f, 0.0f, radius);

  //rotate around X, Y and Z axis, note rotations
  //are counterclockwise in OpenGL, that is why we
  //give the negative rotation values
  SimpleVector rotation = mWorldObject->getRotation();
  glRotatef(-rotation.x, 1.0f, 0.0f, 0.0f);
  glRotatef(-rotation.y, 0.0f, 1.0f, 0.0f);
  glRotatef(-rotation.z, 0.0f, 0.0f, 1.0f);

  //set scale
  SimpleVector scale = mWorldObject->getScale();
  glScalef(scale.x, scale.y, scale.z);

  void* modelData = NULL;
  int texture = -1;

  bool modelLoaded = IconModelManager::getInstance()->getModel(mFilePath, modelData, texture);
  if (modelLoaded)
  {
#ifdef USING_ASSIMP
    //cast model data to assimp's scene struct in order to access root node
    struct aiScene* scene = (struct aiScene*)modelData;
    recursiveRender((void*)scene, (void*)scene->mRootNode, texture);
#endif
  }

  glPopMatrix();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the file path if the model associated with this renderer.
 *
 * @return The file path if the model associated with this renderer
 */
const QString& ModelRenderer::getFilePath() const
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mFilePath;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the file path of the model associated with this renderer.
 *
 * @param filePath File path to model
 */
void ModelRenderer::setFilePath(const QString& filePath)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mFilePath = filePath;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This function is a slight modification of the recursive_render function in
 * Assimp's SimpleOpenGL example. It renders the given scene recursively.
 *
 * @param scene Handle to scene (root node)
 * @param node Handle to node
 * @param texture Texture index
 */
void ModelRenderer::recursiveRender(void* scene, void* node, int texture)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
#ifdef USING_ASSIMP
  struct aiScene* sc = (struct aiScene*)scene;
  struct aiNode* nd = (struct aiNode*)node;

  unsigned int i;
  unsigned int n = 0, t;
  aiMatrix4x4 m = nd->mTransformation;

  //update transform
  aiTransposeMatrix4(&m);
  glPushMatrix();
  glMultMatrixf((float*)&m);

  //draw all meshes assigned to this node
  for (n = 0; n < nd->mNumMeshes; ++n)
  {
    const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];

    for (t = 0; t < mesh->mNumFaces; ++t)
    {
      const struct aiFace* face = &mesh->mFaces[t];
      GLenum face_mode;

      switch(face->mNumIndices)
      {
        case 1: face_mode = GL_POINTS; break;
        case 2: face_mode = GL_LINES; break;
        case 3: face_mode = GL_TRIANGLES; break;
        default: face_mode = GL_POLYGON; break;
      }

      //bind texture
      if (texture >= 0)
      {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
      }

      glBegin(face_mode);
      for (i = 0; i < face->mNumIndices; i++)
      {
        int index = face->mIndices[i];

        if (texture >= 0)
        {
          if (mesh->HasTextureCoords(0))
          {
            const aiVector3D* pTexCoord = &(mesh->mTextureCoords[0][index]);
            glTexCoord2f(pTexCoord->x, pTexCoord->y);
          }
        }

        glVertex3fv(&mesh->mVertices[index].x);
      }
      glEnd();

      if (texture >= 0)
      {
        glDisable(GL_TEXTURE_2D);
      }
    }
  }

  //draw children nodes
  for (n = 0; n < nd->mNumChildren; ++n)
  {
    recursiveRender(sc, nd->mChildren[n], texture);
  }

  glPopMatrix();
#endif
}
