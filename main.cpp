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

#include <QApplication>
#include <QSplashScreen>
#include "MainWindow.h"
#include "CrossPlatformSleep.h"
#include "ElevationMgr.h"
#include "SatelliteImageDownloader.h"
#include "ExampleHelloWorld.h"
#include "ExampleFlyObject.h"
#include "ExampleExpirableObject.h"

//DOXYGEN MAIN PAGE
/**
 * \mainpage The Simple Earth Project v1.0
 *
 * The Simple Earth Project is a cross-platform open source project for
 * real-time GIS visualization enthusiasts. This project uses Qt
 * (qt-project.org) and OpenGL to render a 3-D globe that serves as a GIS
 * visualization sandbox.
 *
 * To start understanding the code and API, look at WorldObject and
 * WorldObjectMgr classes and the globals.h header file. The hello world
 * example emplaces a label in the world with the following code:
 *
 * //instantiate world object and set label<br>
 * WorldObject* worldObject = new WorldObject();<br>
 * worldObject->setLabel("Hello World!");<br>
 *
 * //set world object position<br>
 * GeodeticPosition position;<br>
 * position.latitude = 0.0;//in decimal degrees<br>
 * position.longitude = 0.0;//in decimal degrees<br>
 * position.altitude = 1.0;//in Km<br>
 * worldObject->setGeodeticPosition(position);<br>
 *
 * //add world object to manager so that it gets rendered<br>
 * WorldObjectMgr::getInstance()->addWorldObject(worldObject);
 *
 * We hope you find this code useful and contribute to further developing it.
 */

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Main application method. Execution starts here. This function instantiates
 * the QApplication and MainWindow objects. It also instantiates "plugin"
 * objects if dependencies are being used. Uncomment example code and look at
 * those classes to start understanding the code and API.
 *
 * @version 1.0
 * @author Hector Mendoza
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @return QApplication's exec result
 */
int main(int argc, char* argv[])
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QApplication app(argc, argv);

  //create and show splash screen
  QSplashScreen splash(QPixmap("images/splash.jpg"));
  splash.show();
  app.processEvents();

//give time for splash screen to show up
#ifndef USING_GDAL
#ifdef WIN32
  CrossPlatformSleep::msleep(1500);
#else
  //in linux we have to do this silly thing, otherwise
  //splash screen won't show up correctly
  for (int i=0; i<500; i++)
  {
    app.processEvents();
    CrossPlatformSleep::msleep(1);
  }
#endif//WIN32
#endif//USING_GDAL

  //instantiate main GUI window, this will instantiate GLWidget and other sub objects
  MainWindow* mainWindow = MainWindow::getInstance();

  //instantiate "plugin" objects
  //NOTE: DEFINITIONS FOR USING_GDAL AND USING_PROJ4 ARE LOCATED IN globals.h
  //ElevationMgr loads elevation databases
#ifdef USING_GDAL
  //load elevation database at startup
  splash.showMessage("Loading elevation database...", Qt::AlignLeft | Qt::AlignBottom, Qt::white);
  app.processEvents();
  ElevationMgr* elevationMgr = ElevationMgr::getInstance();
  elevationMgr->loadElevationDatabase("elevation/imgn32w107_1.img");//load example elevation database
#endif

  //SatelliteImageDownloader downloads sattelite imagery
#ifdef USING_PROJ4
  //instantiate object and start image download thread
  SatelliteImageDownloader* satelliteImageDownloader = new SatelliteImageDownloader();
  satelliteImageDownloader->start();
#ifdef USING_GDAL
  satelliteImageDownloader->setElevationMode(true);
#endif//USING_GDAL
#endif//USING_PROJ4

  //++++++++++++++++++++++++++++
  //EXAMPLES

  //uncomment next couple of lines if you want to run hello world example
  //ExampleHelloWorld* exampleHelloWorld = new ExampleHelloWorld();
  //exampleHelloWorld->helloWorld();

  //uncomment next couple of lines if you want to run example on how to emplace and fly an object
  //ExampleFlyObject* exampleFlyObject = new ExampleFlyObject();
  //exampleFlyObject->start();

  //uncomment next couple of lines if you want to run example on expirable objects
  //ExampleExpirableObject* exampleExpirableObject = new ExampleExpirableObject();
  //exampleExpirableObject->start();

  //END OF EXAMPLES
  //++++++++++++++++++++++++++++

  //show main window
  mainWindow->show();

  //hide splash screen
  splash.finish(mainWindow);

  //Qt's execution thread takes over
  return app.exec();
}
