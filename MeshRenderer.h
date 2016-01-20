/*
 *  The Simple Earth Project
 *  Copyright (C) 2016 HueSoft LLC
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

#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

class WorldObject;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Interface definition for a Mesh Renderer. A mesh renderer encapsulates the
 * functionality to render a mesh (a collection of vertices).
 *
 * @version 1.1
 * @author Hector Mendoza
 */
class MeshRenderer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    MeshRenderer();
    virtual ~MeshRenderer();

    virtual void render() = 0;//forced override

    WorldObject* getWorldObject();
    void setWorldObject(WorldObject* worldObject);

  protected:
    WorldObject* mWorldObject;
};

#endif//MESH_RENDERER_H
