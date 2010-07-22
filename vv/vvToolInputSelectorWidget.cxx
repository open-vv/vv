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
#ifndef VVTOOLINPUTSELECTORWIDGET_CXX
#define VVTOOLINPUTSELECTORWIDGET_CXX

#include "vvToolSimpleInputSelectorWidget.h"
#include "vvToolInputSelectorWidget.h"
#include "vvSlicerManager.h"

//------------------------------------------------------------------------------
vvToolInputSelectorWidget::vvToolInputSelectorWidget(QWidget * parent, Qt::WindowFlags f):
  QWidget(parent, f)
{
  setupUi(this);
  setEnabled(true);
  mNumberOfAcceptedInputs = 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int vvToolInputSelectorWidget::GetNumberOfInput()
{
  return mListOfSimpleInputWidget.size();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolInputSelectorWidget::AddInputSelector(QString & s,
						 const std::vector<vvSlicerManager*> & l,
						 int index,
						 bool allowSkip)
{
  // Create a input selector widget 
  vvToolSimpleInputSelectorWidget * input = new vvToolSimpleInputSelectorWidget;
  mListOfSimpleInputWidget.push_back(input);
  mSkipInput.push_back(false);
  input->SetText(s);
  input->EnableAllowSkip(allowSkip);

  // Copy the list of slicermanager
  std::vector<vvSlicerManager*> * ll = new std::vector<vvSlicerManager*>;
  for(unsigned int i=0; i<l.size(); i++)
    ll->push_back(l[i]);

  // Add
  input->SetInputList(*ll, index);
  mVerticalLayout->addWidget(input);

  // Enable or disable
  if (GetNumberOfInput() == 1) input->setEnabled(true);
  else input->setEnabled(false);

  // Connect signals & slots
  connect(input, SIGNAL(accepted()), this, SLOT(accept()));
  connect(input, SIGNAL(rejected()), this, SLOT(reject()));
  connect(input, SIGNAL(sigskip()), this, SLOT(skip()));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolInputSelectorWidget::Initialize()
{
  for(unsigned int i=0; i<mListOfSimpleInputWidget.size(); i++)
    mListOfSimpleInputWidget[i]->Initialize();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolInputSelectorWidget::accept()
{
  mNumberOfAcceptedInputs++;
  if (mNumberOfAcceptedInputs == GetNumberOfInput()) {
    setEnabled(false);
    emit accepted();
  } else {
    mListOfSimpleInputWidget[mNumberOfAcceptedInputs]->setEnabled(true);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolInputSelectorWidget::reject()
{
  if (mNumberOfAcceptedInputs != 0)  {
    mListOfSimpleInputWidget[mNumberOfAcceptedInputs]->setEnabled(false);
    mListOfSimpleInputWidget[mNumberOfAcceptedInputs-1]->setEnabled(true);
    mNumberOfAcceptedInputs--;
  } else {
    emit rejected();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolInputSelectorWidget::skip()
{
  mSkipInput[mNumberOfAcceptedInputs] = true;
  accept();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
std::vector<vvSlicerManager*> & vvToolInputSelectorWidget::GetSelectedInputs()
{
  std::vector<vvSlicerManager*> * l = new std::vector<vvSlicerManager*>;
  for(unsigned int i=0; i<mListOfSimpleInputWidget.size(); i++) {
    if (!mSkipInput[i])
      l->push_back(mListOfSimpleInputWidget[i]->GetSelectedInput());
  }
  return *l;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolInputSelectorWidget::AnImageIsBeingClosed(vvSlicerManager * m)
{
  //  DD("TODO : verify that the image still exist !!");
  //  for(int i=0; i<
}
//------------------------------------------------------------------------------


#endif

