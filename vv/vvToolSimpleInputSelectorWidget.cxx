/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolSimpleInputSelectorWidget.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/17 11:23:46 $
  Version:   $Revision: 1.1 $
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
#include "vvSlicerManager.h"

//------------------------------------------------------------------------------
vvToolSimpleInputSelectorWidget::vvToolSimpleInputSelectorWidget(QWidget * parent, Qt::WindowFlags f):
  QWidget(parent, f) {
  setupUi(this);  
  setEnabled(true);
  mSlicerManagerList.clear();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::Initialize() {
  // Connect signals & slots  
  connect(mInputSelectionButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(mInputSelectionButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(mInputSequenceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeInput(int)));  
  DD("vvToolSimpleInputSelectorWidget::Initialize");
  DD(mSlicerManagerList.size());
  if (mSlicerManagerList.size() == 1) {
    DD("Initialize::accept");
    accept();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::SetInputList(const std::vector<vvSlicerManager*> & l, int index) {
  DD("vvToolSimpleInputSelectorWidget::SetInputList");
  DD(index);
  mInputSequenceBox->clear();
  for(unsigned int i=0; i<l.size(); i++)
    mSlicerManagerList.push_back(l[i]);
  mCurrentIndex = index;
  for (unsigned int i = 0; i < mSlicerManagerList.size(); i++) {
    mInputSequenceBox->addItem(mSlicerManagerList[i]->GetFileName().c_str());
  }
  mInputSequenceBox->setCurrentIndex(mCurrentIndex);
  if (mSlicerManagerList.size() == 0) {
    // TODO !!!
    DD("no input > error message");
    reject();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::setEnabled(bool b) {
  QWidget::setEnabled(b);
  mInputSelectionButtonBox->setEnabled(b);
  mInputSequenceBox->setEnabled(b);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::accept() {
  mInputSelectionButtonBox->setEnabled(false);
  mInputSequenceBox->setEnabled(false);
  emit accepted();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::reject() {
  DD("vvToolSimpleInputSelectorWidget::reject()");
  emit rejected();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::changeInput(int index) {
  DD("changeInput");
  DD(index);
  if (index<0) return;
  mCurrentIndex = index;
  vvImage * mCurrentImage = mSlicerManagerList[index]->GetImage();
  unsigned int d = mCurrentImage->GetNumberOfDimensions();
  QString size;
  QString spacing;
  for(unsigned int i=0; i<d-1; i++) {
    size.append(QString("%1").arg(mCurrentImage->GetSize()[i]));
    size.append("x");
    spacing.append(QString("%1").arg(mCurrentImage->GetSpacing()[i]));
    spacing.append("x");
  }
  size.append(QString("%1").arg(mCurrentImage->GetSize()[d-1]));
  spacing.append(QString("%1").arg(mCurrentImage->GetSpacing()[d-1]));
  mLabelInputInfo->setText(QString("Image: %1D %2   %3    %4")
                           .arg(d)
                           .arg(mCurrentImage->GetScalarTypeAsString().c_str())
                           .arg(size)
                           .arg(spacing));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvSlicerManager * vvToolSimpleInputSelectorWidget::GetSelectedInput() {
  return mSlicerManagerList[GetSelectedInputIndex()];
}
//------------------------------------------------------------------------------

#endif

