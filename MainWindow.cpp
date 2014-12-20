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

#include <QFileDialog>
#include <QProgressDialog>
#include <QDesktopServices>
#include <QUrl>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Hud.h"
#include "ToolMgr.h"
#include "PathTool.h"
#include "VolumeTool.h"
#include "MeasuringTool.h"
#include "CrossPlatformSleep.h"
#include "Camera.h"

MainWindow* MainWindow::mInstance = NULL;//Singleton implementation
static ToolMgr* toolMgr = NULL;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor. Calls parent constructor and initializes attributes. Connects
 * signals and slots.
 *
 * @param parent Handle to parent widget
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  ui->setupUi(this);

  mPathWindow = new PathWindow(this);
  mVolumeWindow = new VolumeWindow(this);
  mMeasuringWindow = new MeasuringWindow(this);
  mPlacesWindow = new PlacesWindow(this);
  mTrackInfoWindow = new TrackInfoWindow(this);
  mPathVolumeWindow = new PathVolumeWindow(this);
  mAboutWindow = new AboutWindow(this);
  mFileIO = new FileIO();
  mShapefileReader = new ShapefileReader();

  //create tool buttons
  mViewModeToolButton = new QToolButton();
  mViewModeToolButton->setIcon(QIcon("images/viewMode.png"));
  mViewModeToolButton->setCheckable(true);
  mViewModeToolButton->setToolTip("Perspective View");

  mPathToolButton = new QToolButton();
  mPathToolButton->setIcon(QIcon("images/path.png"));
  mPathToolButton->setCheckable(true);
  mPathToolButton->setToolTip("Path Tool");

  mVolumeToolButton = new QToolButton();
  mVolumeToolButton->setIcon(QIcon("images/volume.png"));
  mVolumeToolButton->setCheckable(true);
  mVolumeToolButton->setToolTip("Volume Tool");

  mMeasuringToolButton = new QToolButton();
  mMeasuringToolButton->setIcon(QIcon("images/measure.png"));
  mMeasuringToolButton->setCheckable(true);
  mMeasuringToolButton->setToolTip("Measuring Tool");

  //instantiate tool mgr and add tools
  toolMgr = ToolMgr::getInstance();

  PathTool* pathTool = new PathTool(mPathToolButton, mPathWindow);
  VolumeTool* volumeTool = new VolumeTool(mVolumeToolButton, mVolumeWindow);
  MeasuringTool* measuringTool = new MeasuringTool(mMeasuringToolButton, mMeasuringWindow);


  toolMgr->addTool(pathTool);
  toolMgr->addTool(volumeTool);
  toolMgr->addTool(measuringTool);

  //load places
  mPlacesWindow->loadPlaces();

  //create tool bar
  ui->toolBar->addWidget(mViewModeToolButton);
  ui->toolBar->addSeparator();
  ui->toolBar->addWidget(mPathToolButton);
  ui->toolBar->addWidget(mVolumeToolButton);
  ui->toolBar->addWidget(mMeasuringToolButton);

  connect(mViewModeToolButton, SIGNAL(clicked()), this, SLOT(onViewModeTool()));
  connect(mPathToolButton, SIGNAL(clicked()), this, SLOT(onPathTool()));
  connect(mVolumeToolButton, SIGNAL(clicked()), this, SLOT(onVolumeTool()));
  connect(mMeasuringToolButton, SIGNAL(clicked()), this, SLOT(onMeasuringTool()));
  connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
  connect(ui->actionPaths, SIGNAL(triggered()), this, SLOT(onPathSave()));
  connect(ui->actionVolumes, SIGNAL(triggered()), this, SLOT(onVolumeSave()));
  connect(ui->actionHud, SIGNAL(triggered()), this, SLOT(onHud()));
  connect(ui->actionLabels, SIGNAL(triggered()), this, SLOT(onLabels()));
  connect(ui->actionToolbar, SIGNAL(triggered()), this, SLOT(onToolbar()));
  connect(ui->actionPlaces, SIGNAL(triggered()), this, SLOT(onPlaces()));
  connect(ui->actionTrackInfo, SIGNAL(triggered()), this, SLOT(onTrackInfo()));
  connect(ui->actionPathVolume, SIGNAL(triggered()), this, SLOT(onPathVolume()));
  connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(onHelp()));
  connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));
  connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(onExit()));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
MainWindow::~MainWindow()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  delete ui;
  mInstance = NULL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Singleton pattern implementation. Returns the single instance of this class.
 *
 * @return The single instance of this class
 */
MainWindow* MainWindow::getInstance()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mInstance == NULL)
  {
    mInstance = new MainWindow();
  }

  return mInstance;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns a handle to the volume window object.
 *
 * @return Returns handle to volume window
 */
VolumeWindow* MainWindow::getVolumeWindow()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mVolumeWindow;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns a handle to the measuring window object.
 *
 * @return Returns handle to measuring window
 */
MeasuringWindow* MainWindow::getMeasuringWindow()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  return mMeasuringWindow;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Returns handle to the appropriate tool button depending on the given index.
 *
 * @param toolButton Index for tool button
 * @return Returns handle to appropriate tool button
 */
QToolButton* MainWindow::getToolButton(int toolButton)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (toolButton == VIEW_MODE_TOOL_BUTTON)
  {
    return mViewModeToolButton;
  }
  else if (toolButton == PATH_TOOL_BUTTON)
  {
    return mPathToolButton;
  }
  else if (toolButton == VOLUME_TOOL_BUTTON)
  {
    return mVolumeToolButton;
  }
  else if (toolButton == MEASURING_TOOL_BUTTON)
  {
    return mMeasuringToolButton;
  }
  return NULL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Displays the Track List Window and sets the current selection to the given
 * name.
 *
 * @param trackName Selected track name
 */
void MainWindow::displayTrackInfo(const QString& trackName)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mTrackInfoWindow->initialize();
  mTrackInfoWindow->show();
  mTrackInfoWindow->setSelectedName(trackName);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Displays the Volume Window in edit mode.
 *
 * @param volumeName Name of volume we are editing
 */
void MainWindow::displayVolumeEdit(const QString& volumeName)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  //toggle volume tool button in order to show volume window
  mVolumeToolButton->setChecked(true);
  onVolumeTool();
  //set volume window in edit mode
  mVolumeWindow->setupEdit(volumeName);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Simulates a click to the mViewModeToolButton in order to go back to orbit
 * navigation mode.
 */
void MainWindow::setNavigationModeToOrbit()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (!ui->glWidget->getCameraInOrbitMode())
  {
    //click the view mode button to go back to orbit navigation
    mViewModeToolButton->click();
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This method gets called when the user presses the View Mode toolbar button.
 * It toggles the camera mode between Earth orbit navigation and persdpective.
 */
void MainWindow::onViewModeTool()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  ui->glWidget->setCameraInOrbitMode(!ui->glWidget->getCameraInOrbitMode());
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the path tool is selected. Brings up path tool
 * window and sets mouse input on path tool mode.
 */
void MainWindow::onPathTool()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mPathToolButton->isChecked())
  {
    mPathWindow->initialize();
    toolMgr->selectTool("Path");
    ui->glWidget->setMouseInputMode(GLWidget::PATH_MODE);
  }
  else
  {
    toolMgr->selectTool("");
    ui->glWidget->setMouseInputMode(GLWidget::CAMERA_MOVE_MODE);
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the volume tool is selected. Brings up
 * volume tool window and sets mouse input on volume tool mode.
 */
void MainWindow::onVolumeTool()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mVolumeToolButton->isChecked())
  {
    mVolumeWindow->initialize();
    toolMgr->selectTool("Volume");
    ui->glWidget->setMouseInputMode(GLWidget::VOLUME_MODE);
  }
  else
  {
    toolMgr->selectTool("");
    ui->glWidget->setMouseInputMode(GLWidget::CAMERA_MOVE_MODE);
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the measuring tool is selected. Brings up measuring
 * tool window and sets mouse input on measuring tool mode.
 */
void MainWindow::onMeasuringTool()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (mMeasuringToolButton->isChecked())
  {
    toolMgr->selectTool("Measuring");
    ui->glWidget->setMouseInputMode(GLWidget::MEASURING_MODE);
  }
  else
  {
    toolMgr->selectTool("");
    ui->glWidget->setMouseInputMode(GLWidget::CAMERA_MOVE_MODE);
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the Open menu item is selected. Displays
 * open dialog and calls the appropriate reader class to read the file.
 */
void MainWindow::onOpen()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QString currentPath = QDir::currentPath();
  QStringList fileList = QFileDialog::getOpenFileNames(this, "Open File", currentPath, "Simple Earth File (*.sef);;Shape File (*.shp);;All Files (*.*)");
  QString fileName;

  int numFiles = fileList.size();
  QProgressDialog progress("Opening files...", "Cancel", 0, numFiles, this);
  progress.setWindowModality(Qt::WindowModal);

  //loop thru all selected files
  for (int i=0; i<fileList.size(); i++)
  {
    progress.setValue(i);
    if (progress.wasCanceled())
    {
      break;
    }

    fileName = fileList[i];
    if (!fileName.isEmpty())
    {
      if (fileName.contains(".shp"))
      {
        mShapefileReader->readFile(fileName);
      }
      else if (fileName.contains(".sef"))
      {
        mFileIO->readFile(fileName);
      }
      else
      {
        //if no extension, attempt to read Simple Earth type file
        mFileIO->readFile(fileName);
      }
    }
  }

  //hide progress dialog
  progress.setValue(numFiles);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the Save->Paths menu item is selected. Displays
 * save dialog and calls FileIO class to save paths.
 */
void MainWindow::onPathSave()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QString currentPath = QDir::currentPath();
  QString filename = QFileDialog::getSaveFileName(this, "Save File", currentPath + "/untitled.sef", "Simple Earth File (*.sef);;All Files (*.*)");
  if (!filename.isEmpty())
  {
    mFileIO->writeFile(filename, FileIO::PATHS_FILE);
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the Save->Volumes menu item is selected.
 * Displays save dialog and calls FileIO class to save volumes.
 */
void MainWindow::onVolumeSave()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QString currentPath = QDir::currentPath();
  QString filename = QFileDialog::getSaveFileName(this, "Save File", currentPath + "/untitled.sef", "Simple Earth File (*.sef);;All Files (*.*)");
  if (!filename.isEmpty())
  {
    mFileIO->writeFile(filename, FileIO::VOLUMES_FILE);
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the Hud item is selected under the View menu.
 * Shows/Hides the Heads Up Display (HUD).
 */
void MainWindow::onHud()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Hud::getInstance()->setShowHud(ui->actionHud->isChecked());
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the Labels item is selected under the View
 * menu. Sets global showLabels flag.
 */
void MainWindow::onLabels()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  Hud::getInstance()->setShowLabels(ui->actionLabels->isChecked());
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the Toolbar item is selected under the View menu.
 * Shows/Hides the toolbar.
 */
void MainWindow::onToolbar()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  if (ui->toolBar->isHidden())
  {
    ui->toolBar->show();
  }
  else
  {
    ui->toolBar->hide();
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the Places menu item is selected.
 */
void MainWindow::onPlaces()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mPlacesWindow->show();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the Track Info menu item is selected.
 * Shows Track Info window.
 */
void MainWindow::onTrackInfo()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mTrackInfoWindow->initialize();
  mTrackInfoWindow->show();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the Paths and Volumes menu item is selected.
 * Shows Path List window.
 */
void MainWindow::onPathVolume()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mPathVolumeWindow->initialize();
  mPathVolumeWindow->show();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the Help Resources menu item is selected. Invokes
 * the help initialization script.
 */
void MainWindow::onHelp()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QDesktopServices::openUrl(QUrl("file:///" + QFileInfo("./help/help.html").absoluteFilePath()));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the About menu item is selected. Shows about window.
 */
void MainWindow::onAbout()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mAboutWindow->show();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Qt SLOT. Gets called when the Exit menu item is selected. Closes the window.
 */
void MainWindow::onExit()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  this->close();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * OVERRIDE FOR QMainWindow. Gets callled whenever we close the Main Window.
 * Calls PlacesWindow to save curret places.
 *
 * @param event Qt's close event (not used but necessary for override)
 */
void MainWindow::closeEvent(QCloseEvent* event)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  mPlacesWindow->savePlaces();
}
