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

#ifndef FILE_IO_H
#define FILE_IO_H

#include <QString>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the functionality behind the reading and writing of
 * files that contain data for labels, paths and volumes in the system.
 *
 * @version 1.1
 * @author Hector Mendoza
 */
class FileIO
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    enum FileType
    {
      LABELS_FILE,
      PATHS_FILE,
      VOLUMES_FILE
    };

    FileIO();
    ~FileIO();

    void readFile(const QString& fileName);
    void writeFile(const QString& fileName, int fileType);
};

#endif//FILE_IO_H
