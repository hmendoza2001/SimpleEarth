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

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QMessageBox>
#include "FileIO.h"
#include "WorldObjectMgr.h"
#include "PathRenderer.h"
#include "VolumeRenderer.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Constructor.
 */
FileIO::FileIO()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Destructor.
 */
FileIO::~FileIO()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Reads a path or volume data file. It instantiates the appropriate entities
 * and adds them to WorldObjectMgr.
 *
 * @param fileName Name of file to be read
 */
void FileIO::readFile(const QString& fileName)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QMessageBox::critical(NULL, "Error", "Error opening file.");
  }
  else
  {
    QStringList split;
    int fileType = -1;
    QTextStream in(&file);
    QString line;

    //read first line to determine which type of file it is
    line = in.readLine();
    if (line.contains("PATHS"))
    {
      fileType = PATHS_FILE;
    }
    else if (line.contains("VOLUMES"))
    {
      fileType = VOLUMES_FILE;
    }

    if (fileType == PATHS_FILE)
    {
      QString name;
      SimpleColor color;
      SimpleVector point;
      QList<SimpleVector> points;

      while (!in.atEnd())
      {
        line = in.readLine();

        if (line.contains("PathName="))
        {
          split = line.split("=");
          name = split[1];
        }
        else if (line.contains("PathColorR="))
        {
          split = line.split("=");
          color.red = split[1].toFloat();
        }
        else if (line.contains("PathColorG="))
        {
          split = line.split("=");
          color.green = split[1].toFloat();
        }
        else if (line.contains("PathColorB="))
        {
          split = line.split("=");
          color.blue = split[1].toFloat();
        }
        else if (line.contains("PathPointX="))
        {
          split = line.split("=");
          point.x = split[1].toFloat();
        }
        else if (line.contains("PathPointY="))
        {
          split = line.split("=");
          point.y = split[1].toFloat();
        }
        else if (line.contains("PathPointZ="))
        {
          split = line.split("=");
          point.z = split[1].toFloat();
        }
        else if (line.contains("ENDPOINT"))
        {
          points.append(point);
        }
        else if (line.contains("ENDPATH"))
        {
          //instantiate path world object
          WorldObject* worldObject = new WorldObject();
          worldObject->setName(name);
          worldObject->setColor(color);
          worldObject->setGroup(WorldObject::PATH);

          //instantiate path renderer and add points
          PathRenderer* pathRenderer = new PathRenderer();
          for (int i=0; i<points.size(); i++)
          {
            pathRenderer->addPoint(points[i]);
          }

          //add renderer to world object
          worldObject->setMeshRenderer(pathRenderer);

          //add world object to manager's list
          WorldObjectMgr::getInstance()->addWorldObject(worldObject);
        }
      }
    }
    else if (fileType == VOLUMES_FILE)
    {
      QString name;
      QString type;
      SimpleVector position;
      SimpleVector rotation;
      SimpleVector scale;
      SimpleColor color;

      while (!in.atEnd())
      {
        line = in.readLine();

        if (line.contains("Name="))
        {
          split = line.split("=");
          name = split[1];
        }
        else if (line.contains("Type="))
        {
          split = line.split("=");
          type = split[1];
        }
        else if (line.contains("PositionX="))
        {
          split = line.split("=");
          position.x = split[1].toFloat();
        }
        else if (line.contains("PositionY="))
        {
          split = line.split("=");
          position.y = split[1].toFloat();
        }
        else if (line.contains("PositionZ="))
        {
          split = line.split("=");
          position.z = split[1].toFloat();
        }
        else if (line.contains("RotationX="))
        {
          split = line.split("=");
          rotation.x = split[1].toFloat();
        }
        else if (line.contains("RotationY="))
        {
          split = line.split("=");
          rotation.y = split[1].toFloat();
        }
        else if (line.contains("RotationZ="))
        {
          split = line.split("=");
          rotation.z = split[1].toFloat();
        }
        else if (line.contains("ScaleX="))
        {
          split = line.split("=");
          scale.x = split[1].toFloat();
        }
        else if (line.contains("ScaleY="))
        {
          split = line.split("=");
          scale.y = split[1].toFloat();
        }
        else if (line.contains("ScaleZ="))
        {
          split = line.split("=");
          scale.z = split[1].toFloat();
        }
        else if (line.contains("ColorR="))
        {
          split = line.split("=");
          color.red = split[1].toFloat();
        }
        else if (line.contains("ColorG="))
        {
          split = line.split("=");
          color.green = split[1].toFloat();
        }
        else if (line.contains("ColorB="))
        {
          split = line.split("=");
          color.blue = split[1].toFloat();
        }
        else if (line.contains("END_VOLUME"))
        {
          WorldObject* worldObject = new WorldObject();
          worldObject->setName(name);
          worldObject->setPosition(position);
          worldObject->setRotation(rotation);
          worldObject->setScale(scale);
          worldObject->setColor(color);
          worldObject->setGroup(WorldObject::VOLUME);

          //instantiate renderer ans set volume type
          VolumeRenderer* volumeRenderer = new VolumeRenderer();
          if (type == "Sphere")
          {
            volumeRenderer->setType(VolumeRenderer::SPHERE);
          }
          else if (type == "Cube")
          {
            volumeRenderer->setType(VolumeRenderer::CUBE);
          }
          else if (type == "Pyramid")
          {
            volumeRenderer->setType(VolumeRenderer::PYRAMID);
          }

          //add renderer to world object
          worldObject->setMeshRenderer(volumeRenderer);

          //add world object to manager's list
          WorldObjectMgr::getInstance()->addWorldObject(worldObject);

        }//else if (line.contains("END_VOLUME"))
      }//while (in.atEnd())
    }//else if (fileType == VOLUMES_FILE)
  }//no error reading
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Based on the file type, writes the current path or volume data to the given
 * file name.
 *
 * @param fileName File name to write data to
 * @param fileType See file type enum in header declaration
 */
void FileIO::writeFile(const QString& fileName, int fileType)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QMessageBox::critical(NULL, "Error", "Error writing file.");
    return;
  }

  QTextStream out(&file);
  WorldObjectMgr* worldObjectMgr = WorldObjectMgr::getInstance();
  WorldObject* worldObject = NULL;

  if (fileType == PATHS_FILE)
  {
    int i,j;
    QList<SimpleVector>* pointList;
    SimpleColor color;
    out << "PATHS" << "\n";
    for (i=0; i<worldObjectMgr->getNumberOfObjects(); i++)
    {
      worldObject = worldObjectMgr->getWorldObject(i);
      if (worldObject!=NULL && !worldObject->getHasExpired() && worldObject->getGroup()==WorldObject::PATH)
      {
        out << "PathName=" << worldObject->getName() << "\n";
        color = worldObject->getColor();
        out << "PathColorR=" << color.red << "\n";
        out << "PathColorG=" << color.green << "\n";
        out << "PathColorB=" << color.blue << "\n";

        PathRenderer* pathRenderer = (PathRenderer*)worldObject->getMeshRenderer();
        pointList = pathRenderer->getPoints();

        for (j=0; j<pointList->size(); j++)
        {
          out << "PathPointX=" << pointList->at(j).x << "\n";
          out << "PathPointY=" << pointList->at(j).y << "\n";
          out << "PathPointZ=" << pointList->at(j).z << "\n";
          out << "ENDPOINT" << "\n";
        }
        out << "ENDPATH" << "\n";
      }
    }
  }
  else if (fileType == VOLUMES_FILE)
  {
    out << "VOLUMES" << "\n";
    SimpleVector position, rotation, scale;
    SimpleColor color;
    for (int i=0; i<worldObjectMgr->getNumberOfObjects(); i++)
    {
      worldObject = worldObjectMgr->getWorldObject(i);
      if (worldObject!=NULL && !worldObject->getHasExpired() && worldObject->getGroup()==WorldObject::VOLUME)
      {
        out << "Name=" << worldObject->getName() << "\n";

        //get and write volume type
        VolumeRenderer* volumeRenderer = (VolumeRenderer*)worldObject->getMeshRenderer();
        int volumeType = volumeRenderer->getType();
        if (volumeType == VolumeRenderer::SPHERE)
        {
          out << "Type=Sphere\n";
        }
        else if (volumeType == VolumeRenderer::CUBE)
        {
          out << "Type=Cube\n";
        }
        else if (volumeType == VolumeRenderer::PYRAMID)
        {
          out << "Type=Pyramid\n";
        }

        position = worldObject->getPosition();
        out << "PositionX=" << position.x << "\n";
        out << "PositionY=" << position.y << "\n";
        out << "PositionZ=" << position.z << "\n";

        rotation = worldObject->getRotation();
        out << "RotationX=" << rotation.x << "\n";
        out << "RotationY=" << rotation.y << "\n";
        out << "RotationZ=" << rotation.z << "\n";

        scale = worldObject->getScale();
        out << "ScaleX=" << scale.x << "\n";
        out << "ScaleY=" << scale.y << "\n";
        out << "ScaleZ=" << scale.z << "\n";

        color = worldObject->getColor();
        out << "ColorR=" << color.red << "\n";
        out << "ColorG=" << color.green << "\n";
        out << "ColorB=" << color.blue << "\n";

        out << "END_VOLUME" << "\n";
      }
    }//end of for loop
  }//end of else if volumes
}
