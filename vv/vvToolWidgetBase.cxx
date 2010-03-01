/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolWidgetBase.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/01 15:38:09 $
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

#include "vvToolWidgetBase.h"
#include "vvMainWindowBase.h"
#include "vvSlicerManager.h"

//------------------------------------------------------------------------------
vvToolWidgetBase::vvToolWidgetBase(vvMainWindowBase * parent, Qt::WindowFlags f)
  :QDialog(parent, f), 
   Ui::vvToolWidgetBase() {

  // Set Modality : dialog is not modal but stay always on top because
  // parent is set at construction
  mIsInitialized = false;
  mFilter = 0;
  mMainWindowBase = parent;
  setModal(false);
  setAttribute(Qt::WA_DeleteOnClose);
  
  // GUI Initialization
  setupUi(this);

  // Connect signals & slots  
  connect(mMainWindowBase, SIGNAL(AnImageIsBeingClosed(vvSlicerManager*)), 
          this, SLOT(AnImageIsBeingClosed(vvSlicerManager*)));
  connect(mToolInputSelectionWidget, SIGNAL(accepted()), this, SLOT(InputIsSelected()));
  connect(mToolInputSelectionWidget, SIGNAL(rejected()), this, SLOT(close()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(apply()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

  // Disable main widget while input image is not selected
  mToolWidget->setEnabled(false);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolWidgetBase::~vvToolWidgetBase() {
  
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::show() {
 if (!mIsInitialized) {
   InitializeInputs();
 }
 QDialog::show();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolWidgetBase::close() {
  return QDialog::close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::AnImageIsBeingClosed(vvSlicerManager * m) {
  if (m == mCurrentSlicerManager) {
    close();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::InitializeInputs() {
  if (mFilter) {
    int j=0;
    mToolInputSelectionWidget->setToolTip(QString("%1").arg(mFilter->GetAvailableImageTypes().c_str()));
    for(unsigned int i=0; i<mMainWindowBase->GetSlicerManagers().size(); i++) {
      vvImage * s = mMainWindowBase->GetSlicerManagers()[i]->GetImage();
      if (mFilter->CheckImageType(s->GetNumberOfDimensions(), 
                                  s->GetNumberOfScalarComponents(), 
                                  s->GetScalarTypeAsString())) {
        mSlicerManagersCompatible.push_back(mMainWindowBase->GetSlicerManagers()[i]);
        if ((int)i == mMainWindowBase->GetSlicerManagerCurrentIndex()) mCurrentCompatibleIndex = j;
        j++;
      }
    }
  }
  else {
    mSlicerManagersCompatible = mMainWindowBase->GetSlicerManagers();
    mCurrentCompatibleIndex = mMainWindowBase->GetSlicerManagerCurrentIndex();
  }
  mToolInputSelectionWidget->Initialize(mSlicerManagersCompatible, 
                                        mCurrentCompatibleIndex);
  mIsInitialized = true;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::InputIsSelected() {
  // Common
  int index = mToolInputSelectionWidget->GetSelectedInputIndex();
  mCurrentSlicerManager = mSlicerManagersCompatible[index];
  mCurrentImage = mCurrentSlicerManager->GetImage();
  mToolWidget->setEnabled(true);
  if (!mCurrentSlicerManager) close();
  InputIsSelected(mCurrentSlicerManager);
}
//------------------------------------------------------------------------------

