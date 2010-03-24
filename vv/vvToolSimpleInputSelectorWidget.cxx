/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolSimpleInputSelectorWidget.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/24 10:48:18 $
  Version:   $Revision: 1.2 $
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
#include <QAbstractButton>

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
  changeInput(0);
  if (mSlicerManagerList.size() == 1) {
    if (!mAllowSkip) accept();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::SetText(QString & s) {
  mGroupBox->setTitle(s);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::EnableAllowSkip(bool b) {
  mAllowSkip = b;
  if (mAllowSkip) {
    mInputSelectionButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok|QDialogButtonBox::Discard);
    connect(mInputSelectionButtonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(skip(QAbstractButton*)));
  }
  else 
    mInputSelectionButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::SetInputList(const std::vector<vvSlicerManager*> & l, int index) {
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
  emit rejected();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::skip(QAbstractButton* b) {
  if (b->text() == "Discard") emit sigskip();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::changeInput(int index) {
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

