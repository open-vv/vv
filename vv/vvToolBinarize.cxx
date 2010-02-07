/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolBinarize.cxx,v $
  Language:  C++
  Date:      $Date: 2010/02/07 12:00:59 $
  Version:   $Revision: 1.7 $
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
vvToolBinarize::vvToolBinarize(QWidget * parent, Qt::WindowFlags f)
  :QDialog(parent,f), vvToolBase<vvToolBinarize>(), Ui::vvToolBinarize() 
{
  // Set Modality : dialog is not modal but stay always on top because
  // parent is set at construction
  setModal(false);
  setAttribute(Qt::WA_DeleteOnClose);

  // GUI Initialization
  setupUi(this);

  // Connect signals & slots  
  connect(mToolInputSelectionWidget, SIGNAL(accepted()), this, SLOT(InputIsSelected()));
  connect(mToolInputSelectionWidget, SIGNAL(rejected()), this, SLOT(close()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(apply()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
  connect(mThresholdSlider1, SIGNAL(valueChanged(double)), this, SLOT(valueChangedT1(double)));
  connect(mThresholdSlider2, SIGNAL(valueChanged(double)), this, SLOT(valueChangedT2(double)));
  connect(mRadioButtonLowerThan, SIGNAL(toggled(bool)), this, SLOT(enableLowerThan(bool)));
  connect(mCheckBoxUseFG, SIGNAL(toggled(bool)), this, SLOT(useFGBGtoggled(bool)));
  connect(mCheckBoxUseBG, SIGNAL(toggled(bool)), this, SLOT(useFGBGtoggled(bool)));

  // VTK objects
  //mImageContour = new vvImageContour;

  //new vector of contours

  // Initialize some widget
  mThresholdSlider1->SetText("");
  mThresholdSlider2->SetText("");
  mFGSlider->SetText("Foreground value");
  mBGSlider->SetText("Background value");

  // Disable main widget while input image is not selected
  toolMainWidget->setEnabled(false);

  // Main filter 
  mFilter = new clitk::BinarizeImageGenericFilter<args_info_clitkBinarizeImage>; //DS PUT IN BASECLASS ?

  // Initialize the input selection (mFilter should be create before !)
  InitializeListOfInputImages(mToolInputSelectionWidget, mFilter);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolBinarize::~vvToolBinarize() {
  //delete mImageContour;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::enableLowerThan(bool b) {
  if (!b) {
    mThresholdSlider1->resetMaximum();
  }
  else {
    valueChangedT1(mThresholdSlider1->GetValue());
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
void vvToolBinarize::InputIsSelected() {

  // Common
  int index = mToolInputSelectionWidget->GetSelectedInputIndex();
  mCurrentSliceManager = mSlicerManagersCompatible[index];
  mCurrentImage = mCurrentSliceManager->GetImage();
  toolMainWidget->setEnabled(true);

  // Specific for this gui
  mThresholdSlider1->SetValue(0);
  mThresholdSlider2->SetValue(0);
  mThresholdSlider1->SetImage(mCurrentImage);
  mThresholdSlider2->SetImage(mCurrentImage);
  mFGSlider->SetImage(mCurrentImage);
  mBGSlider->SetImage(mCurrentImage);
  DD(mCurrentSliceManager->GetFileName().c_str());
  mFGSlider->SetMaximum(mCurrentImage->GetFirstVTKImageData()->GetScalarTypeMax());
  mFGSlider->SetMinimum(mCurrentImage->GetFirstVTKImageData()->GetScalarTypeMin());
  mBGSlider->SetMaximum(mCurrentImage->GetFirstVTKImageData()->GetScalarTypeMax());
  mBGSlider->SetMinimum(mCurrentImage->GetFirstVTKImageData()->GetScalarTypeMin());
  mFGSlider->SetValue(1);
  mBGSlider->SetValue(0);
  
  // VTK objects for interactive display
  for(int i=0;i<mCurrentSliceManager->NumberOfSlicers(); i++) {
    mImageContour.push_back(new vvImageContour);
    mImageContour[i]->setSlicer(mCurrentSliceManager->GetSlicer(i));
  }
  valueChangedT1(mThresholdSlider1->GetValue());
  connect(mCurrentSliceManager,SIGNAL(UpdateSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
  connect(mCurrentSliceManager,SIGNAL(UpdateTSlice(int,int)),this,SLOT(UpdateSlice(int, int)));

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::UpdateSlice(int slicer,int slices) {
  DD(slicer);
  for(int i=0;i<mCurrentSliceManager->NumberOfSlicers(); i++) {
    mImageContour[i]->update(mThresholdSlider1->GetValue());
  }
  mCurrentSliceManager->Render(); 
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

  mArgsInfo.upper_given = 0;
  mArgsInfo.lower_given = 0;
  bool inverseBGandFG = false;

  mArgsInfo.lower_given = 1;
  mArgsInfo.lower_arg = mThresholdSlider1->GetValue();
  DD(mArgsInfo.lower_arg);
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

  DD(inverseBGandFG);
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

  mArgsInfo.verbose_flag = true;

  // Required (even if not used)
  mArgsInfo.input_given = 0;
  mArgsInfo.output_given = 0;
  
  mArgsInfo.input_arg = new char;
  mArgsInfo.output_arg = new char;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::apply() {
  GetArgsInfoFromGUI();

  // Main filter
  clitk::BinarizeImageGenericFilter<args_info_clitkBinarizeImage>::Pointer filter = 
    clitk::BinarizeImageGenericFilter<args_info_clitkBinarizeImage>::New();
  filter->SetArgsInfo(mArgsInfo);
  filter->SetInputVVImage(mCurrentImage);
  filter->Update();

  // Output ???
  vvImage::Pointer output = filter->GetOutputVVImage();
  std::ostringstream osstream;
  osstream << "Binarized_" << mCurrentSliceManager->GetSlicer(0)->GetFileName();
  CREATOR(vvToolBinarize)->mMainWindow->AddImage(output,osstream.str()); 
  close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::valueChangedT2(double v) {
  if (mRadioButtonLowerThan->isChecked()) mThresholdSlider1->SetMaximum(v);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::valueChangedT1(double v) {
  mThresholdSlider2->SetMinimum(v);
  int m1 = (int)lrint(v);

  for(int i=0;i<mCurrentSliceManager->NumberOfSlicers(); i++) {
    mImageContour[i]->update(m1);
  }
  mCurrentSliceManager->Render();
}
//------------------------------------------------------------------------------
