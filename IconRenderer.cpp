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

#include <QtOpenGL>
#include "IconRenderer.h"
#include "Camera.h"
#include "Utilities.h"
#include "IconModelManager.h"
#include "WorldObject.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Initializes attributes.
 *
 * @param filePath Path to the icon file
 */
IconRenderer::IconRenderer(const QString& filePath)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mFilePath = filePath;
  mWorldObject = NULL;
  mTexture = -1;
  mDepth = 0.0f;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
IconRenderer::~IconRenderer()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the world object associated with this Icon Renderer.
 *
 * @return Handle to world object associated with this IconRederer
 */
WorldObject* IconRenderer::getWorldObject()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mWorldObject;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the file path of the icon associated with this renderer.
 *
 * @return The file path of the icon associated with this renderer
 */
QString IconRenderer::getFilePath()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mFilePath;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the OpenGL texture handle for the icon.
 *
 * @return OpenGL texture handle
 */
int IconRenderer::getTexture()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mTexture;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the depth of the icon. Depth is used to determine which icons get
 * rendered on top of others. By default, depth is determine by the order in
 * which icons are loaded.
 *
 * @return Current icon depth
 */
float IconRenderer::getDepth()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mDepth;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the handle to the world object associated with this renderer.
 *
 * @param worldObject Handle to world object
 */
void IconRenderer::setWorldObject(WorldObject* worldObject)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mWorldObject = worldObject;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the file path of the icon associated with this renderer.
 *
 * @param filePath File path to icon
 */
void IconRenderer::setFilePath(const QString& filePath)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mFilePath = filePath;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * WARNING: This method is provided for convenience. However the IconModelManager
 * is automatically invoked inside the render method to make sure an icon only
 * gets loaded once. Sets OpenGL handle to icon texture.
 *
 * @param index OpenGL handle to texture
 */
void IconRenderer::setTexture(int texture)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mTexture = texture;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the current depth of the icon. Depth is used to determine which icons get
 * rendered on top of others. By default, depth is determine by the order in
 * which icons are loaded.
 *
 * @param depth The value for the icon depth
 */
void IconRenderer::setDepth(float depth)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mDepth = depth;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Builds the geometry to render an icon. Also computes the location of the
 * icon on the screen.
 */
void IconRenderer::render()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //get texture and depth from current file path
  IconModelManager::getInstance()->getIcon(mFilePath, mTexture, mDepth);

  if (mTexture >= 0)
  {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    //get current screen size and set ortho
    ScreenCoordinates screenSize = Camera::getInstance()->getScreenSize();
    glOrtho(0, screenSize.x, 0, screenSize.y, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    SimpleVector screenLocation = mWorldObject->getScreenLocation();

    //only render if world object is not being obscured by the earth
    //and if projection is not negative (behind the camera, which means screenZ
    //is below 1.0)
    if (!Utilities::checkObscure(Camera::getInstance()->getGeodeticPosition(),
        mWorldObject->getGeodeticPosition()) && screenLocation.z < 1.0)
    {
      //enable png transparency
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, mTexture);

      glBegin(GL_QUADS);

      float iconSize = 40.0f;

      glTexCoord3f(0.0f, 0.0f, mDepth);
      glVertex3f(screenLocation.x - (iconSize/2.0f), screenLocation.y - (iconSize/2.0f), mDepth);

      glTexCoord3f(1.0f, 0.0f, mDepth);
      glVertex3f(screenLocation.x + (iconSize/2.0f), screenLocation.y - (iconSize/2.0f), mDepth);

      glTexCoord3f(1.0f, 1.0f, mDepth);
      glVertex3f(screenLocation.x + (iconSize/2.0f), screenLocation.y + (iconSize/2.0f), mDepth);

      glTexCoord3f(0.0f, 1.0f, mDepth);
      glVertex3f(screenLocation.x - (iconSize/2.0f), screenLocation.y + (iconSize/2.0f), mDepth);

      glEnd();//GL_QUADS

      glDisable(GL_TEXTURE_2D);
      glDisable(GL_BLEND);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  }
}
