/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/

#include "vvToolWidgetBase.h"
#include "vvMainWindowBase.h"
#include "vvSlicerManager.h"
#include <QMessageBox>

//------------------------------------------------------------------------------
vvToolWidgetBase::vvToolWidgetBase(vvMainWindowBase * parent, Qt::WindowFlags f)
  :QDialog(parent, f),
   Ui::vvToolWidgetBase()
{

  // Set Modality : dialog is not modal but stay always on top because
  // parent is set at construction
  mIsInitialized = false;
  mFilter = 0;
  mMainWindow = parent;
  setModal(false);
  setAttribute(Qt::WA_DeleteOnClose);
  mCurrentSlicerManager = 0;

  // GUI Initialization
  setupUi(this);

  // Connect signals & slots
  connect(mMainWindow, SIGNAL(AnImageIsBeingClosed(vvSlicerManager*)),
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
vvToolWidgetBase::vvToolWidgetBase(vvMainWindowBase * parent, Qt::WindowFlags f, bool b):
  QDialog(NULL, f), 
  Ui::vvToolWidgetBase()
{
  DD("const without qdialog");
  // TRIAL 

  mIsInitialized = false;
  mFilter = 0;
  mMainWindow = parent;
  //  setModal(false);
  //setAttribute(Qt::WA_DeleteOnClose);
  mCurrentSlicerManager = 0;

  // GUI Initialization
  setupUi(this);

  // Connect signals & slots
  connect(mMainWindow, SIGNAL(AnImageIsBeingClosed(vvSlicerManager*)),
          this, SLOT(AnImageIsBeingClosed(vvSlicerManager*)));
  connect(mToolInputSelectionWidget, SIGNAL(accepted()), this, SLOT(InputIsSelected()));
  connect(mToolInputSelectionWidget, SIGNAL(rejected()), this, SLOT(close()));
  //connect(buttonBox, SIGNAL(accepted()), this, SLOT(apply()));
  //connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

  // Disable main widget while input image is not selected
  mToolWidget->setEnabled(false);
  buttonBox->hide();
  hide();
  DD("end const");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolWidgetBase::~vvToolWidgetBase()
{

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::AddInputSelector(QString s, clitk::ImageToImageGenericFilterBase * f, bool allowSkip)
{
  int j=0;
  mFilter = f;
  mSlicerManagersCompatible.clear();
  mToolInputSelectionWidget->setToolTip(QString("%1").arg(mFilter->GetAvailableImageTypes().c_str()));
  for(unsigned int i=0; i<mMainWindow->GetSlicerManagers().size(); i++) {
    vvImage * s = mMainWindow->GetSlicerManagers()[i]->GetImage();
    if (mFilter->CheckImageType(s->GetNumberOfDimensions(),
                                s->GetNumberOfScalarComponents(),
                                s->GetScalarTypeAsString())) {
      mSlicerManagersCompatible.push_back(mMainWindow->GetSlicerManagers()[i]);
      if ((int)i == mMainWindow->GetSlicerManagerCurrentIndex()) mCurrentCompatibleIndex = j;
      j++;
    }
  }
  if (mSlicerManagersCompatible.size() == 0) {
    QMessageBox::information(this, "No image","Sorry, could not perform operation. No (compatible) opened image type.");
    close();
    return;
  }
  mToolInputSelectionWidget->AddInputSelector(s, mSlicerManagersCompatible, mCurrentCompatibleIndex, allowSkip);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::AddInputSelector(QString s, bool allowSkip)
{
  mSlicerManagersCompatible.clear();
  for(unsigned int i=0; i<mMainWindow->GetSlicerManagers().size(); i++) {
    mSlicerManagersCompatible.push_back(mMainWindow->GetSlicerManagers()[i]);
  }
  if (mMainWindow->GetSlicerManagers().size() == 0) {
    QMessageBox::information(this, "No image","Sorry, could not perform operation. No opened image type.");
    close();
    return;
  }
  mToolInputSelectionWidget->AddInputSelector(s, mMainWindow->GetSlicerManagers(),
      mMainWindow->GetSlicerManagerCurrentIndex(), allowSkip);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::HideInputSelector()
{
  QList<int> s;
  s.push_back(0);
  s.push_back(1);
  splitter->setSizes(s);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::show()
{
  if (!mIsInitialized) {
    mToolInputSelectionWidget->Initialize();
    mIsInitialized = true;
  }
  QDialog::show();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolWidgetBase::close()
{
  return QDialog::close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::AnImageIsBeingClosed(vvSlicerManager * m)
{
  mToolInputSelectionWidget->AnImageIsBeingClosed(m);
  if (m == mCurrentSlicerManager) {
    close();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::InitializeInputs()
{
  /*
    if (mFilter) {
    int j=0;
    mToolInputSelectionWidget->setToolTip(QString("%1").arg(mFilter->GetAvailableImageTypes().c_str()));
    for(unsigned int i=0; i<mMainWindow->GetSlicerManagers().size(); i++) {
    vvImage * s = mMainWindow->GetSlicerManagers()[i]->GetImage();
    if (mFilter->CheckImageType(s->GetNumberOfDimensions(),
    s->GetNumberOfScalarComponents(),
    s->GetScalarTypeAsString())) {
    mSlicerManagersCompatible.push_back(mMainWindow->GetSlicerManagers()[i]);
    if ((int)i == mMainWindow->GetSlicerManagerCurrentIndex()) mCurrentCompatibleIndex = j;
    j++;
    }
    }
    }
    else {
    mSlicerManagersCompatible = mMainWindow->GetSlicerManagers();
    mCurrentCompatibleIndex = mMainWindow->GetSlicerManagerCurrentIndex();
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
void vvToolWidgetBase::InputIsSelected()
{
  // DD("InputIsSelected");
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
void vvToolWidgetBase::InputIsSelected(vvSlicerManager * m)
{
  std::cerr << "You MUST overwrite this method vvToolWidgetBase::InputIsSelected(vvSlicerManager * m) if you use one single input" << std::endl;
  exit(0);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::InputIsSelected(std::vector<vvSlicerManager*> & l)
{
  buttonBox->setEnabled(true);
  // DD("InputIsSelected(vector)");
  //   DD(l.size());
  if (l.size() == 1) InputIsSelected(l[0]);
  else {
    std::cerr << "You MUST overwrite this method vvToolWidgetBase::InputIsSelected(vector<vvSlicerManager *> m) if you use several input" << std::endl;
    exit(0);
  }
}
//------------------------------------------------------------------------------
