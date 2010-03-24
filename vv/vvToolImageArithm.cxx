/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolImageArithm.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/24 13:02:59 $
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

#include "vvToolImageArithm.h"
#include "vvSlicer.h"
#include "clitkImageArithmGenericFilter.h"
#include <QMessageBox>

//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolImageArithm);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vvToolImageArithm::vvToolImageArithm(vvMainWindowBase * parent, Qt::WindowFlags f)
  :vvToolWidgetBase(parent, f), 
   vvToolBase<vvToolImageArithm>(parent), 
   Ui::vvToolImageArithm() {
  // Setup the UI
  Ui_vvToolImageArithm::setupUi(mToolWidget);

  // Main filter 
  mFilter = new clitk::ImageArithmGenericFilter<args_info_clitkImageArithm>;

  // Set how many inputs are needed for this tool
  AddInputSelector("Select first image (A)", mFilter);
  AddInputSelector("Select second image (B)", mFilter, true);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolImageArithm::~vvToolImageArithm() {
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolImageArithm::Initialize() {
  SetToolName("ImageArithm");
  SetToolMenuName("ImageArithm");
  SetToolIconFilename(":/new/prefix1/icons/cross.png");
  SetToolTip("Perform simple arithmetic operations on one or two images.");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolImageArithm::InputIsSelected(std::vector<vvSlicerManager *> & l) {
  mInput1 = l[0];
  mInput2 = l[1];  
  mTwoInputs = true;
  mGroupBoxOneInput->setEnabled(false);
  mGroupBoxTwoInputs->setEnabled(true);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolImageArithm::InputIsSelected(vvSlicerManager * l) {
  mInput1 = l;
  mTwoInputs = false;
  // DD("Single input");
  mGroupBoxTwoInputs->setEnabled(false);
  mGroupBoxOneInput->setEnabled(true);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolImageArithm::GetArgsInfoFromGUI() {
  //  DD("GetArgsInfoFromGUI");
  mArgsInfo.input1_given = false;
  if (mTwoInputs) {
    mArgsInfo.input2_given = true;
    mArgsInfo.input2_arg = new char; // need to indicate that there are two inputs
    mArgsInfo.scalar_given = false;
    if (radioButtonSum->isChecked()) mArgsInfo.operation_arg = 0;
    if (radioButtonMultiply->isChecked()) mArgsInfo.operation_arg = 1;
    if (radioButtonDivide->isChecked()) mArgsInfo.operation_arg = 2;
    if (radioButtonMax->isChecked()) mArgsInfo.operation_arg = 3;
    if (radioButtonMin->isChecked()) mArgsInfo.operation_arg = 4;
    if (radioButtonAbsDiff->isChecked()) mArgsInfo.operation_arg = 5;
    if (radioButtonSquaredDiff->isChecked()) mArgsInfo.operation_arg = 6;
  }
  else {
    mArgsInfo.input2_given = false;    
    mArgsInfo.scalar_given = true;
    if (radioButtonSumV->isChecked()) mArgsInfo.operation_arg = 0;
    if (radioButtonMultiplyV->isChecked()) mArgsInfo.operation_arg = 1;
    if (radioButtonInverseV->isChecked()) mArgsInfo.operation_arg = 2;
    if (radioButtonMaxV->isChecked()) mArgsInfo.operation_arg = 3;
    if (radioButtonMinV->isChecked()) mArgsInfo.operation_arg = 4;
    if (radioButtonAbsDiffV->isChecked()) mArgsInfo.operation_arg = 5;
    if (radioButtonSquaredDiffV->isChecked()) mArgsInfo.operation_arg = 6;
    if (radioButtonLogV->isChecked()) mArgsInfo.operation_arg = 7;
    if (radioButtonExpV->isChecked()) mArgsInfo.operation_arg = 8;
    if (radioButtonSqrtV->isChecked()) mArgsInfo.operation_arg = 9;
    mArgsInfo.scalar_given = true;
    mArgsInfo.scalar_arg = mValueSpinBox->value();
  }
  mArgsInfo.output_given = false;
  mArgsInfo.verbose_flag = false;  
  mArgsInfo.setFloatOutput_flag = mCheckBoxUseFloatOutputType->isChecked();
  mArgsInfo.imagetypes_flag = false;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolImageArithm::apply() {
  if (!mCurrentSlicerManager) close();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  GetArgsInfoFromGUI();

  std::vector<vvImage::Pointer> inputs;
  if (mTwoInputs) {
    // Input
    inputs.push_back(mInput1->GetImage());
    inputs.push_back(mInput2->GetImage());
    
    // Check input type
    if (inputs[0]->GetScalarTypeAsString() != inputs[1]->GetScalarTypeAsString()) {
      std::cerr << "Sorry inputs should have the same pixeltype." << std::endl;
      std::cerr << "Input1 = " << inputs[0]->GetScalarTypeAsString() << std::endl;
      std::cerr << "Input2 = " << inputs[1]->GetScalarTypeAsString() << std::endl;
      QApplication::restoreOverrideCursor();
      QMessageBox::information(this, "Wrong image type","Sorry, could not perform operation. Please select inputs with same pixe type.");
      close();
      return;
    }
  }   
  else {
    // Input
    inputs.push_back(mInput1->GetImage());
    DD("Single input");
  }
   
  // Main filter
  clitk::ImageArithmGenericFilter<args_info_clitkImageArithm>::Pointer filter = 
    clitk::ImageArithmGenericFilter<args_info_clitkImageArithm>::New();
  filter->SetInputVVImages(inputs);
  filter->SetArgsInfo(mArgsInfo);
  filter->EnableReadOnDisk(false);
  filter->EnableOverwriteInputImage(false);
  filter->Update();
    
  // Output
  vvImage::Pointer output = filter->GetOutputVVImage();
  std::ostringstream osstream;
  osstream << "Arithm_" << mArgsInfo.operation_arg << "_ " 
           << mCurrentSlicerManager->GetSlicer(0)->GetFileName() << ".mhd";
  AddImage(output,osstream.str()); 
  QApplication::restoreOverrideCursor();
  close();
}
//------------------------------------------------------------------------------
