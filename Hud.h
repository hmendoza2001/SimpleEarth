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

#ifndef HUD_H
#define HUD_H

#include <QPainter>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the functionality to render the Heads Up Display
 * (HUD). Currently, the HUD renders text representing the camera position on
 * the upper left corner of the screen and also renders the labels for world
 * objects.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class Hud
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    static Hud* getInstance();
    ~Hud();

    bool getShowHud();
    bool getShowLabels();
    void setShowHud(bool show);
    void setShowLabels(bool show);
    void setPainter(QPainter* painter);
    void renderHud();

  private:
    Hud();//private due to Singleton implementation
    void renderWorldObjectLabels();

    static Hud* mInstance;
    bool mShowHud;
    bool mShowLabels;
    QPainter* mPainter;
};

#endif//HUD_H
