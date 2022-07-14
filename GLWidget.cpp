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
#include <GL/glu.h>

#include "GLWidget.h"
#include "MainWindow.h"
#include "WorldObjectManager.h"
#include "Camera.h"
#include "Earth.h"
#include "Atmosphere.h"
#include "Hud.h"
#include "ToolManager.h"
#include "LabelTool.h"
#include "PathTool.h"
#include "VolumeTool.h"
#include "MeasuringTool.h"
#include "Utilities.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

static Camera* camera = NULL;
static WorldObjectManager* worldObjectManager = NULL;
static Earth* earth = NULL;
static Atmosphere* atmosphere = NULL;
static Hud* hud = NULL;
static ToolManager* toolManager = NULL;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Calls parent constructor. Initializes attributes.
 *
 * @param parent Handle to parent widget.
 */
GLWidget::GLWidget(QWidget* parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //get handles for local variables
  camera = Camera::getInstance();
  worldObjectManager = WorldObjectManager::getInstance();
  hud = Hud::getInstance();
  toolManager = ToolManager::getInstance();

  //there are two "special" world objects, the Earth and the Atmosphere,
  //Earth gets "special treatment" because it gets rendered at the
  //very beginning and is a rather static object. The Atmosphere gets
  //rendered at the very end so that texture transparency includes all
  //objects as well as to not interfere with mouse picking
  earth = Earth::getInstance();
  atmosphere = new Atmosphere();

  mLastMouseCoordinates.x = 0;
  mLastMouseCoordinates.y = 0;
  mLeftButtonPressed = false;
  mMiddleButtonPressed = false;
  mRightButtonPressed = false;
  mFramesSinceLastCycle = 0;
  mMouseInputMode = CAMERA_MOVE_MODE;
  for (int i = 0; i < 3; i++)
  {
    mTextures[i] = 0;
  }

  //setup and start timers
  mRenderTimer = new QTimer(this);
  connect(mRenderTimer, SIGNAL(timeout()), this, SLOT(onRenderTimer()));
  mRenderTimer->start(17);

  mFrameRateTimer = new QTimer(this);
  connect(mFrameRateTimer, SIGNAL(timeout()), this, SLOT(onFrameRateTimer()));
  mFrameRateTimer->start(1000);

  //OpenGL will do the clear for us, so we disable auto widget clear
  setAutoFillBackground(false);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
GLWidget::~GLWidget()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  delete mRenderTimer;
  delete mFrameRateTimer;
  delete atmosphere;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns true if the camera is in Earth orbit navigation mode, false if it
 * is set in perspective mode.
 *
 * @return True if Earth orbit mode, false otherwise
 */
bool GLWidget::getCameraInOrbitMode()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return camera->getCameraOrbitMove();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the camera mode. If parameter is true, camera will be in Earth orbit
 * navigation mode. If parameter is false, the camera will be in perspective
 * mode.
 *
 * @param value Flag for camera view mode
 */
void GLWidget::setCameraInOrbitMode(bool value)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  camera->setCameraOrbitMove(value);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the current mouse input mode (see MouseInputMode enum for valid values).
 * The mouse input mode is determined depending on the tool that is currently
 * selected.
 *
 * @param mode New mouse input mode (see MouseInputMode enum for valid values)
 */
void GLWidget::setMouseInputMode(int mode)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mMouseInputMode = mode;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. This is the timeout function for the render timer. This function
 * gets called approx 60 times per second. It calls update which will trigger a
 * call to paintEvent;
 */
void GLWidget::onRenderTimer()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  update();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. This is the timeout function for the frame rate timer. This function
 * gets called once per second and it updates the FPS label in the status bar.
 */
void GLWidget::onFrameRateTimer()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  MainWindow::getInstance()->statusBar()->showMessage(QString::number(mFramesSinceLastCycle) + "FPS");
  mFramesSinceLastCycle = 0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR QGLWidget. Callback method for OpenGL initialization.
 */
void GLWidget::initializeGL()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QString imagePaths[3];
  imagePaths[0] = "images/blueMarble.jpg";
  imagePaths[1] = "images/stars.jpg";
  imagePaths[2] = "images/atmosphere.png";
  QImage images[3];

  //load earth, stars and atmosphere textures
  for (int i = 0; i < 3; i++)
  {
    if (!images[i].load(imagePaths[i]))
    {
      printf("GlWidget.cpp: Error loading image file %s.\n", imagePaths[i].toStdString().c_str());
    }
    mTextures[i] = Utilities::imageToTexture(images[i]);
  }

  //provide textures to earth and atmosphere object
  earth->setEarthTexture(mTextures[0]);
  earth->setStarTexture(mTextures[1]);
  atmosphere->setTexture(mTextures[2]);

  //now read any custom maps defined in Maps.txt
  earth->readMapsFile("Maps.txt");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR QGLWidget. Callback method to flush OpenGL buffers.
 * WARNING:DO NOT call this method directly, call updateGL instead.
 *
 * @param event Handle to QPaintEvent (not used but necessary for override)
 */
void GLWidget::paintEvent(QPaintEvent*)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  makeCurrent();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  //camera position might have been updated on a separate
  //thread, sync movement to ensure smooth motion
  camera->sync();
  SimpleVector cameraPosition = camera->getPosition();
  GeodeticPosition cameraGeodeticPosition = Utilities::xyzToGeodetic(cameraPosition);
  SimpleVector cameraLookAt = camera->getLookAt();
  SimpleVector cameraUpVector = camera->getUpVector();

  //paint background depending on camera altitude (blue if inside atmosphere)
  if (cameraGeodeticPosition.altitude < 7000.0f)
  {
    glClearColor(0.8f, 0.9f, 1.0f, 0.0f);
  }
  else
  {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  }

  mFramesSinceLastCycle++;

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_MULTISAMPLE);
  glDepthMask(GL_TRUE);
  glCullFace(GL_BACK);

  setupViewport(width(), height());

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z,
            cameraLookAt.x, cameraLookAt.y, cameraLookAt.z,
            cameraUpVector.x, cameraUpVector.y, cameraUpVector.z);

  //render our planet first
  earth->render();

  //sync mouse/touch camera movement
  camera->syncMoveByScreen();

  //render tools
  toolManager->renderSelectedTool();

  //render all other entities
  worldObjectManager->renderObjects();

  //render the atmosphere last to include all objects in transparency
  //as well as to not interfere with mouse picking
  atmosphere->render();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  //call Hud object to render hud
  QPainter painter(this);
  hud->setPainter(&painter);
  hud->renderHud();

  painter.end();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR QGLWidget. Callback method to handle window resizing.
 *
 * @param width Screen width
 * @param height Screen height
 */
void GLWidget::resizeGL(int width, int height)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  setupViewport(width, height);

  //privide the camera object with the latest screen size so that
  //other objects have access to this data
  ScreenCoordinates screenSize;
  screenSize.x = width;
  screenSize.y = height;
  camera->setScreenSize(screenSize);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This method is called by resizeGL as well as the paintEvent to set the
 * viewport/frustrum. Notice that on a regular OpenGL application, you would
 * only call this method on resizeGL, but we are using a QPainter along with
 * OpenGL code, so this method needs to be called by the paintEvent.
 *
 * @param width Screen width
 * @param height Screen height
 */
void GLWidget::setupViewport(int width, int height)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  float aspectRatio = (float)width/(float)height;
  float fieldOfView = 45.0f;

  //set near and far clipping according to camera altitude
  GeodeticPosition cameraPosition = camera->getGeodeticPosition();
  if (cameraPosition.altitude >= 10000.0f)
  {
    gluPerspective(fieldOfView, aspectRatio, 1000.0f, 50000.0f);
  }
  else if (cameraPosition.altitude < 10000.0f && cameraPosition.altitude >= 100.0f)
  {
    gluPerspective(fieldOfView, aspectRatio, 50.0f, 20000.0f);
  }
  else if (cameraPosition.altitude < 100.0f)
  {
    gluPerspective(fieldOfView, aspectRatio, 0.01f, 1000.0f);
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR QWidget. Gets called on a key press event.
 *
 * @param event Handle to Qt's key event
 */
void GLWidget::keyPressEvent(QKeyEvent* event)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (event->key() == Qt::Key_Left)
  {
    camera->moveByDiscrete(Camera::SHIFT_RIGHT);
  }
  else if (event->key() == Qt::Key_Right)
  {
    camera->moveByDiscrete(Camera::SHIFT_LEFT);
  }
  else if (event->key() == Qt::Key_Up)
  {
    camera->moveByDiscrete(Camera::SHIFT_UP);
  }
  else if (event->key() == Qt::Key_Down)
  {
    camera->moveByDiscrete(Camera::SHIFT_DOWN);
  }
  else if (event->key() == Qt::Key_Plus)
  {
    camera->moveByDiscrete(Camera::ZOOM_IN);
  }
  else if (event->key() == Qt::Key_Minus)
  {
    camera->moveByDiscrete(Camera::ZOOM_OUT);
  }
  else if (event->key() == Qt::Key_Q)
  {
    camera->moveByDiscrete(Camera::YAW_LEFT);
  }
  else if (event->key() == Qt::Key_E)
  {
    camera->moveByDiscrete(Camera::YAW_RIGHT);
  }
  else if (event->key() == Qt::Key_W)
  {
    camera->moveByDiscrete(Camera::PITCH_UP);
  }
  else if (event->key() == Qt::Key_S)
  {
    camera->moveByDiscrete(Camera::PITCH_DOWN);
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR QWidget. Gets called on a mouse button press event. This
 * method updates the "button pressed" flags accordingly.
 *
 * @param event Handle to Qt's mouse event
 */
void GLWidget::mousePressEvent(QMouseEvent* event)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (event->button() == Qt::LeftButton)
  {
    mLeftButtonPressed = true;
    mLastMouseCoordinates.x = event->x();
    mLastMouseCoordinates.y = event->y();
  }
  else if (event->button() == Qt::MiddleButton)
  {
    mMiddleButtonPressed = true;
    mLastMouseCoordinates.x = event->x();
    mLastMouseCoordinates.y = event->y();
  }
  else if (event->button() == Qt::RightButton)
  {
    mRightButtonPressed = true;
    if (mMouseInputMode == MEASURING_MODE)
    {
      MeasuringTool* measuringTool = (MeasuringTool*)ToolManager::getInstance()->getTool("Measuring");
      measuringTool->onMousePressEvent(event);
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR QWidget. Gets called on a mouse button release event. This
 * method updates the "button pressed" flags accordingly.
 *
 * @param event Handle to Qt's mouse event
 */
void GLWidget::mouseReleaseEvent(QMouseEvent* event)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (event->button() == Qt::LeftButton)
  {
    mLeftButtonPressed = false;
  }
  else if (event->button() == Qt::MiddleButton)
  {
    mMiddleButtonPressed = false;
  }
  else if (event->button() == Qt::RightButton)
  {
    mRightButtonPressed = false;
    if (mMouseInputMode == CAMERA_MOVE_MODE)
    {
      WorldObject* worldObject;
      SimpleVector screenLocation;
      ScreenCoordinates screenSize;
      for (int i = 0; i < worldObjectManager->getNumberOfObjects(); i++)
      {
        worldObject = worldObjectManager->getWorldObject(i);
        if (worldObject!=NULL && !worldObject->getHasExpired() && worldObject->getIsClickable())
        {
          screenSize = camera->getScreenSize();
          screenLocation = worldObject->getScreenLocation();
          screenLocation.y = (float)screenSize.y - screenLocation.y;//reverse Y
          //only select objects if not behind camera and not obsucred
          if (screenLocation.z < 1.0f &&
              !Utilities::checkObscure(camera->getGeodeticPosition(), worldObject->getGeodeticPosition()) &&
              (event->x() < screenLocation.x + 20.0) &&
              (event->x() > screenLocation.x - 20.0) &&
              (event->y() < screenLocation.y + 20.0) &&
              (event->y() > screenLocation.y - 20.0) )
          {
            //display track information
            MainWindow::getInstance()->displayTrackInfo(worldObject->getName());

            //publish track selection event
            QStringList event;
            event.append("TrackSelection");
            event.append(worldObject->getName());
            MainWindow::getInstance()->publishEvent(event);
            break;
          }
        }
      }
    }
    else if (mMouseInputMode == LABEL_MODE)
    {
      LabelTool* labelTool = (LabelTool*)ToolManager::getInstance()->getTool("Label");
      labelTool->onMouseReleaseEvent(event);
    }
    else if (mMouseInputMode == PATH_MODE)
    {
      PathTool* pathTool = (PathTool*)ToolManager::getInstance()->getTool("Path");
      pathTool->onMouseReleaseEvent(event);
    }
    else if (mMouseInputMode == VOLUME_MODE)
    {
      VolumeTool* volumeTool = (VolumeTool*)ToolManager::getInstance()->getTool("Volume");
      volumeTool->onMouseReleaseEvent(event);
    }
    else if (mMouseInputMode == MEASURING_MODE)
    {
      MeasuringTool* measuringTool = (MeasuringTool*)ToolManager::getInstance()->getTool("Measuring");
      measuringTool->onMouseReleaseEvent(event);
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR QWidget. Gets called on a mouse move event. We determine which
 * button (left, middle or right) got pressed thanks to our member flags since
 * the event object itself does not have the right information at the time this
 * method gets called.
 *
 * @param event Handle to Qt's mouse event
 */
void GLWidget::mouseMoveEvent(QMouseEvent* event)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mLeftButtonPressed)
  {
    ScreenCoordinates initialPosition = mLastMouseCoordinates;
    ScreenCoordinates finalPosition;
    finalPosition.x = event->x();
    finalPosition.y = event->y();
    camera->moveByScreen(initialPosition, finalPosition);

    mLastMouseCoordinates.x = event->x();
    mLastMouseCoordinates.y = event->y();
  }
  else if (mMiddleButtonPressed)
  {
    if (event->x() > mLastMouseCoordinates.x)
    {
      camera->moveByDiscrete(Camera::YAW_RIGHT);
      mLastMouseCoordinates.x = event->x();
    }
    else if (event->x() < mLastMouseCoordinates.x)
    {
      camera->moveByDiscrete(Camera::YAW_LEFT);
      mLastMouseCoordinates.x = event->x();
    }
  }
  else if (mRightButtonPressed)
  {
    if (mMouseInputMode == MEASURING_MODE)
    {
      MeasuringTool* measuringTool = (MeasuringTool*)ToolManager::getInstance()->getTool("Measuring");
      measuringTool->onMouseMoveEvent(event);
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR QWidget. Gets called on a mouse wheel event.
 *
 * @param event Handle to Qt's mouse wheel event object
 */
void GLWidget::wheelEvent(QWheelEvent* event)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (event->delta() > 0)
  {
    camera->moveByDiscrete(Camera::ZOOM_IN);
  }
  else
  {
    camera->moveByDiscrete(Camera::ZOOM_OUT);
  }
}
