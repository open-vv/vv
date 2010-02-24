/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolCropImage.cxx,v $
  Language:  C++
  Date:      $Date: 2010/02/24 11:43:37 $
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

#include "vvToolCropImage.h"

//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolCropImage);
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolCropImage::vvToolCropImage(vvMainWindowBase * parent, Qt::WindowFlags f):
  vvToolWidgetBase(parent, f), 
  vvToolBase<vvToolCropImage>(parent), 
  Ui::vvToolCropImage() {
  // GUI Initialization
  Ui_vvToolCropImage::setupUi(mToolWidget);

  // Initialize the input selection
  //UpdateInfoFromMainWindow();
 //  InitializeListOfInputImages(*mMainWindowToolInfo->mSlicerManagers, 
// 			      mMainWindowToolInfo->mSlicerManagerCurrentIndex); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolCropImage::~vvToolCropImage() {

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::InputIsSelected(vvSlicerManager * slicer) {
  DD("InputIsSelected");
}
//------------------------------------------------------------------------------
