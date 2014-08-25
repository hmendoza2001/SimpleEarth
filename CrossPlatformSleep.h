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

#ifndef CROSS_PLATFORM_SLEEP_H
#define CROSS_PLATFORM_SLEEP_H

#include <QThread>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates a platform independent method to do a system sleep.
 * We accomplish this by allowing public access to QThread's protected sleep
 * functions.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class CrossPlatformSleep : public QThread
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    CrossPlatformSleep();
    ~CrossPlatformSleep();

    void run();//FORCED OVERRIDE
    static void sleep(unsigned long seconds);//OVERRIDE
    static void msleep(unsigned long milliseconds);//OVERRIDE
};

#endif//CROSS_PLATFORM_SLEEP_H
