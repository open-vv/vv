/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolWidgetBase.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/17 11:22:18 $
  Version:   $Revision: 1.3 $
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
  mCurrentSlicerManager = 0;
  
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
// void vvToolWidgetBase::SetNumberOfNeededInputs(int nb) {
//   mNumberOfInputs = nb;
//   mListOfFilters.resize(nb);
//   for(int i=0; i<mNumberOfInputs; i++) mListOfFilters[i] = 0;
// }
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::AddInputSelector(clitk::ImageToImageGenericFilterBase * f) {
  DD("AddInputSelector with filter");
  DD(mMainWindowBase->GetSlicerManagers().size());
  int j=0;
  mFilter = f;
  mSlicerManagersCompatible.clear();
  //  mToolInputSelectionWidget->setToolTip(QString("%1").arg(mFilter->GetAvailableImageTypes().c_str()));
  for(unsigned int i=0; i<mMainWindowBase->GetSlicerManagers().size(); i++) {
    DD(i);
    vvImage * s = mMainWindowBase->GetSlicerManagers()[i]->GetImage();
    if (mFilter->CheckImageType(s->GetNumberOfDimensions(), 
				s->GetNumberOfScalarComponents(), 
				s->GetScalarTypeAsString())) {
      mSlicerManagersCompatible.push_back(mMainWindowBase->GetSlicerManagers()[i]);
      if ((int)i == mMainWindowBase->GetSlicerManagerCurrentIndex()) mCurrentCompatibleIndex = j;
      j++;
    }
  }
  mToolInputSelectionWidget->AddInputSelector(mSlicerManagersCompatible, mCurrentCompatibleIndex);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::AddInputSelector() {
  DD("AddInput without filter");
  DD(mMainWindowBase->GetSlicerManagers().size());
  mSlicerManagersCompatible.clear();
  for(unsigned int i=0; i<mMainWindowBase->GetSlicerManagers().size(); i++) {
    mSlicerManagersCompatible.push_back(mMainWindowBase->GetSlicerManagers()[i]);
  }
  mToolInputSelectionWidget->AddInputSelector(mMainWindowBase->GetSlicerManagers(),
					      mMainWindowBase->GetSlicerManagerCurrentIndex());
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::show() {
 if (!mIsInitialized) {
   DD("show -> init");
   mToolInputSelectionWidget->Initialize();
   mIsInitialized = true;
 }
 QDialog::show();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolWidgetBase::close() {
  DD("vvToolWidgetBase::close()");
  return QDialog::close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::AnImageIsBeingClosed(vvSlicerManager * m) {
   mToolInputSelectionWidget->AnImageIsBeingClosed(m);
  if (m == mCurrentSlicerManager) {
    close();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::InitializeInputs() {
  /*
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
  */
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void vvToolWidgetBase::SetNumberOfNeededInputs(int i) {
//   DD("SetNumberOfNeededInputs");
//   DD(i);
// }
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::InputIsSelected() {
  DD("InputIsSelected");
  buttonBox->setEnabled(true);
  std::vector<vvSlicerManager*> & l = mToolInputSelectionWidget->GetSelectedInputs();
  mCurrentSlicerManager = l[0];
  mCurrentImage = mCurrentSlicerManager->GetImage();
  mToolWidget->setEnabled(true);
  if (!mCurrentSlicerManager) close();
  if (l.size() == 1) InputIsSelected(mCurrentSlicerManager);
  else InputIsSelected(l);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::InputIsSelected(vvSlicerManager * m) {
  std::cerr << "You MUST overwrite this method vvToolWidgetBase::InputIsSelected(vvSlicerManager * m) if you use one single input" << std::endl;
  exit(0);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::InputIsSelected(std::vector<vvSlicerManager*> & l) {
  buttonBox->setEnabled(true);
  DD("InputIsSelected(vector)");
  DD(l.size());
  if (l.size() == 1) InputIsSelected(l[0]);
  else {
  std::cerr << "You MUST overwrite this method vvToolWidgetBase::InputIsSelected(vector<vvSlicerManager *> m) if you use several input" << std::endl;
  exit(0);
  }
}
//------------------------------------------------------------------------------
