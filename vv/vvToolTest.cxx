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
#include "vvToolTest.h"
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
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>

#include <itkGDCMImageIO.h>

//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolTest);
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolTest::Initialize()
{ cout << __func__ << endl;
  SetToolName("Test");
  SetToolMenuName("Test");
  SetToolIconFilename(":/common/icons/binarize.png");
  SetToolTip("try to display a sphere.");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolTest::vvToolTest(vvMainWindowBase * parent, Qt::WindowFlags f)
  :vvToolWidgetBase(parent,f),
   vvToolBase<vvToolTest>(parent),
   Ui::vvToolTest()
{ cout << __func__ << endl;
  // GUI Initialization

  // Connect signals & slots
  
  // Main filter
  mFilter = clitk::BinarizeImageGenericFilter::New();

  // Set how many inputs are needed for this tool
  AddInputSelector("Select one image", mFilter);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolTest::~vvToolTest()
{ cout << __func__ << endl;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/*
void vvToolTest::InteractiveDisplayToggled(bool b)
{ cout << __func__ << endl;
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
*/
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolTest::close()
{ cout << __func__ << endl;
  return vvToolWidgetBase::close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolTest::reject()
{ cout << __func__ << endl;
  // DD("vvToolBinarize::reject");
  return vvToolWidgetBase::reject();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

void vvToolTest::InputIsSelected(vvSlicerManager * m)
{ cout << __func__ << endl;
  mCurrentSlicerManager = m;


  vtkSmartPointer<vtkSphereSource> sphereSource = 
      vtkSmartPointer<vtkSphereSource>::New();
  sphereSource->SetCenter(0, 0, 0);
  //sphereSource->SetCenter(235.351, 175.781, 141.0);
  sphereSource->SetRadius(10.0);
  sphereSource->Update();
  vtkSmartPointer<vtkPolyDataMapper> sphereMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkSmartPointer<vtkActor> sphereActor = 
      vtkSmartPointer<vtkActor>::New();
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
  sphereActor->GetProperty()->SetOpacity(0.995);
  sphereActor->SetPosition(235.351, 175.781, -10);
  
  
  // VTK Renderer
  vtkSmartPointer<vtkRenderer> sphereRenderer = 
      vtkSmartPointer<vtkRenderer>::New();
  // Add Actor to renderer
  for(int i=0;i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
    mCurrentSlicerManager->GetSlicer(i)->GetRenderer()->AddActor(sphereActor);
  }
  //sphereRenderer->AddActor(sphereActor); //mettre le vvSlicer
   
  // VTK/Qt wedded
  //this->qvtkWidgetLeft->GetRenderWindow()->AddRenderer(leftRenderer);
 
 
 
 

  // VTK objects for interactive display
  valueChangedT1();

  //connect(mThresholdSlider1, SIGNAL(valueChanged(double)), this, SLOT(valueChangedT1()));

  connect(mCurrentSlicerManager,SIGNAL(UpdateSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
  connect(mCurrentSlicerManager,SIGNAL(UpdateTSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
  
  connect(mCurrentSlicerManager,SIGNAL(UpdateOrientation(int,int)),this,SLOT(UpdateSlice(int, int)));

  //  connect(mCurrentSlicerManager, SIGNAL(LeftButtonReleaseSignal(int)), SLOT(LeftButtonReleaseEvent(int)));
  //InteractiveDisplayToggled(mInteractiveDisplayIsEnabled);
  
  
  
typedef signed short InputPixelType;
const unsigned int Dimension = 3;
typedef itk::Image< InputPixelType, Dimension > InputImageType;
typedef itk::ImageFileReader< InputImageType > ReaderType;
ReaderType::Pointer reader = ReaderType::New();
reader->SetFileName( "/home/tbaudier/BJ13/RTSTRUCT/1.2.840.113704.1.111.4140.1439902720.30/20160201/160325.000000_/2.16.840.1.113669.1919.1454339005/2.16.840.1.113669.1919.1454339005/1.2.840.10008.5.1.4.1.1.481.3.1454339000.dcm" );
typedef itk::GDCMImageIO ImageIOType;
ImageIOType::Pointer gdcmImageIO = ImageIOType::New();
reader->SetImageIO( gdcmImageIO ); 
try
{
reader->Update();
}
catch (itk::ExceptionObject & e)
{
std::cerr << "exception in file reader " << std::endl;
std::cerr << e.GetDescription() << std::endl;
std::cerr << e.GetLocation() << std::endl;
return;
}
  
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
void vvToolTest::apply()
{ cout << __func__ << endl;
  if (!mCurrentSlicerManager) close();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  //GetArgsInfoFromGUI();  
  
  // Main filter



  // Output
  QApplication::restoreOverrideCursor();
  close();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolTest::UpdateSlice(int slicer,int slices)
{ cout << __func__ << endl;
  Update(slicer);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolTest::Update(int slicer)
{ cout << __func__ << endl;
  if (!mCurrentSlicerManager) close();
  mCurrentSlicerManager->Render();
  //mImageContour[slicer]->Update(mThresholdSlider1->GetValue());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/* void vvToolTest::GetArgsInfoFromGUI()
{ cout << __func__ << endl;

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
/*  cmdline_parser_clitkBinarizeImage_init(&mArgsInfo); // Initialisation to default
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
} */
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolTest::valueChangedT1()
{ cout << __func__ << endl;
  //  DD("valueChangedT1");
  if (!mCurrentSlicerManager) close();
  for(int i=0;i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
    //mImageContour[i]->Update(v);
  }
  mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------
