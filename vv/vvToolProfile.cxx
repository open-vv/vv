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
#include "vvToolProfile.h"
#include "vvSlicerManager.h"
#include "vvSlicer.h"
#include "vvToolInputSelectorWidget.h"

// clitk
#include "clitkProfileImageGenericFilter.h"

// vtk
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkImageClip.h>
#include <vtkRenderWindow.h>


//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolProfile);
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::Initialize()
{ 
  SetToolName("Profile");
  SetToolMenuName("Intensity Profile");
  SetToolIconFilename(":/common/icons/profile.png");
  SetToolTip("Display the intensity profile between 2 points of the image.");
  SetToolExperimental(false);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolProfile::vvToolProfile(vvMainWindowBase * parent, Qt::WindowFlags f)
  :vvToolWidgetBase(parent,f),
   vvToolBase<vvToolProfile>(parent),
   Ui::vvToolProfile()
{ 
  // GUI Initialization
  Ui_vvToolProfile::setupUi(mToolWidget);
  //mInteractiveDisplayIsEnabled = mCheckBoxInteractiveDisplay->isChecked();

  // Connect signals & slots
  connect(mSelectPoint1Button, SIGNAL(clicked()), this, SLOT(selectPoint1()));
  connect(mSelectPoint2Button, SIGNAL(clicked()), this, SLOT(selectPoint2()));
  connect(mCancelPointsButton, SIGNAL(clicked()), this, SLOT(cancelPoints()));
  connect(mComputeProfileButton, SIGNAL(clicked()), this, SLOT(computeProfile()));

  // Initialize some widget
  //mThresholdSlider1->SetText("");
  //mThresholdSlider2->SetText("");
  //mFGSlider->SetText("Foreground value");
  //mBGSlider->SetText("Background value");
  
  mPoint1 = NULL;
  mPoint2 = NULL;
  
  point1Selected = false;
  point2Selected = false;

  // Main filter
  mFilter = clitk::ProfileImageGenericFilter::New();

  // Set how many inputs are needed for this tool
  AddInputSelector("Select one image", mFilter);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolProfile::~vvToolProfile()
{ 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::selectPoint1()
{
  QString position = "";
  point1Selected = false;
  if(mCurrentSlicerManager) {
      if(mCurrentSlicerManager->GetSelectedSlicer() != -1) {
          double *pos;
          int *index;
          pos = new double [mCurrentSlicerManager->GetImage()->GetNumberOfDimensions()];
          index = new int [mCurrentSlicerManager->GetImage()->GetNumberOfDimensions()];
          
          for (int i=0; i<mCurrentSlicerManager->GetImage()->GetNumberOfDimensions(); ++i) {
            pos[i] = mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetCursorPosition()[i];
            index[i] = (int) (pos[i] - mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[i])/mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[i];
            
            position += QString::number(pos[i],'f',1) + " ";
            mPoint1[i] = index[i];
          }
          point1Selected = true;
      }
  }
  mPosPoint1Label->setText(position);
  isPointsSelected();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::selectPoint2()
{
  QString position = "";
  point2Selected = false;
  if(mCurrentSlicerManager) {
      if(mCurrentSlicerManager->GetSelectedSlicer() != -1) {
          double *pos;
          int *index;
          pos = new double [mCurrentSlicerManager->GetImage()->GetNumberOfDimensions()];
          index = new int [mCurrentSlicerManager->GetImage()->GetNumberOfDimensions()];
          
          for (int i=0; i<mCurrentSlicerManager->GetImage()->GetNumberOfDimensions(); ++i) {
            pos[i] = mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetCursorPosition()[i];
            index[i] = (int) (pos[i] - mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[i])/mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[i];
            
            position += QString::number(pos[i],'f',1) + " ";
            mPoint2[i] = index[i];
          }
          point2Selected = true;
      }
  }
  mPosPoint2Label->setText(position);
  isPointsSelected();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool vvToolProfile::isPointsSelected()
{
  if (point1Selected && point2Selected)
      mComputeProfileButton->setEnabled(true);
  else
      mComputeProfileButton->setEnabled(false);
  
  return (point1Selected && point2Selected);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::computeProfile()
{
    if (!mCurrentSlicerManager) close();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    GetArgsInfoFromGUI();
    
    // Main filter
    clitk::ProfileImageGenericFilter::Pointer filter = clitk::ProfileImageGenericFilter::New();
    filter->SetInputVVImage(mCurrentImage);
    filter->SetArgsInfo(mArgsInfo);
    filter->Update();
    
    QApplication::restoreOverrideCursor();
 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::cancelPoints()
{ 
  QString position = "";
  mPosPoint1Label->setText(position);
  mPosPoint2Label->setText(position);
  point1Selected = false;
  point2Selected = false;
  mComputeProfileButton->setEnabled(false);
  isPointsSelected();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::InteractiveDisplayToggled(bool b)
{ 
  /*mInteractiveDisplayIsEnabled = b;
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
  }*/  
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::RemoveVTKObjects()
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
bool vvToolProfile::close()
{ 
  // RemoveVTKObjects();
  return vvToolWidgetBase::close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::closeEvent(QCloseEvent *event)
{ 
  RemoveVTKObjects();
  event->accept();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::reject()
{ 
  // DD("vvToolProfile::reject");
  RemoveVTKObjects();
  delete [] mPoint1;
  delete [] mPoint2;
  return vvToolWidgetBase::reject();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::enableLowerThan(bool b)
{ 
  /*if (!b) {
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
  }*/
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::useFGBGtoggled(bool)
{ 
  if (!mCheckBoxUseBG->isChecked() && !mCheckBoxUseFG->isChecked())
    mCheckBoxUseBG->toggle();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void vvToolProfile::InputIsSelected(std::vector<vvSlicerManager *> & m) {
//   DD("vvToolProfile::InputIsSelected vector in Profile");
//   DD(m.size());
// }
void vvToolProfile::InputIsSelected(vvSlicerManager * m)
{ 
  mCurrentSlicerManager = m;

  mPoint1 = new int[mCurrentSlicerManager->GetImage()->GetNumberOfDimensions()];
  mPoint2 = new int[mCurrentSlicerManager->GetImage()->GetNumberOfDimensions()];
  
  mComputeProfileButton->setEnabled(false);

  // Specific for this gui
  //mThresholdSlider1->SetValue(0);
  //mThresholdSlider2->SetValue(0);
  //mThresholdSlider1->SetImage(mCurrentImage);
  //mThresholdSlider2->SetImage(mCurrentImage);
  //mFGSlider->SetImage(mCurrentImage);
  //mBGSlider->SetImage(mCurrentImage);
  //  DD(mCurrentSlicerManager->GetFileName().c_str());
  //  mFGSlider->SetMaximum(mCurrentImage->GetFirstVTKImageData()->GetScalarTypeMax());
  //   mFGSlider->SetMinimum(mCurrentImage->GetFirstVTKImageData()->GetScalarTypeMin());
  //   mBGSlider->SetMaximum(mCurrentImage->GetFirstVTKImageData()->GetScalarTypeMax());
  //   mBGSlider->SetMinimum(mCurrentImage->GetFirstVTKImageData()->GetScalarTypeMin());

  // Output is uchar ...
  /*mFGSlider->SetMaximum(255);
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
  //valueChangedT1(mThresholdSlider1->GetValue());

  //connect(mThresholdSlider1, SIGNAL(valueChanged(double)), this, SLOT(valueChangedT1(double)));
  //connect(mThresholdSlider2, SIGNAL(valueChanged(double)), this, SLOT(valueChangedT2(double)));

  //connect(mCurrentSlicerManager,SIGNAL(UpdateSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
  //connect(mCurrentSlicerManager,SIGNAL(UpdateTSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
  
  //connect(mCurrentSlicerManager,SIGNAL(UpdateOrientation(int,int)),this,SLOT(UpdateOrientation(int, int)));

  //  connect(mCurrentSlicerManager, SIGNAL(LeftButtonReleaseSignal(int)), SLOT(LeftButtonReleaseEvent(int)));
  InteractiveDisplayToggled(mInteractiveDisplayIsEnabled); */
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void vvToolProfile::LeftButtonReleaseEvent(int slicer) {
//   DD("LeftButtonReleaseEvent");
//   for(int i=0; i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
//     if (i == slicer);
//     mCurrentSlicerManager->GetSlicer(i)->GetRenderWindow()->Render();
//   }
// }
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::UpdateOrientation(int slicer,int orientation)
{ 
  Update(slicer);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolProfile::UpdateSlice(int slicer,int slices)
{ 
  Update(slicer);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolProfile::Update(int slicer)
{ 
  //if (!mInteractiveDisplayIsEnabled) return;
  if (!mCurrentSlicerManager) close();
  //mImageContour[slicer]->Update(mThresholdSlider1->GetValue());
  //if (mRadioButtonLowerThan->isChecked()) 
  //  mImageContourLower[slicer]->Update(mThresholdSlider2->GetValue());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolProfile::GetArgsInfoFromGUI()
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
  cmdline_parser_clitkProfileImage_init(&mArgsInfo); // Initialisation to default
  //bool inverseBGandFG = false;

  //mArgsInfo.lower_given = 1;
  /*mArgsInfo.lower_arg = mThresholdSlider1->GetValue();
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
*/
  mArgsInfo.verbose_flag = false;
  
  mArgsInfo.point1_arg = mPoint1;
  mArgsInfo.point2_arg = mPoint2;
  mArgsInfo.point1_given = mCurrentSlicerManager->GetImage()->GetNumberOfDimensions();
  mArgsInfo.point2_given = mCurrentSlicerManager->GetImage()->GetNumberOfDimensions();
  
  // Required (even if not used)
  mArgsInfo.input_given = 0;
  mArgsInfo.output_given = 0;

  mArgsInfo.input_arg = new char;
  mArgsInfo.output_arg = new char;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::apply()
{ 
  if (!mCurrentSlicerManager) close();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  GetArgsInfoFromGUI();  
  
  // Main filter
  clitk::ProfileImageGenericFilter::Pointer filter =
    clitk::ProfileImageGenericFilter::New();
  filter->SetInputVVImage(mCurrentImage);
  filter->SetArgsInfo(mArgsInfo);
  filter->Update();


  // Output
  vvImage::Pointer output = filter->GetOutputVVImage();  
  std::ostringstream osstream;
  osstream << "Profiled_" << mCurrentSlicerManager->GetSlicer(0)->GetFileName() << ".mhd";
  AddImage(output,osstream.str());
  QApplication::restoreOverrideCursor();
  close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::valueChangedT2(double v)
{ 
  /*//  DD("valueChangedT2");
  if (mRadioButtonLowerThan->isChecked()) {
    mThresholdSlider1->SetMaximum(v);
    if (!mInteractiveDisplayIsEnabled) return;
    for(int i=0;i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
      mImageContourLower[i]->Update(v);
    }
    mCurrentSlicerManager->Render();
  }*/
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::valueChangedT1(double v)
{ 
  /*//  DD("valueChangedT1");
  if (!mCurrentSlicerManager) close();
  mThresholdSlider2->SetMinimum(v);
  //  int m1 = (int)lrint(v);
  if (!mInteractiveDisplayIsEnabled) return;
  for(int i=0;i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
    mImageContour[i]->Update(v);
  }
  mCurrentSlicerManager->Render();*/
}
//------------------------------------------------------------------------------
