/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolBase.h,v $
  Language:  C++
  Date:      $Date: 2010/03/01 07:37:25 $
  Version:   $Revision: 1.4 $
  Author :   David Sarrut (david.sarrut@creatis.insa-lyon.fr)

  Copyright (C) 2008
  Léon Bérard cancer center  http://oncora1.lyon.fnclcc.fr
  CREATIS                    http://www.creatis.insa-lyon.fr

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

#ifndef VVTOOLBASE_H
#define VVTOOLBASE_H

#include "vvToolBaseBase.h"
#include "vvToolCreator.h"

//------------------------------------------------------------------------------
template<class ToolType>
class vvToolBase : public vvToolBaseBase {
public:
  vvToolBase(vvMainWindowBase * m);
  static void Initialize();  // can't be virtual, must be overwritten

  static void SetToolName(QString n) { vvToolCreator<ToolType>::mSingleton->mToolName = n; }
  static void SetToolMenuName(QString n) { vvToolCreator<ToolType>::mSingleton->mToolMenuName = n; }
  static void SetToolIconFilename(QString n) { vvToolCreator<ToolType>::mSingleton->mToolIconFilename = n; }
  static void SetToolTip(QString n) { vvToolCreator<ToolType>::mSingleton->mToolTip = n; }

  void AddImage(vvImage::Pointer image,std::string filename) {
    CREATOR(ToolType)->GetMainWindow()->AddImage(image,filename); 
  }

};
//------------------------------------------------------------------------------

#include "vvToolBase.txx"

#endif

