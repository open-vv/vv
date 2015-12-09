/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  ===========================================================================**/

// vv
#include "vvToolBinarize.h"
#include "vvSlicerManager.h"
#include "vvSlicer.h"
#include "vvToolInputSelectorWidget.h"

// clitk
#include "clitkBinarizeImageGenericFilter.h"

// vtk
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkImageClip.h>
#include <vtkRenderWindow.h>


//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolBinarize);
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::Initialize()
{ 
  SetToolName("Binarize");
  SetToolMenuName("Binarize");
  SetToolIconFilename(":/common/icons/binarize.png");
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
  mFilter = clitk::BinarizeImageGenericFilter::New();

  // Set how many inputs are needed for this tool
  AddInputSelector("Select one image", mFilter);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolBinarize::~vvToolBinarize()
{ 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::InteractiveDisplayToggled(bool b)
{ 
  mInteractiveDisplayIsEnabled = b;
  if (!mInteractiveDisplayIsEnabled) {
    RemoveVTKObjects();
  } else {
    for(unsigned int i=0; i<mImageContour.size(); i++) {
      mImageContour[i]->ShowActors();
      if (mRadioButtonLowerThan->isChecked())
        mImageContourLower[i]->ShowActors();
    }
    if (mCurrentSlicerManager)
      mCurrentSlicerManager->Render();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::RemoveVTKObjects()
{ 
  for(unsigned int i=0; i<mImageContour.size(); i++) {
    mImageContour[i]->HideActors();
    mImageContourLower[i]->HideActors();    
  }
  if (mCurrentSlicerManager)
    mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolBinarize::close()
{ 
  // RemoveVTKObjects();
  return vvToolWidgetBase::close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::closeEvent(QCloseEvent *event)
{ 
  RemoveVTKObjects();
  event->accept();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::reject()
{ 
  // DD("vvToolBinarize::reject");
  RemoveVTKObjects();
  return vvToolWidgetBase::reject();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::enableLowerThan(bool b)
{ 
  if (!b) {
    mThresholdSlider1->resetMaximum();
    for(unsigned int i=0; i<mImageContour.size(); i++) {
      mImageContourLower[i]->HideActors();    
    }
    mCurrentSlicerManager->Render();
  } else {
    valueChangedT1(mThresholdSlider1->GetValue());
    valueChangedT2(mThresholdSlider2->GetValue());
    for(unsigned int i=0; i<mImageContour.size(); i++) {
      mImageContourLower[i]->ShowActors();    
    }
    mCurrentSlicerManager->Render();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::useFGBGtoggled(bool)
{ 
  if (!mCheckBoxUseBG->isChecked() && !mCheckBoxUseFG->isChecked())
    mCheckBoxUseBG->toggle();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void vvToolBinarize::InputIsSelected(std::vector<vvSlicerManager *> & m) {
//   DD("vvToolBinarize::InputIsSelected vector in binarize");
//   DD(m.size());
// }
void vvToolBinarize::InputIsSelected(vvSlicerManager * m)
{ 
  mCurrentSlicerManager = m;

  // Specific for this gui
  mThresholdSlider1->SetValue(0);
  mThresholdSlider2->SetValue(0);
  mThresholdSlider1->SetImage(mCurrentImage);
  mThresholdSlider2->SetImage(mCurrentImage);
  mFGSlider->SetImage(mCurrentImage);
  mBGSlider->SetImage(mCurrentImage);
  //  DD(mCurrentSlicerManager->GetFileName().c_str());
  //  mFGSlider->SetMaximum(mCurrentImage->GetFirstVTKImageData()->GetScalarTypeMax());
  //   mFGSlider->SetMinimum(mCurrentImage->GetFirstVTKImageData()->GetScalarTypeMin());
  //   mBGSlider->SetMaximum(mCurrentImage->GetFirstVTKImageData()->GetScalarTypeMax());
  //   mBGSlider->SetMinimum(mCurrentImage->GetFirstVTKImageData()->GetScalarTypeMin());

  // Output is uchar ...
  mFGSlider->SetMaximum(255);
  mFGSlider->SetMinimum(0);
  mBGSlider->SetMaximum(255);
  mBGSlider->SetMinimum(0);

  mFGSlider->SetValue(1);
  mBGSlider->SetValue(0);
  mFGSlider->SetSingleStep(1);
  mBGSlider->SetSingleStep(1);

  // VTK objects for interactive display
  for(int i=0; i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
    mImageContour.push_back(vvImageContour::New());
    mImageContour[i]->SetSlicer(mCurrentSlicerManager->GetSlicer(i));
    mImageContour[i]->SetColor(1.0, 0.0, 0.0);
    mImageContour[i]->SetDepth(0); // to be in front of (whe used with ROI tool)
    mImageContourLower.push_back(vvImageContour::New());
    mImageContourLower[i]->SetSlicer(mCurrentSlicerManager->GetSlicer(i));
    mImageContourLower[i]->SetColor(0.0, 0.0, 1.0);
    mImageContourLower[i]->SetDepth(100); // to be in front of (whe used with ROI tool)
  }
  valueChangedT1(mThresholdSlider1->GetValue());

  connect(mThresholdSlider1, SIGNAL(valueChanged(double)), this, SLOT(valueChangedT1(double)));
  connect(mThresholdSlider2, SIGNAL(valueChanged(double)), this, SLOT(valueChangedT2(double)));

  connect(mCurrentSlicerManager,SIGNAL(UpdateSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
  connect(mCurrentSlicerManager,SIGNAL(UpdateTSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
  
  connect(mCurrentSlicerManager,SIGNAL(UpdateOrientation(int,int)),this,SLOT(UpdateOrientation(int, int)));

  //  connect(mCurrentSlicerManager, SIGNAL(LeftButtonReleaseSignal(int)), SLOT(LeftButtonReleaseEvent(int)));
  InteractiveDisplayToggled(mInteractiveDisplayIsEnabled);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void vvToolBinarize::LeftButtonReleaseEvent(int slicer) {
//   DD("LeftButtonReleaseEvent");
//   for(int i=0; i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
//     if (i == slicer);
//     mCurrentSlicerManager->GetSlicer(i)->GetRenderWindow()->Render();
//   }
// }
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::UpdateOrientation(int slicer,int orientation)
{ 
  Update(slicer);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolBinarize::UpdateSlice(int slicer,int slices)
{ 
  Update(slicer);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolBinarize::Update(int slicer)
{ 
  if (!mInteractiveDisplayIsEnabled) return;
  if (!mCurrentSlicerManager) close();
  mImageContour[slicer]->Update(mThresholdSlider1->GetValue());
  if (mRadioButtonLowerThan->isChecked()) 
    mImageContourLower[slicer]->Update(mThresholdSlider2->GetValue());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolBinarize::GetArgsInfoFromGUI()
{ 

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
  cmdline_parser_clitkBinarizeImage_init(&mArgsInfo); // Initialisation to default
  bool inverseBGandFG = false;

  mArgsInfo.lower_given = 1;
  mArgsInfo.lower_arg = mThresholdSlider1->GetValue();
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
  } else mArgsInfo.mode_arg = (char*)"FG";

  mArgsInfo.verbose_flag = false;

  // // Required (even if not used)
  // mArgsInfo.input_given = 0;
  // mArgsInfo.output_given = 0;

  // mArgsInfo.input_arg = new char;
  // mArgsInfo.output_arg = new char;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::apply()
{ 
  if (!mCurrentSlicerManager) close();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  GetArgsInfoFromGUI();  
  
  // Main filter
  clitk::BinarizeImageGenericFilter::Pointer filter =
    clitk::BinarizeImageGenericFilter::New();
  filter->SetInputVVImage(mCurrentImage);
  filter->SetArgsInfo(mArgsInfo);
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
void vvToolBinarize::valueChangedT2(double v)
{ 
  //  DD("valueChangedT2");
  if (mRadioButtonLowerThan->isChecked()) {
    mThresholdSlider1->SetMaximum(v);
    if (!mInteractiveDisplayIsEnabled) return;
    for(int i=0;i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
      mImageContourLower[i]->Update(v);
    }
    mCurrentSlicerManager->Render();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::valueChangedT1(double v)
{ 
  //  DD("valueChangedT1");
  if (!mCurrentSlicerManager) close();
  mThresholdSlider2->SetMinimum(v);
  //  int m1 = (int)lrint(v);
  if (!mInteractiveDisplayIsEnabled) return;
  for(int i=0;i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
    mImageContour[i]->Update(v);
  }
  mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------
