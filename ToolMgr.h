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

#ifndef TOOL_MGR_H
#define TOOL_MGR_H

#include <QList>
#include "Tool.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * This class implements the facade and singleton patterns. The Tool Manager
 * handles all Tool objects (refer to Tool.h). When a tool is selected by the
 * user, the ToolMgr shows/hides the appropriate dialogs and
 * activates/deactivates their corresponding tool buttons.
 *
 * @version 1.0
 * @author Hector Mendoza
 */
class ToolMgr
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
  public:
    static ToolMgr* getInstance();
    ~ToolMgr();

    void addTool(Tool* tool);
    Tool* getTool(const QString& toolID);
    void selectTool(const QString& toolID);
    void renderSelectedTool();

  private:
    ToolMgr();//private due to Singleton implementation

    static ToolMgr* mInstance;
    QList<Tool*> mToolList;
    int mCurrentToolIndex;
};

#endif//TOOL_MGR_H
