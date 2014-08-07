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

#include "Constants.h"
#include "Camera.h"
#include "math.h"
#include "Utilities.h"
#include "ElevationMgr.h"

//Singleton implementation
Camera* Camera::mInstance = NULL;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Initializes attributes.
 */
Camera::Camera()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mPosition.latitude = mDesiredPosition.latitude = 0.0;
  mPosition.longitude = mDesiredPosition.longitude = 0.0;
  mPosition.altitude = mDesiredPosition.altitude = Constants::CAMERA_INITIAL_ALTITUDE;
  mLookAt.x = mDesiredLookAt.x = 0.0;
  mLookAt.y = mDesiredLookAt.y = 0.0;
  mLookAt.z = mDesiredLookAt.z = 0.0;
  mUpVector.x = mDesiredUpVector.x = 0.0;
  mUpVector.y = mDesiredUpVector.y = 0.0;
  mUpVector.z = mDesiredUpVector.z = 1.0;
  mYaw = mDesiredYaw = Constants::PI/2.0;
  mLookAtAltitude = mDesiredLookAtAltitude = 0.0f;
  mCameraOrbitMove = true;
  mScreenSize.x = 1600;
  mScreenSize.y = 1200;
  mAltitudeGranularity = 300.0;
  mShiftAngleGranularity = 1.0;
  mYawAngleGranularity = 0.02;
  mPitchAltitudeGranulatity = 0.5f;
  mMaximumRadius = 40000.0f;
  mSyncMoveByScreen = false;
  mMoveByScreenInitialPoint.x = mMoveByScreenFinalPoint.x = 0;
  mMoveByScreenInitialPoint.y = mMoveByScreenFinalPoint.y = 0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
Camera::~Camera()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mInstance = NULL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Singleton pattern implementation. Returns the single instance of this class.
 *
 * @return Returns the single instance of this class.
 */
Camera* Camera::getInstance()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mInstance == NULL)
  {
    mInstance = new Camera();
  }

  return mInstance;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This is one of the main camera movement function. It moves the camera
 * according to the given discrete movement option.
 *
 * @param movement See discrete movement options in header file for valid values
 */
void Camera::moveByDiscrete(int movement)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  double overShoot;
  float groundElevation = ElevationMgr::getInstance()->getElevation(mDesiredPosition.latitude, mDesiredPosition.longitude);

  //compute radius increment step based on
  //distance from the surface of the Earth
  mAltitudeGranularity = (mDesiredPosition.altitude - groundElevation)/10.0;
  //clamp altitude increment to 1m
  if (mAltitudeGranularity < 0.001)
  {
    mAltitudeGranularity = 0.001;
  }

  mShiftAngleGranularity = (mDesiredPosition.altitude - (double)groundElevation)/10000.0;
  //clamp angle granularity to 1m at the equator
  if (mShiftAngleGranularity < 0.000009)
  {
    mShiftAngleGranularity = 0.000009;
  }

  if (movement == ZOOM_IN || movement == ZOOM_OUT)
  {
    if (movement == ZOOM_IN)
    {
      //prevent terrain getting too close to the camera
      //that it would dissapear due to near clipping
      overShoot = mDesiredPosition.altitude - mAltitudeGranularity;

      if (overShoot > (groundElevation+0.01))
      {
        mDesiredPosition.altitude -= mAltitudeGranularity;
      }
      else
      {
        //clamp at the ground
        mDesiredPosition.altitude = groundElevation + 0.01;
      }
    }
    else if (movement == ZOOM_OUT && mDesiredPosition.altitude <= mMaximumRadius)
    {
      mDesiredPosition.altitude += mAltitudeGranularity;
    }
  }

  if (mCameraOrbitMove)
  {
    if (movement == SHIFT_LEFT)
    {
      mDesiredPosition.longitude += mShiftAngleGranularity;
      if (mDesiredPosition.longitude > 180.0)
      {
        mDesiredPosition.longitude -= 360.0;
      }
    }
    else if (movement == SHIFT_RIGHT)
    {
      mDesiredPosition.longitude -= mShiftAngleGranularity;
      if (mDesiredPosition.longitude < -180.0)
      {
        mDesiredPosition.longitude += 360.0;
      }
    }
    else if (movement == SHIFT_UP)
    {
      //prevent gimbal lock at north pole
      overShoot = mDesiredPosition.latitude + mShiftAngleGranularity;
      if (overShoot < 85.0)
      {
        mDesiredPosition.latitude = overShoot;
      }
    }
    else if (movement == SHIFT_DOWN)
    {
      //prevent gimbal lock at south pole
      overShoot = mDesiredPosition.latitude - mShiftAngleGranularity;
      if (overShoot > -85.0)
      {
        mDesiredPosition.latitude = overShoot;
      }
    }
  }
  else
  {
    bool setLatitude = false;

    //if camera is in perspective view mode, the
    //movements need to reflect the forward vector
    if (movement == SHIFT_LEFT)
    {
      overShoot = mDesiredPosition.latitude - mShiftAngleGranularity * cos(mDesiredYaw);
      mDesiredPosition.longitude += mShiftAngleGranularity * sin(mDesiredYaw);
      setLatitude = true;
    }
    else if (movement == SHIFT_RIGHT)
    {
      overShoot = mDesiredPosition.latitude + mShiftAngleGranularity * cos(mDesiredYaw);
      mDesiredPosition.longitude -= mShiftAngleGranularity * sin(mDesiredYaw);
      setLatitude = true;
    }
    else if (movement == SHIFT_UP)
    {
      overShoot = mDesiredPosition.latitude + mShiftAngleGranularity * sin(mDesiredYaw);
      mDesiredPosition.longitude += mShiftAngleGranularity * cos(mDesiredYaw);
      setLatitude = true;
    }
    else if (movement == SHIFT_DOWN)
    {
      overShoot = mDesiredPosition.latitude - mShiftAngleGranularity * sin(mDesiredYaw);
      mDesiredPosition.longitude -= mShiftAngleGranularity * cos(mDesiredYaw);
      setLatitude = true;
    }
    else if (movement == YAW_LEFT)
    {
      mDesiredYaw += mYawAngleGranularity;
    }
    else if (movement == YAW_RIGHT)
    {
      mDesiredYaw -= mYawAngleGranularity;
    }
    else if (movement == PITCH_UP)
    {
      mDesiredLookAtAltitude += mPitchAltitudeGranulatity;
    }
    else if (movement == PITCH_DOWN)
    {
      mDesiredLookAtAltitude -= mPitchAltitudeGranulatity;
    }

    //protect from gimbal lock
    if (setLatitude && overShoot < 85.0 && overShoot > -85.0)
    {
      mDesiredPosition.latitude = overShoot;
    }

    //after the new camera position has been determined we need to
    //compute up vector and look at since we are in perspective view
    computeUpVectorAndLookAt();
  }//end of else of if (mCameraOrbitMove)
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the desired navigation end point and starts the navigation on a separate
 * thread.
 *
 * @param desiredPosition The desired end point for the navigation.
 */
void Camera::moveByDestinationPoint(GeodeticPosition desiredPosition)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mDesiredEndPoint = desiredPosition;
  start();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDEN FROM QThread. Executes on a separate thread. Gets called once by
 * moveByDestinationPoint to navigate to the desired location.
 */
void Camera::run()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //determine if sling shot effect needs to be
  //shown depending on desired camera altitude
  int slingshotAltitude;
  if (mDesiredEndPoint.altitude < 4000.0f)
  {
    slingshotAltitude = 4000.0f;
  }
  else
  {
    slingshotAltitude = mDesiredEndPoint.altitude;
  }

  //interpolate between current and desired position
  int numberOfSteps = 100;
  float latitudeStepSize = (mDesiredEndPoint.latitude - mDesiredPosition.latitude)/(float)numberOfSteps;
  float longitudeStepSize = (mDesiredEndPoint.longitude - mDesiredPosition.longitude)/(float)numberOfSteps;
  float altitudeStepSize1 = (slingshotAltitude - mDesiredPosition.altitude)/((float)numberOfSteps/2.0f);
  float altitudeStepSize2 = (mDesiredEndPoint.altitude - slingshotAltitude)/((float)numberOfSteps/2.0f);
  GeodeticPosition newPosition;

  for (int i=0; i<numberOfSteps; i++)
  {
    newPosition.latitude = mDesiredPosition.latitude + latitudeStepSize;
    newPosition.longitude = mDesiredPosition.longitude + longitudeStepSize;

    if (i < (numberOfSteps/2))
    {
      newPosition.altitude = mDesiredPosition.altitude + altitudeStepSize1;
    }
    else
    {
      newPosition.altitude = mDesiredPosition.altitude + altitudeStepSize2;
    }

    setGeodeticPosition(newPosition);
    msleep(33);
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This method takes two pixel coordinates and stores the values in order to
 * later compute the world coordinates of both locations and move the camera
 * according to the difference between the two points.
 *
 * @param initialLocation Initial pixel location
 * @param finalLocation Final pixel location
 */
void Camera::moveByScreen(ScreenCoordinates initialPoint, ScreenCoordinates finalPoint)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mMoveByScreenInitialPoint = initialPoint;
  mMoveByScreenFinalPoint = finalPoint;
  mSyncMoveByScreen = true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This method gets called under the OpenGL rendering context in order to
 * perform projections and set the desired position based on previosuly given
 * screen coordinates.
 */
void Camera::syncMoveByScreen()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mSyncMoveByScreen)
  {
    mSyncMoveByScreen = false;

    //compute geodetic position of screen locations
    GeodeticPosition initialPosGeo = Utilities::xyzToGeodetic(Utilities::screenToWorld(mMoveByScreenInitialPoint));
    GeodeticPosition finalPosGeo = Utilities::xyzToGeodetic(Utilities::screenToWorld(mMoveByScreenFinalPoint));

    //ensure both locations are on the surface of the earth. Note, Mt. Everest is
    //a little less than 9Km in altitude and is the highest point on the earth
    if (initialPosGeo.altitude < 9.0 && finalPosGeo.altitude < 9.0)
    {
      //get angle difference between both points and move camera by angle difference
      GeodeticPosition newPosition = mDesiredPosition;
      newPosition.latitude += initialPosGeo.latitude - finalPosGeo.latitude;
      newPosition.longitude += initialPosGeo.longitude - finalPosGeo.longitude;

      //also make sure we don't go near the poles to avoid gimbal lock
      if (newPosition.latitude < 85.0 && newPosition.latitude > -85.0)
      {
        setGeodeticPosition(newPosition);
      }

      //wrap longitude around
      if (mDesiredPosition.longitude > 180.0)
      {
        mDesiredPosition.longitude -= 360.0;
      }
      else if (mDesiredPosition.longitude < -180.0)
      {
        mDesiredPosition.longitude += 360.0;
      }
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This method ensures that the camera only moves under the OpenGL rendering
 * thread to achieve smooth camera motion.
 */
void Camera::sync()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mPosition = mDesiredPosition;
  mLookAt = mDesiredLookAt;
  mUpVector = mDesiredUpVector;
  mYaw = mDesiredYaw;
  mLookAtAltitude = mDesiredLookAtAltitude;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the current position of the camera in OpenGL(XYZ) coordinates.
 *
 * @return Current XYZ position of the camera
 */
SimpleVector Camera::getPosition()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return Utilities::geodeticToXYZ(mPosition);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the camera's current geodetic position.
 *
 * @return Current geodetic position for the camera
 */
GeodeticPosition Camera::getGeodeticPosition()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mPosition;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the current look-at position of the camera.
 *
 * @return Current look-at position of the camera
 */
SimpleVector Camera::getLookAt()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mLookAt;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the current up vector of the camera.
 *
 * @return Current camera up vector
 */
SimpleVector Camera::getUpVector()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mUpVector;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the camera yaw angle. This is only meaningful when the user is in
 * perspective view mode.
 *
 * @return The yaw angle in radians
 */
double Camera::getYaw()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mYaw;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the altitude for the camera's look at point.
 *
 * @return The altitude of current look at point
 */
float Camera::getLookAtAltitude()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mLookAtAltitude;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns true if the camera is set to orbit movement. False if camera is set
 * to pespective movement.
 *
 * @return True if camera is set to orbit movement, false if it is set to
 * perspective movement
 */
bool Camera::getCameraOrbitMove()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mCameraOrbitMove;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the current screen size.
 *
 * @return Screen size
 */
ScreenCoordinates Camera::getScreenSize()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mScreenSize;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the desired camera position in XYZ coordinates.
 *
 * @param position Desired camera position in XYZ
 */
void Camera::setPosition(SimpleVector position)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  setGeodeticPosition(Utilities::xyzToGeodetic(position));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the desired geodetic position for the camera.
 *
 * @param position Desired geodetic position
 */
void Camera::setGeodeticPosition(GeodeticPosition position)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mDesiredPosition = position;

  //after the new camera position has been determined we need to
  //compute up vector and look at if we are in perspective view
  if (!mCameraOrbitMove)
  {
      computeUpVectorAndLookAt();
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the current look at position of the camera.
 *
 * @param lookAt New look at position
 */
void Camera::setLookAt(SimpleVector lookAt)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mDesiredLookAt = lookAt;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the current up vector of the camera.
 *
 * @param upVector New camera up vector
 */
void Camera::setUpVector(SimpleVector upVector)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mDesiredUpVector = upVector;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the camera yaw angle. This is only meaningful when the user is in
 * perspective view mode.
 *
 * @param angle Yaw angle in radians
 */
void Camera::setYaw(double angle)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mDesiredYaw = angle;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the altitude of the camera's look at point.
 *
 * @param altitude New altitude of look at point
 */
void Camera::setLookAtAltitude(float altitude)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mDesiredLookAtAltitude = altitude;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Set whether the camera movement is orbit or perspective mode.
 *
 * @param value New camera movement mode
 */
void Camera::setCameraOrbitMove(bool value)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (value)
  {
    //orbit up vector is the z axis
    mDesiredUpVector.x = 0.0;
    mDesiredUpVector.y = 0.0;
    mDesiredUpVector.z = 1.0;

    //look at center of the Earth
    mDesiredLookAt.x = 0.0;
    mDesiredLookAt.y = 0.0;
    mDesiredLookAt.z = 0.0;
  }
  else
  {
    mDesiredYaw = Constants::PI / 2.0;//reset yaw
    computeUpVectorAndLookAt();
  }
  mCameraOrbitMove = value;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Set method for the current screen size.
 *
 * @param screenSize New value for screen size
 */
void Camera::setScreenSize(ScreenCoordinates screenSize)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mScreenSize = screenSize;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Computes up vector and look at position based on current position and yaw.
 */
void Camera::computeUpVectorAndLookAt()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  SimpleVector desiredPosition = Utilities::geodeticToXYZ(mDesiredPosition);
  float cameraRadius = mDesiredPosition.altitude + Constants::EARTH_MEAN_RADIUS;

  //compute up vector
  mDesiredUpVector.x = desiredPosition.x / cameraRadius;
  mDesiredUpVector.y = desiredPosition.y / cameraRadius;
  mDesiredUpVector.z = desiredPosition.z / cameraRadius;

  //look at is half a degree up towards the forward vector
  GeodeticPosition lookAtPosition = mDesiredPosition;
  lookAtPosition.latitude += sin(mDesiredYaw) / 2.0;
  lookAtPosition.longitude += cos(mDesiredYaw) / 2.0;
  lookAtPosition.altitude = mDesiredLookAtAltitude;
  mDesiredLookAt = Utilities::geodeticToXYZ(lookAtPosition);
}
