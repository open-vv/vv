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

// vtk
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkImageClip.h>
#include <vtkRenderWindow.h>
#include <vtkChartXY.h>
#include <vtkPlot.h>
#include <vtkRendererCollection.h>
#include <vtkRenderer.h>


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

  // Connect signals & slots
  connect(mSelectPoint1Button, SIGNAL(clicked()), this, SLOT(selectPoint1()));
  connect(mSelectPoint2Button, SIGNAL(clicked()), this, SLOT(selectPoint2()));
  connect(mCancelPointsButton, SIGNAL(clicked()), this, SLOT(cancelPoints()));
  connect(mComputeProfileButton, SIGNAL(clicked()), this, SLOT(computeProfile()));

  // Initialize some widget
  ProfileWidget->hide();
  mPoint1 = NULL;
  mPoint2 = NULL;
  
  mPoint1Selected = false;
  mPoint2Selected = false;
    
  mView = vtkSmartPointer<vtkContextView>::New();
  vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
  chart->SetAutoSize(false);
  chart->SetRenderEmpty(true);
  mView->GetScene()->AddItem(chart);
  this->ProfileWidget->GetRenderWindow()->GetRenderers()->RemoveAllItems();
  this->ProfileWidget->GetRenderWindow()->AddRenderer(mView->GetRenderer());
  ProfileWidget->show();

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
  
    if (mPoint1Selected) {
      ProfileWidget->hide();
      vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
      chart->SetAutoSize(false);
      chart->SetRenderEmpty(true);
      mView->GetScene()->ClearItems();
      mView->GetScene()->AddItem(chart);
      this->ProfileWidget->GetRenderWindow()->GetRenderers()->RemoveAllItems();
      this->ProfileWidget->GetRenderWindow()->AddRenderer(mView->GetRenderer());
      ProfileWidget->show();
  }
  
  mPoint1Selected = false;
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
          mPoint1Selected = true;
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
  
  if (mPoint2Selected) {
      ProfileWidget->hide();
      vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
      chart->SetAutoSize(false);
      chart->SetRenderEmpty(true);
      mView->GetScene()->ClearItems();
      mView->GetScene()->AddItem(chart);
      this->ProfileWidget->GetRenderWindow()->GetRenderers()->RemoveAllItems();
      this->ProfileWidget->GetRenderWindow()->AddRenderer(mView->GetRenderer());
      ProfileWidget->show();
  }
  
  mPoint2Selected = false;
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
          mPoint2Selected = true;
      }
  }
  mPosPoint2Label->setText(position);
  isPointsSelected();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool vvToolProfile::isPointsSelected()
{
  if (mPoint1Selected && mPoint2Selected)
      mComputeProfileButton->setEnabled(true);
  else
      mComputeProfileButton->setEnabled(false);
  
  return (mPoint1Selected && mPoint2Selected);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::computeProfile()
{
    if (!mCurrentSlicerManager) close();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    GetArgsInfoFromGUI();
    ProfileWidget->hide();
    
    // Main filter
    mFilter->SetInputVVImage(mCurrentImage);
    mFilter->SetArgsInfo(mArgsInfo);
    mFilter->Update();
    
    vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();
    vtkSmartPointer<vtkFloatArray> arrX = vtkSmartPointer<vtkFloatArray>::New();
    vtkSmartPointer<vtkFloatArray> arrY = vtkSmartPointer<vtkFloatArray>::New();
    arrX = mFilter->GetArrayX();
    arrY = mFilter->GetArrayY();
    arrX->SetName("Voxel");
    arrY->SetName("Intensity");
    table->AddColumn(arrX);
    table->AddColumn(arrY);
    
    mView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
 
    vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
    chart->SetAutoSize(true);
    mView->GetScene()->ClearItems();
    mView->GetScene()->AddItem(chart);
    vtkPlot *line = chart->AddPlot(vtkChart::LINE);
#if VTK_MAJOR_VERSION <= 5
    line->SetInput(table, 0, 1);
#else
    line->SetInputData(table, 0, 1);
#endif
    line->SetColor(0, 255, 0, 255);
    line->SetWidth(1.0);
    
    this->ProfileWidget->GetRenderWindow()->GetRenderers()->RemoveAllItems();
    this->ProfileWidget->GetRenderWindow()->AddRenderer(mView->GetRenderer());
    ProfileWidget->show();
    
    QApplication::restoreOverrideCursor();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::cancelPoints()
{ 
  ProfileWidget->hide();
  vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
  chart->SetAutoSize(false);
  chart->SetRenderEmpty(true);
  mView->GetScene()->ClearItems();
  mView->GetScene()->AddItem(chart);
  this->ProfileWidget->GetRenderWindow()->GetRenderers()->RemoveAllItems();
  this->ProfileWidget->GetRenderWindow()->AddRenderer(mView->GetRenderer());
  ProfileWidget->show();
  
  QString position = "";
  mPosPoint1Label->setText(position);
  mPosPoint2Label->setText(position);
  mPoint1Selected = false;
  mPoint2Selected = false;
  mComputeProfileButton->setEnabled(false);
  isPointsSelected();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::RemoveVTKObjects()
{ 
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
void vvToolProfile::InputIsSelected(vvSlicerManager * m)
{ 
  mCurrentSlicerManager = m;

  mPoint1 = new int[mCurrentSlicerManager->GetImage()->GetNumberOfDimensions()];
  mPoint2 = new int[mCurrentSlicerManager->GetImage()->GetNumberOfDimensions()];
  
  mComputeProfileButton->setEnabled(false);
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
  if (!mCurrentSlicerManager || !isPointsSelected()) {
      close();
      return;
  }
  
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  // Output
  std::string fileName = "Profiled_" + mCurrentSlicerManager->GetSlicer(0)->GetFileName() + ".txt";
  ofstream fileOpen(fileName.c_str(), std::ofstream::trunc);
  
  if(!fileOpen) {
    cerr << "Error during saving" << endl;
    QApplication::restoreOverrideCursor();
    close();
    
    return;
  }
  vtkSmartPointer<vtkFloatArray> arrX = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> arrY = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> coords = vtkSmartPointer<vtkFloatArray>::New();
  arrX = mFilter->GetArrayX();
  arrY = mFilter->GetArrayY();
  coords = mFilter->GetCoord();
  double *tuple;
  tuple = new double[mCurrentSlicerManager->GetImage()->GetNumberOfDimensions()];
  int i(0);
   
  while (i<arrX->GetNumberOfTuples()) {
      fileOpen << arrX->GetTuple(i)[0] << "\t" << arrY->GetTuple(i)[0] << "\t" ;
      
      coords->GetTuple(i, tuple);
      for (int j=0; j<mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() ; ++j) {
          fileOpen << tuple[j] << "\t" ;
      }
      fileOpen << endl;
      ++i;
  }
  
  delete [] tuple;

  fileOpen.close();

  QApplication::restoreOverrideCursor();
  close();
}
//------------------------------------------------------------------------------


