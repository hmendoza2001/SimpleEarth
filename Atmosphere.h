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

#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class encapsulates the functionality to render our planet's atmosphere.
 * This rendering could not be more simple. Instead of using scattering shaders
 * we just use a texture that simulates the atmosphere. This is simple, short
 * and gets the job done. Atmosphere is considered a special rendering object
 * because it needs to be rendered at the very end. This so that all other
 * objects are included in the texture transparency as well as to not interfere
 * with mouse picking of any other objects.
 *
 * @version 1.1
 * @author Hector Mendoza
 */
class Atmosphere
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    Atmosphere();
    ~Atmosphere();

    void render();
    void setTexture(unsigned int handle);

  private:
    unsigned int mTextureHandle;
};

#endif//ATMOSPHERE_H
