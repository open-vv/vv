/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolCreatorBase.txx,v $
  Language:  C++
  Date:      $Date: 2010/01/29 13:54:37 $
  Version:   $Revision: 1.1 $
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

#include "vvMainWindow.h"
#include "vvSlicerManager.h"

//------------------------------------------------------------------------------
template<class ToolType>
void vvToolCreatorBase::CreateTool() {
  DD("Create new tool ");
  ToolType * tool = new ToolType(mMainWindow);
  tool->show();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
template<class ToolType>
void vvToolCreator<ToolType>::Initialize(vvMainWindow * m) { 
  // Set MainWindow pointer
  mMainWindow = m;
  
  // Default Initialization
  mToolMenuName = mToolName;
  mToolIconFilename = "noicon";
  mToolTip = mToolName;

  // User Tool Initialization
  ToolType::Initialize();

  // Common Initialization (insertion into menu)
  vvToolCreatorBase::Initialize(mMainWindow); 
}
//------------------------------------------------------------------------------

// template<class ToolType>
//   void vvToolCreator<ToolType>::UpdateInfoFromMainWindow() { mMainWindowToolInfo = CREATOR(ToolType)->mMainWindow->GetInfoForTool(); }
