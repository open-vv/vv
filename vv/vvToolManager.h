/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolManager.h,v $
  Language:  C++
  Date:      $Date: 2010/02/03 10:54:08 $
  Version:   $Revision: 1.2 $
  Author :   David Sarrut (david.sarrut@creatis.insa-lyon.fr)

  Copyright (C) 2008
  Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
  CREATIS-LRMN http://www.creatis.insa-lyon.fr

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, version 3 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  =========================================================================*/

#ifndef VVTOOLMANAGER_H
#define VVTOOLMANAGER_H

#include "vvToolCreatorBase.h"
#include "clitkCommon.h"

class vvToolCreatorBase;
class vvMainWindow;

//------------------------------------------------------------------------------
// Manage a list of ToolCreator. Each tool will be automagically
// inserted into a list at construction time, before main. Then the
// MainWindow will call the 'Initialize' to insert the tool in the
// menu bar.
class vvToolManager {
public:
  // Functions
  static vvToolManager * GetInstance();  
  static void AddTool(vvToolCreatorBase * v);
  static void Initialize(vvMainWindow * m);
  static void UpdateEnabledTool();
  // Variables
   std::vector<vvToolCreatorBase *> mListOfTools;
  static vvToolManager * mSingleton;
};
//------------------------------------------------------------------------------

#endif

