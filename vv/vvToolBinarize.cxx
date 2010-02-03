/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolBinarize.cxx,v $
  Language:  C++
  Date:      $Date: 2010/02/03 13:08:55 $
  Version:   $Revision: 1.4 $
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

#include "vtkImageActor.h"
#include "vtkCamera.h"

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

  // VTK objects
  mClipper = vtkImageClip::New();
  mSquares1 = vtkMarchingSquares::New();
  mSquaresMapper1 = vtkPolyDataMapper::New();
  mSquaresActor1 = vtkActor::New();

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
void vvToolBinarize::InputIsSelected() {

  // Common
  int index = mToolInputSelectionWidget->GetSelectedInputIndex();
  mCurrentSliceManager = mSlicerManagersCompatible[index];
  mCurrentImage = mCurrentSliceManager->GetImage();
  toolMainWidget->setEnabled(true);

  // Specific for this gui
  mThresholdSlider1->SetImage(mCurrentImage);
  mThresholdSlider2->SetImage(mCurrentImage);
  mFGSlider->SetImage(mCurrentImage);
  mBGSlider->SetImage(mCurrentImage);
  mFGSlider->SetValue(1);
  mBGSlider->SetValue(0);
  DD(mCurrentSliceManager->GetFileName().c_str());
  
  DD("VTK");
  DD(mCurrentSliceManager->NumberOfSlicers());
  //    mClipper->SetInput(mCurrentSliceManager->GetSlicer(0)->GetInput());
  DD(mCurrentImage->GetFirstVTKImageData());
  DD(mClipper);
  DD(mCurrentSliceManager->GetSlicer(0));
  mClipper->SetInput(mCurrentImage->GetFirstVTKImageData());
  
  mSquares1->SetInput(mClipper->GetOutput());
  mSquaresMapper1->SetInput(mSquares1->GetOutput());
  mSquaresActor1->SetMapper(mSquaresMapper1);
  mSquaresActor1->GetProperty()->SetColor(1.0,0,0);
  mSquaresActor1->SetPickable(0);
  mCurrentSliceManager->GetSlicer(0)->GetRenderer()->AddActor(mSquaresActor1);
  mSquares1->Update();
  
  
  DD("VTK end");
  
  //    connect(mCurrentSliceManager,SIGNAL(UpdateTSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
  connect(mCurrentSliceManager,SIGNAL(UpdateSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
  //connect(mCurrentSliceManager,SIGNAL(UpdateSliceRange(int,int,int,int,int)),this,SLOT(UpdateSlice(int, int)));
  //    connect(mCurrentSliceManager,SIGNAL(LandmarkAdded()),this,SLOT(InsertSeed()));
  
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBinarize::UpdateSlice(int slicer,int slices) {
 
  // A METTRE SUR TOUT LES SLICES ! PAS QUE 0

  // !! signal update slice pas tjs quand move slicer ???


   int slice = mCurrentSliceManager->GetSlicer(0)->GetSlice();
    //int tslice = mCurrentSliceManager->GetSlicer(0)->GetTSlice();
    mClipper->SetInput(mCurrentSliceManager->GetSlicer(0)->GetInput());
    int* extent = mCurrentSliceManager->GetSlicer(0)->GetImageActor()->GetDisplayExtent();
    mClipper->SetOutputWholeExtent(extent[0],extent[1],extent[2],extent[3],extent[4],extent[5]);
    int i;
    for (i = 0; i < 6;i = i+2)
    {
        if (extent[i] == extent[i+1])
        {
            break;
        }
    }

    switch (i)
    {
    case 0:
        if (mCurrentSliceManager->GetSlicer(0)->GetRenderer()->GetActiveCamera()->GetPosition()[0] > slice)
        {
            mSquaresActor1->SetPosition(1,0,0);
            // mSquaresActor2->SetPosition(1,0,0);
        }
        else
        {
            mSquaresActor1->SetPosition(-1,0,0);
            // mSquaresActor2->SetPosition(-1,0,0);
        }
        break;
    case 2:
        if (mCurrentSliceManager->GetSlicer(0)->GetRenderer()->GetActiveCamera()->GetPosition()[1] > slice)
        {
            mSquaresActor1->SetPosition(0,1,0);
          //   mSquaresActor2->SetPosition(0,1,0);
        }
        else
        {
            mSquaresActor1->SetPosition(0,-1,0);
            // mSquaresActor2->SetPosition(0,-1,0);
        }
        break;
    case 4:
        if (mCurrentSliceManager->GetSlicer(0)->GetRenderer()->GetActiveCamera()->GetPosition()[2] > slice)
        {
            mSquaresActor1->SetPosition(0,0,1);
            // mSquaresActor2->SetPosition(0,0,1);
        }
        else
        {
            mSquaresActor1->SetPosition(0,0,-1);
            // mSquaresActor2->SetPosition(0,0,-1);
        }
        break;
    }
    mSquares1->Update();
   //  mSquares2->Update();

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

  if (mRadioButtonGreaterThan->isChecked()) { // Greater Than (and Lower Than)
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
  }
  else {
    if (mRadioButtonEqualThan->isChecked()) {
      mArgsInfo.lower_given = 1;
      mArgsInfo.upper_given = 1;
      mArgsInfo.lower_arg = mThresholdSlider1->GetValue();
      mArgsInfo.upper_arg = mThresholdSlider1->GetValue();
    }
    else {
      mArgsInfo.lower_given = 1;
      mArgsInfo.upper_given = 1;
      mArgsInfo.lower_arg = mThresholdSlider1->GetValue();
      mArgsInfo.upper_arg = mThresholdSlider1->GetValue();
      inverseBGandFG = true;
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

  mArgsInfo.useBG_flag = mCheckBoxUseBG->isChecked();
  mArgsInfo.useFG_flag = mCheckBoxUseFG->isChecked();

  DD(mArgsInfo.useBG_flag);
  DD(mArgsInfo.useFG_flag);

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
  DD("Apply");

  GetArgsInfoFromGUI();

  DD(mArgsInfo.lower_arg);
  // cmdline_parser2(argc, argv, &args_info, 1, 1, 0);			
  //   if (args_info.config_given)	
  // cmdline_parser_configfile ("toto.config", &args_info, 0, 0, 1);
  //   else cmdline_parser(argc, argv, &args_info);

  // Main filter
  clitk::BinarizeImageGenericFilter<args_info_clitkBinarizeImage>::Pointer filter = 
    clitk::BinarizeImageGenericFilter<args_info_clitkBinarizeImage>::New();
  filter->SetArgsInfo(mArgsInfo);
  filter->SetInputVVImage(mCurrentImage);
  filter->Update();

  // Output ???
  vvImage::Pointer output = filter->GetOutputVVImage();
  DD(output->GetScalarTypeAsString());
  CREATOR(vvToolBinarize)->mMainWindow->AddImage(output,"toto.mhd"); 
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
  DD(v);
  int m1 = (int)lrint(v);
  DD(m1);  
  int* extent = mCurrentSliceManager->GetSlicer(0)->GetImageActor()->GetDisplayExtent();
  mClipper->SetOutputWholeExtent(extent[0],extent[1],extent[2],extent[3],extent[4],extent[5]);


int slice = mCurrentSliceManager->GetSlicer(0)->GetSlice();

 int i;
for (i = 0; i < 6;i = i+2)
    {
        if (extent[i] == extent[i+1])
        {
            break;
        }
    }

    switch (i)
    {
    case 0:
        if (mCurrentSliceManager->GetSlicer(0)->GetRenderer()->GetActiveCamera()->GetPosition()[0] > slice)
        {
            mSquaresActor1->SetPosition(1,0,0);
            // mSquaresActor2->SetPosition(1,0,0);
        }
        else
        {
            mSquaresActor1->SetPosition(-1,0,0);
            // mSquaresActor2->SetPosition(-1,0,0);
        }
        break;
    case 2:
        if (mCurrentSliceManager->GetSlicer(0)->GetRenderer()->GetActiveCamera()->GetPosition()[1] > slice)
        {
            mSquaresActor1->SetPosition(0,1,0);
           //  mSquaresActor2->SetPosition(0,1,0);
        }
        else
        {
            mSquaresActor1->SetPosition(0,-1,0);
            // mSquaresActor2->SetPosition(0,-1,0);
        }
        break;
    case 4:
        if (mCurrentSliceManager->GetSlicer(0)->GetRenderer()->GetActiveCamera()->GetPosition()[2] > slice)
        {
            mSquaresActor1->SetPosition(0,0,1);
            // mSquaresActor2->SetPosition(0,0,1);
        }
        else
        {
            mSquaresActor1->SetPosition(0,0,-1);
            // mSquaresActor2->SetPosition(0,0,-1);
        }
        break;
    }




  mSquares1->SetValue(0,m1);
  mSquares1->Update();
  mCurrentSliceManager->Render();
}
//------------------------------------------------------------------------------
