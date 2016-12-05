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

#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#include <QGLWidget>

#include "globals.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class is instantiated automatically by Qt, since the MainWindow class
 * contains this widget. This class inherits from QGLWidget. It is the main
 * class that renders OpenGL. It also handles user mouse and keyboard input.
 *
 * @version 1.1
 * @author Hector Mendoza
 */
class GLWidget : public QGLWidget
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Q_OBJECT

  public:
    enum MouseInputMode
    {
      CAMERA_MOVE_MODE,
      LABEL_MODE,
      PATH_MODE,
      VOLUME_MODE,
      MEASURING_MODE
    };

    GLWidget(QWidget* parent = 0);
    ~GLWidget();

    bool getCameraInOrbitMode();
    void setCameraInOrbitMode(bool value);
    void setMouseInputMode(int mode);

  public slots:
      void onRenderTimer();
      void onFrameRateTimer();

  protected:
    void initializeGL();//OVERRIDE
    void paintEvent(QPaintEvent *event);//OVERRIDE
    void resizeGL(int width, int height);//OVERRIDE
    void setupViewport(int width, int height);
    void keyPressEvent(QKeyEvent* event);//OVERRIDE
    void mousePressEvent(QMouseEvent* event);//OVERRIDE
    void mouseReleaseEvent(QMouseEvent* event);//OVERRIDE
    void mouseMoveEvent(QMouseEvent* event);//OVERRIDE
    void wheelEvent(QWheelEvent* event);//OVERRIDE

  private:
    ScreenCoordinates mLastMouseCoordinates;
    bool mLeftButtonPressed;
    bool mMiddleButtonPressed;
    bool mRightButtonPressed;
    unsigned int mTextures[3];
    unsigned int mFramesSinceLastCycle;
    QTimer* mRenderTimer;
    QTimer* mFrameRateTimer;
    int mMouseInputMode;
};

#endif//GL_WIDGET_H
