/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolBase.h,v $
  Language:  C++
  Date:      $Date: 2010/01/29 13:54:37 $
  Version:   $Revision: 1.1 $
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

#include "vvToolCreatorBase.h"
#include "vvToolInputSelectorWidget.h"
#include "clitkImageToImageGenericFilter.h"

//------------------------------------------------------------------------------
template<class ToolType>
class vvToolBase {
public:
  static void Initialize();  // can't be virtual, must be overwritten
  void UpdateInfoFromMainWindow();
  vvMainWindowToolInfo * mMainWindowToolInfo;

  static void SetToolName(QString n) { vvToolCreator<ToolType>::mSingleton->mToolName = n; }
  static void SetToolMenuName(QString n) { vvToolCreator<ToolType>::mSingleton->mToolMenuName = n; }
  static void SetToolIconFilename(QString n) { vvToolCreator<ToolType>::mSingleton->mToolIconFilename = n; }
  static void SetToolTip(QString n) { vvToolCreator<ToolType>::mSingleton->mToolTip = n; }

  void InitializeListOfInputImages();
  void InitializeListOfInputImages(vvToolInputSelectorWidget * w, 
                                   clitk::ImageToImageGenericFilterBase * f);
  std::vector<vvSlicerManager*> mSlicerManagersCompatible;
  unsigned int mCurrentIndex;

  vvImage::Pointer mCurrentImage;
  vvSlicerManager * mCurrentSliceManager;
  clitk::ImageToImageGenericFilterBase * mFilter;

};
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
template<class ToolType>
void vvToolBase<ToolType>::UpdateInfoFromMainWindow() { 
  mMainWindowToolInfo = vvToolCreator<ToolType>::mSingleton->mMainWindow->GetInfoForTool(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------  
template<class ToolType>
void vvToolBase<ToolType>::Initialize() {
  SetToolName("Unnamed tool");
  SetToolMenuName("Unnamed tool");
  SetToolIconFilename("");
  SetToolTip("Unamed tool.");
}
//------------------------------------------------------------------------------    


//------------------------------------------------------------------------------
template<class ToolType>
void vvToolBase<ToolType>::InitializeListOfInputImages(vvToolInputSelectorWidget * w, 
                                                       clitk::ImageToImageGenericFilterBase * f) {
  mFilter = f;
  InitializeListOfInputImages();
  w->Initialize(mSlicerManagersCompatible, mCurrentIndex);
  w->SetToolTip(mFilter->GetAvailableImageTypes().c_str());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
template<class ToolType>
void vvToolBase<ToolType>::InitializeListOfInputImages() {
  UpdateInfoFromMainWindow();

  std::vector<vvSlicerManager*> & mSlicerManagers = *mMainWindowToolInfo->mSlicerManagers;
  mCurrentIndex = mMainWindowToolInfo->mSlicerManagerCurrentIndex;
  DD("InputUpdate");
  DD(mCurrentIndex);
  // HERE : Check if image is ok (dimension, type etc ...)

  if (mFilter) {
    mFilter->PrintAvailableImageTypes();
    
  }
  else {
    /// if not set, do not check filter type
    std::cerr << "*** ERROR, set mFilter member in the vvTool class before calling InitializeListOfInputImages" << std::endl;
    exit(0);
  }

  //unsigned int previousIndex = mInputSequenceBox->mCurrentIndex();
  mSlicerManagersCompatible.clear();
  // mInputSequenceBox->clear();
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++) {
    
    vvImage * image = mSlicerManagers[i]->GetImage();
    
    if ((mFilter->CheckDimension(image->GetNumberOfDimensions()) && 
         mFilter->CheckPixelType(image->GetScalarTypeAsString()))) {
      mSlicerManagersCompatible.push_back(mSlicerManagers[i]);
      if (mCurrentIndex == i) {
        mCurrentIndex = mSlicerManagersCompatible.size()-1;
      }
    }
    else {
      std::cerr << mSlicerManagers[i]->GetFileName().c_str() << " not compatible image" << std::endl;
      if (mCurrentIndex == i) {
        mCurrentIndex = 0;
      }
    }
  }  
}
//------------------------------------------------------------------------------


#endif

