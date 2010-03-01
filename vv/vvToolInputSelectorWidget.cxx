/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolInputSelectorWidget.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/01 15:38:09 $
  Version:   $Revision: 1.3 $
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

#include "vvToolInputSelectorWidget.h"
#include "vvSlicerManager.h"

//------------------------------------------------------------------------------
vvToolInputSelectorWidget::vvToolInputSelectorWidget(QWidget * parent, Qt::WindowFlags f):
  QWidget(parent, f) {
  setupUi(this);  

  setEnabled(true);

  // Connect signals & slots  
  connect(mInputSelectionButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(mInputSelectionButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(mInputSequenceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeInput(int)));  
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolInputSelectorWidget::Initialize(std::vector<vvSlicerManager*> l, int index) {
  mInputSequenceBox->clear();
  mSlicerManagerList = l;
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
  if (mSlicerManagerList.size() == 1) {
    accept();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolInputSelectorWidget::accept() {
  mInputSelectionButtonBox->setEnabled(false);
  mInputSequenceBox->setEnabled(false);
  emit accepted();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolInputSelectorWidget::reject() {
  emit rejected();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolInputSelectorWidget::changeInput(int index) {
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

#endif

