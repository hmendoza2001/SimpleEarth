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

#include "WorldObject.h"
#include "Utilities.h"
#include "IconModelManager.h"
#include "ModelRenderer.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Initializes attributes.
 */
WorldObject::WorldObject()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mPosition.x = 0.0;
  mPosition.y = 0.0;
  mPosition.z = 0.0;
  mScale.x = 1.0;
  mScale.y = 1.0;
  mScale.z = 1.0;
  mRotation.x = 0.0;
  mRotation.y = 0.0;
  mRotation.z = 0.0;
  mIsVisible = true;
  mColor.red = 1.0f;
  mColor.green = 1.0f;
  mColor.blue = 1.0f;
  mColor.alpha = 1.0f;
  mIconRenderer = NULL;
  mMeshRenderer = NULL;
  mIsClickable = false;
  mIsExpirable = false;
  mHasExpired = false;
  mExpirationTime = 5;
  mGroup = NO_GROUP;
  mSpeed = 0.0f;
  mScreenLocation.x = 0.0;
  mScreenLocation.y = 0.0;
  mScreenLocation.z = 0.0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor. Frees allocated memory.
 */
WorldObject::~WorldObject()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mIconRenderer != NULL)
  {
    delete mIconRenderer;
  }
  if (mMeshRenderer != NULL)
  {
    delete mMeshRenderer;
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Calls the icon renderer's (if one exists) render method.
 */
void WorldObject::renderIcon()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mIconRenderer != NULL)
  {
    mIconRenderer->render();
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Calls the mesh renderer's (if one exists) render method.
 */
void WorldObject::renderMesh()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mMeshRenderer != NULL)
  {
    mMeshRenderer->render();
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Calls IconModelManager to make sure only one instance of this icon is loaded
 * into memory and creates and/or configures the icon renderer.
 *
 * @param filePath Path to icon/image file
 * @return False if loading of icon was unsuccessful
 */
bool WorldObject::loadIcon(const QString& filePath)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //call IconModelManager to make sure only one instance of the icon is loaded
  bool returnValue = IconModelManager::getInstance()->loadIcon(filePath);

  if (returnValue)
  {
    if (mIconRenderer == NULL)
    {
      mIconRenderer = new IconRenderer(filePath);
      //associate icon renderer with this world object
      mIconRenderer->setWorldObject(this);
    }
    else
    {
      mIconRenderer->setFilePath(filePath);
    }
  }

  return returnValue;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Calls IconModelManager to make sure only one instance of this model is loaded
 * into memory and creates and/or configures the mesh renderer.
 *
 * @param filePath Path to model file
 * @return False if loading of model was unsuccessful
 */
bool WorldObject::loadModel(const QString& filePath)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //call IconModelManager to make sure only one instance of the model is loaded
  bool returnValue = IconModelManager::getInstance()->loadModel(filePath);

  if (returnValue)
  {
    ModelRenderer* modelRenderer = NULL;
    if (mMeshRenderer == NULL)
    {
      modelRenderer = new ModelRenderer(filePath);
      mMeshRenderer = modelRenderer;
      //associate mesh renderer with this world object
      modelRenderer->setWorldObject(this);
    }
    else
    {
      //downcast to model renderer and set file path
      modelRenderer = (ModelRenderer*)mMeshRenderer;
      modelRenderer->setFilePath(filePath);
    }
  }

  return returnValue;;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Copies key attributes (except type and name) from source into this object.
 *
 * @param source Object we will copy values from
 */
void WorldObject::copy(WorldObject* source)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mIsVisible = source->getIsVisible();
  mPosition = source->getPosition();
  mScale = source->getScale();
  mRotation = source->getRotation();
  mColor = source->getColor();
  mLabel = source->getLabel();
  mGroup = source->getGroup();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the world object's current position.
 *
 * @return This object's current position
 */
const SimpleVector& WorldObject::getPosition() const
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mPosition;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the world object's current geodetic position.
 *
 * @return This object's current geodetic position
 */
GeodeticPosition WorldObject::getGeodeticPosition()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return Utilities::xyzToGeodetic(mPosition);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the world object's current x y z scale.
 *
 * @return This object's current scale or dimensions
 */
const SimpleVector& WorldObject::getScale() const
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mScale;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the world object's current rotation.
 *
 * @return This object's current rotation or orientation
 */
const SimpleVector& WorldObject::getRotation() const
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mRotation;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns true if this world object is visible.
 *
 * @return True if object is visible
 */
bool WorldObject::getIsVisible()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mIsVisible;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the world object's type. Type can be used for introspection purposes.
 *
 * @return This world object's type
 */
const QString& WorldObject::getType() const
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mType;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the world object's name. Name is used as a unique identifier.
 *
 * @return This object's name
 */
const QString& WorldObject::getName() const
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mName;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the world object's label. A label serves as the string displayed next
 * to the world object.
 *
 * @return This object's label
 */
const QString& WorldObject::getLabel() const
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mLabel;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the world object's current color.
 *
 * @return This object's current color in floating point RGBA format
 */
const SimpleColor& WorldObject::getColor() const
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mColor;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the world object's handle to the icon renderer.
 *
 * @return This object's handle to icon renderer object
 */
IconRenderer* WorldObject::getIconRenderer()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mIconRenderer;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns the world object's handle to the mesh renderer.
 *
 * @return This object's handle to mesh renderer object
 */
MeshRenderer* WorldObject::getMeshRenderer()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mMeshRenderer;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns true if the world object is clickable. mIsClickable is a flag used
 * to determine which objects need to be computed for picking.
 *
 * @return True if object is clickable
 */
bool WorldObject::getIsClickable()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mIsClickable;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns true if world object can expire. If it can, it means object will
 * automatically be removed from display list after time has expired.
 *
 * @return True if object can expire
 */
bool WorldObject::getIsExpirable()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mIsExpirable;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns true if this object is flagged as expired, in which case consumers of
 * this world object should stop use of it since it will be deleted from the
 * world object list.
 *
 * @return True if this object is flagged as expired.
 */
bool WorldObject::getHasExpired()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mHasExpired;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns this world object's expiration time.
 *
 * @return This object's expiration time in seconds
 */
int WorldObject::getExpirationTime()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mExpirationTime;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns this world object's group. See Groups enumeration for a list of valid
 * values.
 *
 * @return This world object's group.
 */
int WorldObject::getGroup()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mGroup;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns this world object's speed in Km/h.
 *
 * @return This world object's speed
 */
float WorldObject::getSpeed()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mSpeed;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns this world object's screen location. Note that screen location is
 * saved as a SimpleVector and not as ScreenCoordinates because we need the z
 * value to tell us if we have a negative hit in the projection (object lies
 * behind the camera).
 *
 * @return SimpleVector structure representing screen location
 */
const SimpleVector& WorldObject::getScreenLocation() const
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mScreenLocation;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns a string list that contains custom info for this world object. Custom
 * info just serves as a generic way to display any pertinent information about
 * the object.
 *
 * @return String list with custom info
 */
const QStringList& WorldObject::getCustomInfo() const
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mCustomInfo;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the current position for this world object.
 *
 * @param position New position for this world object
 */
void WorldObject::setPosition(const SimpleVector& position)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mPosition = position;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Takes the given geodetic position and sets the current xyz position of this
 * world object.
 *
 * @param position New geodetic position for this world object
 */
void WorldObject::setGeodeticPosition(const GeodeticPosition& position)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mPosition = Utilities::geodeticToXYZ(position);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the current scaling for this world object.
 *
 * @param scale New scaling or dimensions for this world object
 */
void WorldObject::setScale(const SimpleVector& scale)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mScale = scale;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the rotation(orientation) for this world object.
 *
 * @param rotation X, Y and Z rotations in degrees
 */
void WorldObject::setRotation(const SimpleVector& rotation)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mRotation = rotation;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets if this world object is visible or not.
 *
 * @param isVisible Flag to set if this world object is visible
 */
void WorldObject::setIsVisible(bool isVisible)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mIsVisible = isVisible;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the type for this world object. Type is a string that can be used for
 * introspection purposes.
 *
 * @param type New type for this world object
 */
void WorldObject::setType(const QString& type)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mType = type;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the name for this world object. The name serves as the unique identifier
 * for world objects in the system.
 *
 * @param name New name for this world object
 */
void WorldObject::setName(const QString& name)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mName = name;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the label for this world object. A label serves as the string displayed
 * next to the world object.
 *
 * @param label String representing this world object's label
 */
void WorldObject::setLabel(const QString& label)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mLabel = label;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the color for this world object.
 *
 * @param color New color in floating point RGBA format
 */
void WorldObject::setColor(const SimpleColor& color)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mColor = color;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the handle to the icon renderer for this object and associates the icon
 * renderer with this object.
 *
 * @param renderer Handle to IconRenderer object
 */
void WorldObject::setIconRenderer(IconRenderer* renderer)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mIconRenderer = renderer;
  mIconRenderer->setWorldObject(this);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the handle to the mesh renderer for this object and associates the mesh
 * renderer with this object.
 *
 * @param renderer Handle to MeshRenderer object
 */
void WorldObject::setMeshRenderer(MeshRenderer* renderer)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mMeshRenderer = renderer;
  mMeshRenderer->setWorldObject(this);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets whether or not the world object is clickable. mIsClickable is a flag
 * used to determine which objects need to be computed for picking.
 *
 * @param isClickable Set to true if this world object is clickable
 */
void WorldObject::setIsClickable(bool isClickable)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mIsClickable = isClickable;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets whether or not this object can expire. If it can, it means object will
 * automatically be removed from display list after time has expired.
 *
 * @param isExpirable Set to true if this object can expire
 */
void WorldObject::setIsExpirable(bool isExpirable)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mIsExpirable = isExpirable;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets whether or not this object is flagged as expired in which case consumers
 * of this world object should stop use of it since it will get deleted from the
 * world object list.
 *
 * @param hasExpired True means this world object has expired
 */
void WorldObject::setHasExpired(bool hasExpired)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mHasExpired = hasExpired;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the expiration time for this world object. This method can be called
 * to reset the expiration time for an object (e.g. when a position update has
 * been received) to prevent it from expiring and getting removed from the
 * display list.
 *
 * @param time Expiration time in seconds
 */
void WorldObject::setExpirationTime(int time)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mExpirationTime = time;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Set the group for this world object. See Groups enumeration for a list of valid
 * values.
 *
 * @param See Groups enumeration for a list of valid values.
 */
void WorldObject::setGroup(int group)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mGroup = group;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets the speed for the world object.
 *
 * @param speed Speed value should be in Km/h
 */
void WorldObject::setSpeed(float speed)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mSpeed = speed;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets this world object's screen location. Note that screen location is saved
 * as a SimpleVector and not as ScreenCoordinates because we need the z value
 * to tell us if we have a negative hit in the projection (object lies behind
 * the camera).
 *
 * @param screenLocation New screen location for this object
 */
void WorldObject::setScreenLocation(const SimpleVector& screenLocation)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mScreenLocation = screenLocation;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Sets this world object's custom info with the given list of strings. Custom
 * info just serves as a generic way to display any pertinent information about
 * the track.
 *
 * @param stringList List of string representing custom info
 */
void WorldObject::setCustomInfo(const QStringList& stringList)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mCustomInfo = stringList;
}
