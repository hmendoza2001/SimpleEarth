TEMPLATE = app
TARGET = SimpleEarth2022

QT = core gui opengl network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#uncomment next line if you want to use elevation databases
#CONFIG += using_gdal
#uncomment next line if you want to use satellite imagery
#CONFIG += using_proj4
#uncomment next line if you want to load 3D models
#CONFIG += using_assimp

win32 {
  LIBS += -lopengl32
} else {
  LIBS += -lGLU
}

CONFIG(using_gdal) {
  win32 {
    INCLUDEPATH += FWTools/include
    LIBS += -LFWTools/lib \
      -lgdal_i
  } else {
    INCLUDEPATH += /usr/include/gdal
    LIBS += -lgdal
  }
}

CONFIG(using_proj4) {
  win32 {
    INCLUDEPATH += FWTools/include
    LIBS += -LFWTools/lib \
      -lproj_i
  } else {
    LIBS += -lproj
  }
}

CONFIG(using_assimp) {
  win32 {
    INCLUDEPATH += assimp/include
    LIBS += -Lassimp/bin32
  }
  LIBS += -lassimp
}


HEADERS += AboutWindow.h \
    Atmosphere.h \
    Camera.h \
    ColorSelectWidget.h \
    Constants.h \
    CrossPlatformSleep.h \
    Earth.h \
    ElevationManager.h \
    EventListener.h \
    EventPublisher.h \
    ExampleExpirableObject.h \
    ExampleFlyObject.h \
    ExampleHelloWorld.h \
    FileIO.h \
    globals.h \
    GLWidget.h \
    Hud.h \
    IconModelManager.h \
    IconRenderer.h \
    LabelTool.h \
    LabelWindow.h \
    MainWindow.h \
    MeasuringTool.h \
    MeasuringWindow.h \
    MeshRenderer.h \
    ModelRenderer.h \
    NewPlaceDialog.h \
    PathRenderer.h \
    PathTool.h \
    PathVolumeWindow.h \
    PathWindow.h \
    PlacesWindow.h \
    SatelliteImageDownloader.h \
    ShapefileReader.h \
    ShapeRenderer.h \
    Tool.h \
    ToolManager.h \
    TrackInfoWindow.h \
    Utilities.h \
    VolumeRenderer.h \
    VolumeTool.h \
    VolumeWindow.h \
    WorldObject.h \
    WorldObjectManager.h


SOURCES += AboutWindow.cpp \
    Atmosphere.cpp \
    Camera.cpp \
    ColorSelectWidget.cpp \
    CrossPlatformSleep.cpp \
    Earth.cpp \
    ElevationManager.cpp \
    EventPublisher.cpp \
    ExampleExpirableObject.cpp \
    ExampleFlyObject.cpp \
    ExampleHelloWorld.cpp \
    FileIO.cpp \
    GLWidget.cpp \
    Hud.cpp \
    IconModelManager.cpp \
    IconRenderer.cpp \
    LabelTool.cpp \
    LabelWindow.cpp \
    main.cpp \
    MainWindow.cpp \
    MeasuringTool.cpp \
    MeasuringWindow.cpp \
    MeshRenderer.cpp \
    ModelRenderer.cpp \
    NewPlaceDialog.cpp \
    PathRenderer.cpp \
    PathTool.cpp \
    PathVolumeWindow.cpp \    
    PathWindow.cpp \
    PlacesWindow.cpp \
    SatelliteImageDownloader.cpp \
    ShapefileReader.cpp \
    ShapeRenderer.cpp \
    Tool.cpp \
    ToolManager.cpp \
    TrackInfoWindow.cpp \
    Utilities.cpp \
    VolumeRenderer.cpp \
    VolumeTool.cpp \
    VolumeWindow.cpp \
    WorldObject.cpp \
    WorldObjectManager.cpp


FORMS += AboutWindow.ui \
    LabelWindow.ui \
    MainWindow.ui \
    MeasuringWindow.ui \
    NewPlaceDialog.ui \
    PathVolumeWindow.ui \
    PathWindow.ui \
    PlacesWindow.ui \
    TrackInfoWindow.ui \
    VolumeWindow.ui
