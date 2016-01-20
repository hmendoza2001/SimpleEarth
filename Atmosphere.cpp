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
#include "Atmosphere.h"
#include "Camera.h"
#include "Constants.h"
#include "Utilities.h"

static Camera* camera = NULL;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Initializes attributes.
 */
Atmosphere::Atmosphere()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  camera = Camera::getInstance();
  mTextureHandle = 0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
Atmosphere::~Atmosphere()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the atmosphere texture.
 *
 * @param handle OpenGL handle to texture
 */
void Atmosphere::setTexture(unsigned int handle)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mTextureHandle = handle;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Builds the geometry and renders the atmosphere. The way we render the
 * atmosphere could not be more simple. It is just a semi-transparent texture
 * that gets drawn in front of the Earth and that provides the visual effect of
 * atmospheric scattering.
 */
void Atmosphere::render()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  glPushMatrix();

  GeodeticPosition cameraPosition = camera->getGeodeticPosition();
  SimpleVector cameraXYZPosition = Utilities::geodeticToXYZ(cameraPosition);
  float cameraRadius = cameraPosition.altitude + Constants::EARTH_MEAN_RADIUS;

  //calculate image position so that it is always in front of the earth
  float x = (cameraXYZPosition.x/cameraRadius) * 7000.0f;
  float y = (cameraXYZPosition.y/cameraRadius) * 7000.0f;
  float z = (cameraXYZPosition.z/cameraRadius) * 7000.0f;
  glTranslatef(x, y, z);

  //rotate image so that it always faces the camera
  glRotatef(cameraPosition.longitude, 0.0f, 0.0f, 1.0f);
  glRotatef(-cameraPosition.latitude, 0.0f, 1.0f, 0.0f);

  //calculate image radius based on camera distance from
  //center of the earth (camera radius)
  float theta = atan(Constants::EARTH_MEAN_RADIUS/cameraRadius);
  float adjacent = cameraRadius - Constants::EARTH_MEAN_RADIUS + 330.0f;
  float textureSize = adjacent * tan(theta);//texture size is equal to the opposite side in a right triangle
  textureSize *= 1.21f;//increment a little to account for image size

  //enable texture transparency
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_TEXTURE_2D);

  //get texture object id
  glBindTexture(GL_TEXTURE_2D, mTextureHandle);

  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(0.0f, -textureSize, -textureSize);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(0.0f, textureSize, -textureSize);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(0.0f, textureSize, textureSize);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(0.0f, -textureSize, textureSize);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  glPopMatrix();
}
