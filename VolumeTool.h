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

#ifndef VOLUME_TOOL_H
#define VOLUME_TOOL_H

#include <QMouseEvent>
#include <QString>
#include "globals.h"
#include "Tool.h"
#include "WorldObject.h"
#include "VolumeRenderer.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the functionality behind the volume tool. It inherits
 * from Tool (refer to Tool.h).
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class VolumeTool : public Tool
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    VolumeTool(QToolButton* toolButton = 0, QDialog* dialog = 0);
    ~VolumeTool();

    void render();//OVERRIDE
    bool addCurrent();
    void editCurrent(WorldObject* entity);
    void onMouseReleaseEvent(QMouseEvent* event);

    void setName(QString name);
    void setType(int type);
    void setPosition(SimpleVector position);
    void setRotation(SimpleVector rotation);
    void setScale(SimpleVector scale);
    void setColor(SimpleColor color);

  private:
    WorldObject* mWorldObject;
    VolumeRenderer* mVolumeRenderer;
    bool mCalculateClickPosition;
    ScreenCoordinates mClickCordinates;
};

#endif//VOLUME_TOOL_H
