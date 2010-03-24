/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolBinarize.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/24 10:48:18 $
  Version:   $Revision: 1.13 $
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

#include "vvToolBinarize.h"
#include "vvSlicerManager.h"
#include "vvSlicer.h"
#include "vvToolInputSelectorWidget.h"

#include "clitkBinarizeImageGenericFilter.h"

#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkImageClip.h>

//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolBinarize);
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::Initialize() {
  SetToolName("Binarize");
  SetToolMenuName("Binarize");
  SetToolIconFilename(":/new/prefix1/icons/binarize.png");
  SetToolTip("Image interactive binarization with thresholds.");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolBinarize::vvToolBinarize(vvMainWindowBase * parent, Qt::WindowFlags f)
  :vvToolWidgetBase(parent,f), 
   vvToolBase<vvToolBinarize>(parent), 
   Ui::vvToolBinarize() 
{
  // GUI Initialization
  Ui_vvToolBinarize::setupUi(mToolWidget);
  mInteractiveDisplayIsEnabled = mCheckBoxInteractiveDisplay->isChecked();

  // Connect signals & slots  
  connect(mRadioButtonLowerThan, SIGNAL(toggled(bool)), this, SLOT(enableLowerThan(bool)));
  connect(mCheckBoxUseFG, SIGNAL(toggled(bool)), this, SLOT(useFGBGtoggled(bool)));
  connect(mCheckBoxUseBG, SIGNAL(toggled(bool)), this, SLOT(useFGBGtoggled(bool)));
  connect(mCheckBoxInteractiveDisplay, SIGNAL(toggled(bool)), this, SLOT(InteractiveDisplayToggled(bool)));

  // Initialize some widget
  mThresholdSlider1->SetText("");
  mThresholdSlider2->SetText("");
  mFGSlider->SetText("Foreground value");
  mBGSlider->SetText("Background value");

  // Main filter 
  mFilter = new clitk::BinarizeImageGenericFilter<args_info_clitkBinarizeImage>;

  // Set how many inputs are needed for this tool
  AddInputSelector("Select one image", mFilter);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolBinarize::~vvToolBinarize() {
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::InteractiveDisplayToggled(bool b) {
  mInteractiveDisplayIsEnabled = b;
  if (!mInteractiveDisplayIsEnabled) {
    RemoveVTKObjects();
  }
  else {
    for(unsigned int i=0; i<mImageContour.size(); i++) {
      mImageContour[i]->showActors();
      if (mRadioButtonLowerThan->isChecked())
        mImageContourLower[i]->showActors();
    }
    if (mCurrentSlicerManager)
      mCurrentSlicerManager->Render();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::RemoveVTKObjects() { 
  for(unsigned int i=0; i<mImageContour.size(); i++) {
    mImageContour[i]->hideActors();
    mImageContourLower[i]->hideActors();    
  }
  if (mCurrentSlicerManager)
    mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolBinarize::close() { 
  RemoveVTKObjects();
  return vvToolWidgetBase::close(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::reject() { 
  // DD("vvToolBinarize::reject");
  RemoveVTKObjects();
  return vvToolWidgetBase::reject(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::enableLowerThan(bool b) {
  if (!b) {
    mThresholdSlider1->resetMaximum();
    for(unsigned int i=0; i<mImageContour.size(); i++) {
      mImageContourLower[i]->hideActors();    
    }
    mCurrentSlicerManager->Render();
  }
  else {
    valueChangedT1(mThresholdSlider1->GetValue());
    valueChangedT2(mThresholdSlider2->GetValue());
    for(unsigned int i=0; i<mImageContour.size(); i++) {
      mImageContourLower[i]->showActors();    
    }
    mCurrentSlicerManager->Render();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::useFGBGtoggled(bool) {
  if (!mCheckBoxUseBG->isChecked() && !mCheckBoxUseFG->isChecked()) 
    mCheckBoxUseBG->toggle();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void vvToolBinarize::InputIsSelected(std::vector<vvSlicerManager *> & m) {
//   DD("vvToolBinarize::InputIsSelected vector in binarize");
//   DD(m.size());
// }
void vvToolBinarize::InputIsSelected(vvSlicerManager * m) {
  mCurrentSlicerManager = m;

  // Specific for this gui
  mThresholdSlider1->SetValue(0);
  mThresholdSlider2->SetValue(0);
  mThresholdSlider1->SetImage(mCurrentImage);
  mThresholdSlider2->SetImage(mCurrentImage);
  mFGSlider->SetImage(mCurrentImage);
  mBGSlider->SetImage(mCurrentImage);
  //  DD(mCurrentSlicerManager->GetFileName().c_str());
  mFGSlider->SetMaximum(mCurrentImage->GetFirstVTKImageData()->GetScalarTypeMax());
  mFGSlider->SetMinimum(mCurrentImage->GetFirstVTKImageData()->GetScalarTypeMin());
  mBGSlider->SetMaximum(mCurrentImage->GetFirstVTKImageData()->GetScalarTypeMax());
  mBGSlider->SetMinimum(mCurrentImage->GetFirstVTKImageData()->GetScalarTypeMin());
  mFGSlider->SetValue(1);
  mBGSlider->SetValue(0);
  mFGSlider->SetSingleStep(1);
  mBGSlider->SetSingleStep(1);
  
  // VTK objects for interactive display
  for(int i=0;i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
    mImageContour.push_back(new vvImageContour);
    mImageContour[i]->setSlicer(mCurrentSlicerManager->GetSlicer(i));
    mImageContour[i]->setColor(1.0, 0.0, 0.0);
    mImageContourLower.push_back(new vvImageContour);
    mImageContourLower[i]->setSlicer(mCurrentSlicerManager->GetSlicer(i));
    mImageContourLower[i]->setColor(0.0, 0.0, 1.0);
  }
  valueChangedT1(mThresholdSlider1->GetValue());

  connect(mThresholdSlider1, SIGNAL(valueChanged(double)), this, SLOT(valueChangedT1(double)));
  connect(mThresholdSlider2, SIGNAL(valueChanged(double)), this, SLOT(valueChangedT2(double)));

  connect(mCurrentSlicerManager,SIGNAL(UpdateSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
  connect(mCurrentSlicerManager,SIGNAL(UpdateTSlice(int,int)),this,SLOT(UpdateSlice(int, int)));

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::UpdateSlice(int slicer,int slices) {
  if (!mInteractiveDisplayIsEnabled) return;
  if (!mCurrentSlicerManager) close();
  for(int i=0;i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
    mImageContour[i]->update(mThresholdSlider1->GetValue());
    if (mRadioButtonLowerThan->isChecked()) 
      mImageContourLower[i]->update(mThresholdSlider2->GetValue());

  }
  mCurrentSlicerManager->Render(); 
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolBinarize::GetArgsInfoFromGUI() {

  /* //KEEP THIS FOR READING GGO FROM FILE
    int argc=1;
    std::string a = "toto";
    char * const* argv = new char*;
    //a.c_str();
    struct cmdline_parser_params p;
    p.check_required = 0;
    int good = cmdline_parser_ext(argc, argv, &args_info, &p);
  DD(good);
  */

  mArgsInfo.imagetypes_flag = 0;
  mArgsInfo.upper_given = 0;
  mArgsInfo.lower_given = 0;
  bool inverseBGandFG = false;

  mArgsInfo.lower_given = 1;
  mArgsInfo.lower_arg = mThresholdSlider1->GetValue();
  // DD(mArgsInfo.lower_arg);
  if (mRadioButtonLowerThan->isChecked()) {
    mArgsInfo.upper_given = 1;
    mArgsInfo.upper_arg = mThresholdSlider2->GetValue();
    if (mArgsInfo.upper_arg<mArgsInfo.lower_arg) {
      mArgsInfo.upper_given = 0;
      DD("TODO : lower thres greater than greater thres ! Ignoring ");
    }
  }

  mArgsInfo.fg_arg = mFGSlider->GetValue();
  mArgsInfo.bg_arg = mBGSlider->GetValue();

  if (inverseBGandFG) {
    mArgsInfo.fg_arg = mFGSlider->GetValue();
    mArgsInfo.bg_arg = mBGSlider->GetValue();
  }
  mArgsInfo.fg_given = 1;
  mArgsInfo.bg_given = 1;

  if (mCheckBoxUseBG->isChecked()) {
    if (mCheckBoxUseFG->isChecked()) mArgsInfo.mode_arg = (char*)"both";
    else mArgsInfo.mode_arg = (char*)"BG";
  }
  else mArgsInfo.mode_arg = (char*)"FG";

  mArgsInfo.verbose_flag = false;

  // Required (even if not used)
  mArgsInfo.input_given = 0;
  mArgsInfo.output_given = 0;
  
  mArgsInfo.input_arg = new char;
  mArgsInfo.output_arg = new char;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::apply() {
  if (!mCurrentSlicerManager) close();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  GetArgsInfoFromGUI();

  // Main filter
  clitk::BinarizeImageGenericFilter<args_info_clitkBinarizeImage>::Pointer filter = 
     clitk::BinarizeImageGenericFilter<args_info_clitkBinarizeImage>::New();
  filter->SetInputVVImage(mCurrentImage);
  filter->SetArgsInfo(mArgsInfo);
  filter->EnableReadOnDisk(false);
  filter->Update();

  // Output
  vvImage::Pointer output = filter->GetOutputVVImage();
  std::ostringstream osstream;
  osstream << "Binarized_" << mCurrentSlicerManager->GetSlicer(0)->GetFileName() << ".mhd";
  AddImage(output,osstream.str()); 
  QApplication::restoreOverrideCursor();
  close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::valueChangedT2(double v) {
  //  DD("valueChangedT2");
  if (mRadioButtonLowerThan->isChecked()) {
    mThresholdSlider1->SetMaximum(v);
    if (!mInteractiveDisplayIsEnabled) return;
    for(int i=0;i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
      mImageContourLower[i]->update(v);
    }
    mCurrentSlicerManager->Render();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::valueChangedT1(double v) {
  //  DD("valueChangedT1");
  if (!mCurrentSlicerManager) close();
  mThresholdSlider2->SetMinimum(v);
  //  int m1 = (int)lrint(v);  
  if (!mInteractiveDisplayIsEnabled) return;
  for(int i=0;i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
    mImageContour[i]->update(v);
  }
  mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------
