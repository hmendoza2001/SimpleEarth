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

#ifndef CAMERA_H
#define CAMERA_H

#include <QThread>
#include "globals.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Singleton class that encapsulates the functionality behind the main OpenGL
 * camera. There are multiple ways to move the camera. moveByDiscrete provides
 * discrete movement optimal for keyboard control. moveByDestinationPoint
 * provides navigation movement to a destination point. Finally moveByScreen
 * provides movement that is optimal for mouse or touch. This class inherits
 * from QThread to execute navigation movement on a separate thread.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class Camera : public QThread
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    enum DiscreteMovements
    {
      SHIFT_LEFT,
      SHIFT_RIGHT,
      SHIFT_UP,
      SHIFT_DOWN,
      ZOOM_IN,
      ZOOM_OUT,
      YAW_LEFT,
      YAW_RIGHT,
      PITCH_UP,
      PITCH_DOWN
    };

    static Camera* getInstance();
    ~Camera();

    //main camera interface methods
    void moveByDiscrete(int movement);
    void moveByDestinationPoint(const GeodeticPosition& desiredPosition);
    void moveByScreen(const ScreenCoordinates& initialPoint, const ScreenCoordinates& finalPoint);
    void syncMoveByScreen();
    void sync();

    void run();//OVERRIDE

    //get methods
    SimpleVector getPosition();
    const GeodeticPosition& getGeodeticPosition() const;
    const SimpleVector& getLookAt() const;
    const SimpleVector& getUpVector() const;
    double getYaw();
    float getLookAtAltitude();
    bool getCameraOrbitMove();
    const ScreenCoordinates& getScreenSize() const;

    //set methods
    void setPosition(const SimpleVector& position);
    void setGeodeticPosition(const GeodeticPosition& position);
    void setLookAt(const SimpleVector& lookAt);
    void setUpVector(const SimpleVector& upVector);
    void setYaw(double angle);
    void setLookAtAltitude(float altitude);
    void setCameraOrbitMove(bool value);
    void setScreenSize(const ScreenCoordinates& screenSize);

  private:
    Camera();//private due to Singleton implementation
    void computeUpVectorAndLookAt();

    static Camera* mInstance;
    GeodeticPosition mPosition, mDesiredPosition;
    SimpleVector mLookAt, mDesiredLookAt;
    SimpleVector mUpVector, mDesiredUpVector;
    double mYaw, mDesiredYaw;
    float mLookAtAltitude, mDesiredLookAtAltitude;

    bool mCameraOrbitMove;
    double mAltitudeGranularity;
    double mShiftAngleGranularity;
    double mYawAngleGranularity;
    float mPitchAltitudeGranulatity;
    float mMaximumRadius;
    ScreenCoordinates mScreenSize;
    GeodeticPosition mDesiredEndPoint;
    bool mSyncMoveByScreen;
    ScreenCoordinates mMoveByScreenInitialPoint, mMoveByScreenFinalPoint;
};

#endif//CAMERA_H
