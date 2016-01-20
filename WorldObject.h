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

#ifndef WORLD_OBJECT_H
#define WORLD_OBJECT_H

#include <QStringList>
#include "globals.h"
#include "IconRenderer.h"
#include "MeshRenderer.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the data behind a world object which is the central
 * object in the system. A world object has attributes like position,
 * orientation and scale but it also has renderers associated to it.
 *
 * @version 1.1
 * @author Hector Mendoza
 */
class WorldObject
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    enum Groups
    {
      NO_GROUP,
      PATH,
      VOLUME,
      TRACK
    };

    WorldObject();
    ~WorldObject();

    //main methods
    void renderIcon();
    void renderMesh();
    bool loadIcon(const QString& filePath);
    bool loadModel(const QString& filePath);
    void copy(WorldObject* source);

    //get methods
    const SimpleVector& getPosition() const;
    GeodeticPosition getGeodeticPosition();
    const SimpleVector& getScale() const;
    const SimpleVector& getRotation() const;
    bool getIsVisible();
    const QString& getType() const;
    const QString& getName() const;
    const QString& getLabel() const;
    const SimpleColor& getColor() const;
    IconRenderer* getIconRenderer();
    MeshRenderer* getMeshRenderer();
    bool getIsClickable();
    bool getIsExpirable();
    bool getHasExpired();
    int getExpirationTime();
    int getGroup();
    float getSpeed();
    const SimpleVector& getScreenLocation() const;
    const QStringList& getCustomInfo() const;

    //set methods
    void setPosition(const SimpleVector& position);
    void setGeodeticPosition(const GeodeticPosition& position);
    void setScale(const SimpleVector& scale);
    void setRotation(const SimpleVector& rotation);
    void setIsVisible(bool isVisible);
    void setType(const QString& type);
    void setName(const QString& name);
    void setLabel(const QString& label);
    void setColor(const SimpleColor& color);
    void setIconRenderer(IconRenderer* renderer);
    void setMeshRenderer(MeshRenderer* renderer);
    void setIsClickable(bool isClickable);
    void setIsExpirable(bool isExpirable);
    void setHasExpired(bool hasExpired);
    void setExpirationTime(int time);
    void setGroup(int group);
    void setSpeed(float speed);
    void setScreenLocation(const SimpleVector& screenLocation);
    void setCustomInfo(const QStringList& stringList);

  protected:
    SimpleVector mPosition;
    SimpleVector mScale;
    SimpleVector mRotation;
    bool mIsVisible;
    QString mType;
    QString mName;
    QString mLabel;
    SimpleColor mColor;
    IconRenderer* mIconRenderer;
    MeshRenderer* mMeshRenderer;
    bool mIsClickable;
    bool mIsExpirable;
    bool mHasExpired;
    int mExpirationTime;
    int mGroup;
    float mSpeed;
    SimpleVector mScreenLocation;
    QStringList mCustomInfo;

  private:
    WorldObject(const WorldObject&);//disallow default copying
    WorldObject& operator=(const WorldObject&);//disallow default copying

};

#endif//WORLD_OBJECT_H
