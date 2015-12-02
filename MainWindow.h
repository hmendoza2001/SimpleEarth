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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include "EventPublisher.h"
#include "PathWindow.h"
#include "VolumeWindow.h"
#include "MeasuringWindow.h"
#include "PlacesWindow.h"
#include "TrackInfoWindow.h"
#include "PathVolumeWindow.h"
#include "AboutWindow.h"
#include "FileIO.h"
#include "ShapefileReader.h"

namespace Ui
{
  class MainWindow;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Singleton class that encapsulates all functionality behind the Main Window
 * for the system. To see the window's layout and contents please refer to
 * MainWindow.ui. This class inherits from QMainWindow. If you followed the code
 * from the main method and end up here, and think no other classes are
 * instantiated, bear in mind that GLWidget is contained by the MainWindow widget
 * (refer to MainWindow.ui) and instantiated automatically by Qt.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class MainWindow : public QMainWindow, public EventPublisher
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Q_OBJECT

  public:
    enum ToolButtons
    {
      VIEW_MODE_TOOL_BUTTON,
      PATH_TOOL_BUTTON,
      VOLUME_TOOL_BUTTON,
      MEASURING_TOOL_BUTTON
    };

    static MainWindow* getInstance();
    ~MainWindow();

    VolumeWindow* getVolumeWindow();
    MeasuringWindow* getMeasuringWindow();
    QToolButton* getToolButton(int toolButton);
    void displayTrackInfo(const QString& trackName);
    void displayVolumeEdit(const QString& volumeName);
    void setNavigationModeToOrbit();
    void closeEvent(QCloseEvent* event);//OVERRIDE

  public slots:
    void onViewModeTool();
    void onPathTool();
    void onVolumeTool();
    void onMeasuringTool();
    void onOpen();
    void onPathSave();
    void onVolumeSave();
    void onHud();
    void onLabels();
    void onToolbar();
    void onLatLonGrid();
    void onPlaces();
    void onTrackInfo();
    void onPathVolume();
    void onHelp();
    void onAbout();
    void onExit();

  private:
    MainWindow(QWidget* parent = 0);//private due to Singleton implementation
    void cleanup();

    Ui::MainWindow* ui;
    static MainWindow* mInstance;
    QToolButton* mViewModeToolButton;
    QToolButton* mPathToolButton;
    QToolButton* mVolumeToolButton;
    QToolButton* mMeasuringToolButton;
    PathWindow* mPathWindow;
    MeasuringWindow* mMeasuringWindow;
    VolumeWindow* mVolumeWindow;
    PlacesWindow* mPlacesWindow;
    TrackInfoWindow* mTrackInfoWindow;
    PathVolumeWindow* mPathVolumeWindow;
    AboutWindow* mAboutWindow;
    FileIO* mFileIO;
    ShapefileReader* mShapefileReader;
};

#endif//MAIN_WINDOW_H
