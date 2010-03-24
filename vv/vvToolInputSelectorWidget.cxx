/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolInputSelectorWidget.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/24 20:35:13 $
  Version:   $Revision: 1.6 $
  Author :   David Sarrut (david.sarrut@creatis.insa-lyon.fr)

  Copyright (C) 2010
  Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
  CREATIS                   http://www.creatis.insa-lyon.fr

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

#ifndef VVTOOLINPUTSELECTORWIDGET_CXX
#define VVTOOLINPUTSELECTORWIDGET_CXX

#include "vvToolSimpleInputSelectorWidget.h"
#include "vvToolInputSelectorWidget.h"
#include "vvSlicerManager.h"

//------------------------------------------------------------------------------
vvToolInputSelectorWidget::vvToolInputSelectorWidget(QWidget * parent, Qt::WindowFlags f):
  QWidget(parent, f) {
  setupUi(this);  
  setEnabled(true);
  mNumberOfAcceptedInputs = 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int vvToolInputSelectorWidget::GetNumberOfInput() {
  return mListOfSimpleInputWidget.size();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolInputSelectorWidget::AddInputSelector(QString & s, 
						 const std::vector<vvSlicerManager*> & l, 
						 int index, 
						 bool allowSkip) {
  //  DD("ICICICICICICICIC AddInputSelector ADD layout");
  //  DD(index);
  //DD(l.size());
  vvToolSimpleInputSelectorWidget * input = new vvToolSimpleInputSelectorWidget;
  mListOfSimpleInputWidget.push_back(input);
  mSkipInput.push_back(false);
  
  input->SetText(s);
  input->EnableAllowSkip(allowSkip);

  // copy list
  std::vector<vvSlicerManager*> * ll = new std::vector<vvSlicerManager*>;
  for(unsigned int i=0; i<l.size(); i++)
    ll->push_back(l[i]);

  // add
  input->SetInputList(*ll, index);
  // input->setObjectName(QString::fromUtf8("TOTO"));
  mVerticalLayout->addWidget(input);

  // Enable or disable
  if (GetNumberOfInput() == 1) input->setEnabled(true);
  else input->setEnabled(false);
  //DD(GetNumberOfInput());

  // Connect signals & slots  
  connect(input, SIGNAL(accepted()), this, SLOT(accept()));
  connect(input, SIGNAL(rejected()), this, SLOT(reject()));  
  connect(input, SIGNAL(sigskip()), this, SLOT(skip()));  
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolInputSelectorWidget::Initialize() {
  for(unsigned int i=0; i<mListOfSimpleInputWidget.size(); i++)
    mListOfSimpleInputWidget[i]->Initialize();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolInputSelectorWidget::accept() {
  //  DD("vvToolInputSelectorWidget::accept");
  //DD(mNumberOfAcceptedInputs);
  mNumberOfAcceptedInputs++;
  if (mNumberOfAcceptedInputs == GetNumberOfInput()) {
    setEnabled(false);
    emit accepted();
  }
  else {
    //DD("accepted");
    //    for(unsigned int i=mNumberOfAcceptedInputs; i<mListOfSimpleInputWidget.size(); i++) {
    //      mListOfSimpleInputWidget[i]->Initialize();
    mListOfSimpleInputWidget[mNumberOfAcceptedInputs]->setEnabled(true);
    //}
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolInputSelectorWidget::reject() {
  //  DD("vvToolInputSelectorWidget::reject");
  if (mNumberOfAcceptedInputs != 0)  {
    //    for(unsigned int i=mNumberOfAcceptedInputs; i<mListOfSimpleInputWidget.size(); i++) {
    //      mListOfSimpleInputWidget[i]->Initialize();
    //    DD(mNumberOfAcceptedInputs);
    mListOfSimpleInputWidget[mNumberOfAcceptedInputs]->setEnabled(false);
    mListOfSimpleInputWidget[mNumberOfAcceptedInputs-1]->setEnabled(true);
    mNumberOfAcceptedInputs--;
    //}
  }
  else {
    emit rejected();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolInputSelectorWidget::skip() {
  //  DD("SKIP");
  mSkipInput[mNumberOfAcceptedInputs] = true;
  accept();//mNumberOfAcceptedInputs++;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
std::vector<vvSlicerManager*> & vvToolInputSelectorWidget::GetSelectedInputs() {
  std::vector<vvSlicerManager*> * l = new std::vector<vvSlicerManager*>;
  for(unsigned int i=0; i<mListOfSimpleInputWidget.size(); i++) {
    if (!mSkipInput[i])
      l->push_back(mListOfSimpleInputWidget[i]->GetSelectedInput());
  }
  return *l;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolInputSelectorWidget::AnImageIsBeingClosed(vvSlicerManager * m) {
  //  DD("TODO : verify that the image still exist !!");
  //  for(int i=0; i<
}
//------------------------------------------------------------------------------


#endif

