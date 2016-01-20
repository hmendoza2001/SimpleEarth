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
#include <GL/glu.h>
#include "VolumeRenderer.h"
#include "globals.h"
#include "WorldObject.h"

static GLUquadricObj* sphericalQuadricObject = NULL;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Initializes attributes and calls parent's constructor.
 */
VolumeRenderer::VolumeRenderer() : MeshRenderer()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  sphericalQuadricObject = gluNewQuadric();
  mVolumeType = SPHERE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
VolumeRenderer::~VolumeRenderer()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Renders the appropriate volume depending on the current volume type.
 */
void VolumeRenderer::render()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  glPushMatrix();

  //set color
  SimpleColor color = mWorldObject->getColor();
  glColor4f(color.red, color.green, color.blue, 0.5f);

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

  //draw outlined geometry
  glPolygonMode(GL_BACK, GL_LINE);
  glPolygonMode(GL_FRONT, GL_LINE);

  if (mVolumeType == SPHERE)
  {
    renderSphere();
  }
  else if (mVolumeType == CUBE)
  {
    renderCube();
  }
  else if (mVolumeType == PYRAMID)
  {
    renderPyramid();
  }

  glPolygonMode(GL_BACK, GL_FILL);
  glPolygonMode(GL_FRONT, GL_FILL);

  //draw semi transparent geometry
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if (mVolumeType == SPHERE)
  {
    renderSphere();
  }
  else if (mVolumeType == CUBE)
  {
    renderCube();
  }
  else if (mVolumeType == PYRAMID)
  {
    renderPyramid();
  }

  glDisable(GL_BLEND);
  glPopMatrix();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the current volume type. Please refer to volyme types enum in header
 * file to see the possible values.
 *
 * @return Current volume type
 */
int VolumeRenderer::getType()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mVolumeType;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the current volume type. Please refer to volyme types enum in header
 * file to see the possible values.
 *
 * @param type New volume type
 */
void VolumeRenderer::setType(int type)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mVolumeType = type;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OpenGL code to render a sphere.
 */
void VolumeRenderer::renderSphere()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  gluSphere(sphericalQuadricObject, 1.0f, 20, 10);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OpenGL code to render a cube.
 */
void VolumeRenderer::renderCube()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  glBegin(GL_QUADS);

  glVertex3f(-1.0f, -1.0f, 1.0f);
  glVertex3f(1.0f, -1.0f, 1.0f);
  glVertex3f(1.0f, 1.0f, 1.0f);
  glVertex3f(-1.0f, 1.0f, 1.0f);

  glVertex3f(-1.0f, 1.0f, -1.0f);
  glVertex3f(1.0f, 1.0f, -1.0f);
  glVertex3f(1.0f, -1.0f, -1.0f);
  glVertex3f(-1.0f, -1.0f, -1.0f);

  glVertex3f(-1.0f, -1.0f, -1.0f);
  glVertex3f(1.0f, -1.0f, -1.0f);
  glVertex3f(1.0f, -1.0f, 1.0f);
  glVertex3f(-1.0f, -1.0f, 1.0f);

  glVertex3f(-1.0f, 1.0f, 1.0f);
  glVertex3f(1.0f, 1.0f, 1.0f);
  glVertex3f(1.0f, 1.0f, -1.0f);
  glVertex3f(-1.0f, 1.0f, -1.0f);

  glVertex3f(1.0f, -1.0f, -1.0f);
  glVertex3f(1.0f, 1.0f, -1.0f);
  glVertex3f(1.0f, 1.0f, 1.0f);
  glVertex3f(1.0f, -1.0f, 1.0f);

  glVertex3f(-1.0f, -1.0f, 1.0f);
  glVertex3f(-1.0f, 1.0f, 1.0f);
  glVertex3f(-1.0f, 1.0f, -1.0f);
  glVertex3f(-1.0f, -1.0f, -1.0f);

  glEnd();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OpenGL code to render a pyramid. A pyramid is usually a good representation
 * of a field of view sensor (e.g. camera or radar).
 */
void VolumeRenderer::renderPyramid()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //create bottom of pyramid
  glBegin(GL_QUADS);

  glVertex3f(2.0f, -1.0f, -1.0f);
  glVertex3f(2.0f, 1.0f, -1.0f);
  glVertex3f(2.0f, 1.0f, 1.0f);
  glVertex3f(2.0f, -1.0f, 1.0f);

  glEnd();

  //NOTE: We need to flush here or some graphics cards will get confused
  //when drawing the outline for the pyramid.
  glFlush();

  //create pyramid sides
  glBegin(GL_TRIANGLES);

  glVertex3f(2.0f, 1.0f, 1.0f);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(2.0f, -1.0f, 1.0f);

  glVertex3f(2.0f, -1.0f, 1.0f);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(2.0f, -1.0f, -1.0f);

  glVertex3f(2.0f, 1.0f, -1.0f);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(2.0f, 1.0f, 1.0f);

  glVertex3f(2.0f, -1.0f, -1.0f);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(2.0f, 1.0f, -1.0f);

  glEnd();
}
