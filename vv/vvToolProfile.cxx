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

#include <QFileDialog>
#include <QShortcut>

#include <algorithm>

// vv
#include "vvToolProfile.h"
#include "vvProgressDialog.h"
#include "vvSlicerManager.h"
#include "vvSlicer.h"
#include "vvToolInputSelectorWidget.h"

// vtk
#include <vtkAxis.h>
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkImageClip.h>
#include <vtkRenderWindow.h>
#include <vtkChartXY.h>
#include <vtkPlot.h>
#include <vtkRendererCollection.h>
#include <vtkRenderer.h>

#include <vtkLine.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkBox.h>
#include <vtkInformation.h>
#include <vtkUnsignedCharArray.h>

#ifdef Q_OS_OSX
# include "vvOSXHelper.h"
#endif

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
  
  QShortcut *shortcutPoint1 = new QShortcut(QKeySequence("Ctrl+1"), parent);
  shortcutPoint1->setContext(Qt::ApplicationShortcut);
  QObject::connect(shortcutPoint1, SIGNAL(activated()), this, SLOT(selectPoint1()));
  QShortcut *shortcutPoint2 = new QShortcut(QKeySequence("Ctrl+2"), parent);
  shortcutPoint2->setContext(Qt::ApplicationShortcut);
  QObject::connect(shortcutPoint2, SIGNAL(activated()), this, SLOT(selectPoint2()));

  // Connect signals & slots
  connect(mSelectPoint1Button, SIGNAL(clicked()), this, SLOT(selectPoint1()));
  connect(mSelectPoint2Button, SIGNAL(clicked()), this, SLOT(selectPoint2()));
  connect(mCancelPointsButton, SIGNAL(clicked()), this, SLOT(cancelPoints()));
  connect(mSaveProfileButton, SIGNAL(clicked()), this, SLOT(SaveAs()));

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
#if VTK_MAJOR_VERSION >= 9
  this->ProfileWidget->renderWindow()->GetRenderers()->RemoveAllItems();
  this->ProfileWidget->renderWindow()->AddRenderer(mView->GetRenderer());
#else
  this->ProfileWidget->GetRenderWindow()->GetRenderers()->RemoveAllItems();
  this->ProfileWidget->GetRenderWindow()->AddRenderer(mView->GetRenderer());
#endif
  ProfileWidget->show();

#ifdef Q_OS_OSX
  disableGLHiDPI(ProfileWidget->winId());
#endif

  // Main filter
  mFilter = clitk::ProfileImageGenericFilter::New();

  // Set how many inputs are needed for this tool
  AddInputSelector("Select one image", mFilter);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolProfile::~vvToolProfile()
{ 
  delete [] mPoint1;
  delete [] mPoint2;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::selectPoint1()
{
  QString position = "";
  
  if(mCurrentSlicerManager) {
    if (mPoint1Selected) {
      ProfileWidget->hide();
      vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
      chart->SetAutoSize(false);
      chart->SetRenderEmpty(true);
      mView->GetScene()->ClearItems();
      mView->GetScene()->AddItem(chart);
#if VTK_MAJOR_VERSION >= 9
      this->ProfileWidget->renderWindow()->GetRenderers()->RemoveAllItems();
      this->ProfileWidget->renderWindow()->AddRenderer(mView->GetRenderer());
#else
      this->ProfileWidget->GetRenderWindow()->GetRenderers()->RemoveAllItems();
      this->ProfileWidget->GetRenderWindow()->AddRenderer(mView->GetRenderer());
#endif
      ProfileWidget->show();
      mCurrentSlicerManager->GetLandmarks()->RemoveLandmarkWithLabel("P1", mPoint1[3]);
    }
  
    mPoint1Selected = false;

    if(mCurrentSlicerManager->GetSelectedSlicer() != -1) {
      double *pos;
      pos = new double [4];
      pos[0] = pos[1] = pos[2] = pos[3] = 0;
          
      int i(0);
      while (i<mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() && i<3) {
        pos[i] = mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetCursorPosition()[i];
        ++i;
      }
      double posTransformed[3];
      mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetConcatenatedTransform()->TransformPoint(pos, posTransformed);
      i = 0;
      while (i<mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() && i<3) {
        pos[i] = posTransformed[i];
        mPoint1[i] = round((pos[i] - mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetOrigin()[i])/mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetSpacing()[i]);
        pos[i] = mPoint1[i]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetSpacing()[i] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetOrigin()[i]; //Ensure to be at the center of the voxel
        position += QString::number(mPoint1[i],'f',0) + " ";
        ++i;
      }
      if (mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() == 4) {
        pos[3] = mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetTSlice();
        mPoint1[3] = pos[3];
        position += QString::number(mPoint1[3],'f',0) + " ";
      }
      mPoint1Selected = true;
      mCurrentSlicerManager->AddLandmarkProfile(pos[0], pos[1], pos[2], pos[3]);
      mCurrentSlicerManager->GetLandmarks()->GetLabels()->SetNumberOfValues(mCurrentSlicerManager->GetLandmarks()->GetLabels()->GetNumberOfValues()-1);
      mCurrentSlicerManager->GetLandmarks()->GetLabels()->Modified();
      mCurrentSlicerManager->GetLandmarks()->GetLabels()->InsertNextValue("P1");
    }
  }
  mPosPoint1Label->setText(position);
  isPointsSelected();
  mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::selectPoint2()
{
  QString position = "";
  
  if(mCurrentSlicerManager) {
    if (mPoint2Selected) {
      ProfileWidget->hide();
      vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
      chart->SetAutoSize(false);
      chart->SetRenderEmpty(true);
      mView->GetScene()->ClearItems();
      mView->GetScene()->AddItem(chart);
#if VTK_MAJOR_VERSION >= 9
      this->ProfileWidget->renderWindow()->GetRenderers()->RemoveAllItems();
      this->ProfileWidget->renderWindow()->AddRenderer(mView->GetRenderer());
#else
      this->ProfileWidget->GetRenderWindow()->GetRenderers()->RemoveAllItems();
      this->ProfileWidget->GetRenderWindow()->AddRenderer(mView->GetRenderer());
#endif
      ProfileWidget->show();
      mCurrentSlicerManager->GetLandmarks()->RemoveLandmarkWithLabel("P2", mPoint2[3]);
    }
  
    mPoint2Selected = false;
    
    if(mCurrentSlicerManager->GetSelectedSlicer() != -1) {
      double *pos;
      pos = new double [4];
      pos[0] = pos[1] = pos[2] = pos[3] = 0;;
          
      int i(0);
      while (i<mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() && i<3) {
        pos[i] = mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetCursorPosition()[i];
        ++i;
      }
      double posTransformed[3];
      mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetConcatenatedTransform()->TransformPoint(pos, posTransformed);
      i = 0;
      while (i<mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() && i<3) {
        pos[i] = posTransformed[i];
        mPoint2[i] = round((pos[i] - mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetOrigin()[i])/mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetSpacing()[i]);
        pos[i] = mPoint2[i]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetSpacing()[i] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetOrigin()[i]; //Ensure to be at the center of the voxel
        position += QString::number(mPoint2[i],'f',0) + " ";
        ++i;
      }
      if (mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() == 4) {
        pos[3] = mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetTSlice();
        mPoint2[3] = pos[3];
        position += QString::number(mPoint2[3],'f',0) + " ";
      }
      mPoint2Selected = true;
      mCurrentSlicerManager->AddLandmarkProfile(pos[0], pos[1], pos[2], pos[3]);
      mCurrentSlicerManager->GetLandmarks()->GetLabels()->SetNumberOfValues(mCurrentSlicerManager->GetLandmarks()->GetLabels()->GetNumberOfValues()-1);
      mCurrentSlicerManager->GetLandmarks()->GetLabels()->Modified();
      mCurrentSlicerManager->GetLandmarks()->GetLabels()->InsertNextValue("P2");
    }
  }
  mPosPoint2Label->setText(position);
  isPointsSelected();
  mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool vvToolProfile::isPointsSelected()
{
  if (mPoint1Selected && mPoint2Selected) {
      mSaveProfileButton->setEnabled(true);
      computeProfile();
      SetPoints();
      for(int i=0;i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
        DisplayLine(i);
        connect(mCurrentSlicerManager->GetSlicer(i), SIGNAL(UpdateDisplayExtentBegin(int)), this, SLOT(DeleteLine(int)));
        connect(mCurrentSlicerManager->GetSlicer(i), SIGNAL(UpdateDisplayExtentEnd(int)), this, SLOT(DisplayLine(int)));
      }
      mCurrentSlicerManager->Render();
  }
  else {
      mSaveProfileButton->setEnabled(false);
      for(int i=0;i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
        disconnect(mCurrentSlicerManager->GetSlicer(i), SIGNAL(UpdateDisplayExtentBegin(int)), this, SLOT(DeleteLine(int)));
        disconnect(mCurrentSlicerManager->GetSlicer(i), SIGNAL(UpdateDisplayExtentEnd(int)), this, SLOT(DisplayLine(int)));
        mCurrentSlicerManager->GetSlicer(i)->GetRenderer()->RemoveActor(mLineActors[i]);
      }

  }
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
    
    //Creation of the XY chart
    vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();
    vtkSmartPointer<vtkFloatArray> arrX = vtkSmartPointer<vtkFloatArray>::New();
    vtkSmartPointer<vtkFloatArray> arrY = vtkSmartPointer<vtkFloatArray>::New();
    arrX = mFilter->GetArrayX();
    arrY = mFilter->GetArrayY();
    arrX->SetName("Distance (mm)");
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
    chart->GetAxis(vtkAxis::LEFT)->SetTitle("Intensity");
    chart->GetAxis(vtkAxis::BOTTOM)->SetTitle("Distance (mm)");
    
#if VTK_MAJOR_VERSION >= 9
    this->ProfileWidget->renderWindow()->GetRenderers()->RemoveAllItems();
    this->ProfileWidget->renderWindow()->AddRenderer(mView->GetRenderer());
#else
    this->ProfileWidget->GetRenderWindow()->GetRenderers()->RemoveAllItems();
    this->ProfileWidget->GetRenderWindow()->AddRenderer(mView->GetRenderer());
#endif
    ProfileWidget->show();
    
    QApplication::restoreOverrideCursor();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::cancelPoints()
{ 
  if (mPoint1Selected)
    mCurrentSlicerManager->GetLandmarks()->RemoveLandmarkWithLabel("P1", mPoint1[3]);
  if (mPoint2Selected)
    mCurrentSlicerManager->GetLandmarks()->RemoveLandmarkWithLabel("P2", mPoint2[3]);
  ProfileWidget->hide();
  vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
  chart->SetAutoSize(false);
  chart->SetRenderEmpty(true);
  mView->GetScene()->ClearItems();
  mView->GetScene()->AddItem(chart);
#if VTK_MAJOR_VERSION >= 9
  this->ProfileWidget->renderWindow()->GetRenderers()->RemoveAllItems();
  this->ProfileWidget->renderWindow()->AddRenderer(mView->GetRenderer());
#else
  this->ProfileWidget->GetRenderWindow()->GetRenderers()->RemoveAllItems();
  this->ProfileWidget->GetRenderWindow()->AddRenderer(mView->GetRenderer());
#endif
  ProfileWidget->show();
  
  QString position = "";
  mPosPoint1Label->setText(position);
  mPosPoint2Label->setText(position);
  mPoint1Selected = false;
  mPoint2Selected = false;
  mSaveProfileButton->setEnabled(false);
  isPointsSelected();
  mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::RemoveVTKObjects()
{ 
  if (mCurrentSlicerManager)
  {
    connect(mCurrentSlicerManager, SIGNAL(callAddLandmark(float,float,float,float)), mCurrentSlicerManager, SLOT(AddLandmark(float,float,float,float)));

    for(int i=0;i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
      mCurrentSlicerManager->GetSlicer(i)->GetRenderer()->RemoveActor(mLineActors[i]);
    }

    if (mPoint1Selected)
      mCurrentSlicerManager->GetLandmarks()->RemoveLandmarkWithLabel("P1", mPoint1[3]);
    if (mPoint2Selected)
      mCurrentSlicerManager->GetLandmarks()->RemoveLandmarkWithLabel("P2", mPoint2[3]);

    mCurrentSlicerManager->Render();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolProfile::close()
{ 
  //RemoveVTKObjects();

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
  return vvToolWidgetBase::reject();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::InputIsSelected(vvSlicerManager * m)
{ 
  mCurrentSlicerManager = m;

  mPoint1 = new double[4];
  mPoint1[0] = mPoint1[1] = mPoint1[2] = mPoint1[3] = 0;
  mPoint2 = new double[4];
  mPoint2[0] = mPoint2[1] = mPoint2[2] = mPoint2[3] = 0;
  
  mSaveProfileButton->setEnabled(false);
  mTextFileName = "Profile.txt";
  InitializeLine();
  
  disconnect(mCurrentSlicerManager, SIGNAL(callAddLandmark(float,float,float,float)), mCurrentSlicerManager, SLOT(AddLandmark(float,float,float,float)));
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
  close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::SaveAs()
{ 
  QStringList OutputListeFormat;
  OutputListeFormat.clear();
  OutputListeFormat.push_back(".txt");
  
  QString Extensions = "AllFiles(*.*)";
  for (int i = 0; i < OutputListeFormat.count(); i++) {
    Extensions += ";;Text File ( *";
    Extensions += OutputListeFormat[i];
    Extensions += ")";
  }
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), mTextFileName.c_str(), Extensions);
  if (!fileName.isEmpty()) {
    std::string fileformat = itksys::SystemTools::GetFilenameLastExtension(fileName.toStdString());
    QString fileQFormat = fileformat.c_str();
    if (OutputListeFormat.contains(fileformat.c_str()) || fileQFormat.isEmpty()) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        std::string action = "Saving";
        vvProgressDialog progress("Saving "+fileName.toStdString());
        qApp->processEvents();
        
        if (!mCurrentSlicerManager || !isPointsSelected()) {
            close();
            return;
        }

        // Output
        mTextFileName = fileName.toStdString();
        if (fileQFormat.isEmpty())
            mTextFileName += ".txt";
        std::ofstream fileOpen(mTextFileName.c_str(), std::ofstream::trunc);
  
        if(!fileOpen) {
            cerr << "Error during saving" << endl;
            QApplication::restoreOverrideCursor();
            close();
            return;
        }

        mFilter->WriteOutput(mTextFileName.c_str());

        fileOpen.close();
        QApplication::restoreOverrideCursor();
    } else {
      QString error = fileformat.c_str();
      error += " format unknown !!!\n";
      QMessageBox::information(this,tr("Saving Problem"),error);
      SaveAs();
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::DeleteLine(int slicer)
{
  if (!mPoint1Selected && !mPoint2Selected)
    return;
  
  if(mCurrentSlicerManager) {
      if(mCurrentSlicerManager->GetSelectedSlicer() != -1) {
          mCurrentSlicerManager->GetSlicer(slicer)->GetRenderer()->RemoveActor(mLineActors[slicer]);
      }
   }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::DisplayLine(int slicer)
{ 
  if (!mPoint1Selected && !mPoint2Selected)
    return;
  
  if(mCurrentSlicerManager) {
      if(mCurrentSlicerManager->GetSelectedSlicer() != -1) {
          double currentSlice = (mCurrentSlicerManager->GetSlicer(slicer)->GetSlice()*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()] - mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetOrigin()[mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()] )/ mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetSpacing()[mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()];
          if (std::min(mPoint1[mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()],mPoint2[mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()]) <= currentSlice && std::max(mPoint1[mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()],mPoint2[mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()]) >= currentSlice) {
            vtkSmartPointer<vtkBox> clippingBox = vtkSmartPointer<vtkBox>::New();
            double extent[6];
            for (int j=0; j<6; ++j) {
                extent[j] = mCurrentSlicerManager->GetSlicer(slicer)->GetExtent()[j];
            }
            extent[2*mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()] = mCurrentSlicerManager->GetSlicer(slicer)->GetImageActor()->GetBounds()[ mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()*2 ]-fabs(mCurrentSlicerManager->GetSlicer(slicer)->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetSpacing()[mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()]);
            extent[2*mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()+1] = mCurrentSlicerManager->GetSlicer(slicer)->GetImageActor()->GetBounds()[ mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()*2+1 ]+fabs(mCurrentSlicerManager->GetSlicer(slicer)->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetSpacing()[mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()]);
            clippingBox->SetBounds(extent);
            
            vtkSmartPointer<vtkClipPolyData> clipper = vtkSmartPointer<vtkClipPolyData>::New();
            clipper->SetClipFunction(clippingBox);
#if VTK_MAJOR_VERSION <= 5
            clipper->SetInput(mLinesPolyData);
#else
            mLineTransform = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
            mLineTransform->SetInputData(mLinesPolyData);
            mLineTransform->SetTransform(mCurrentSlicerManager->GetSlicer(slicer)->GetConcatenatedTransform()->GetInverse());
            clipper->SetInputConnection(mLineTransform->GetOutputPort());
#endif
            clipper->InsideOutOff();
            clipper->Update();        
            vtkSmartPointer<vtkPolyDataMapper> lineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
            lineMapper->SetInput(clipper->GetOutput());
#else
            lineMapper->SetInputData(clipper->GetOutput());
#endif 
            
            mLineActors[slicer]->SetMapper(lineMapper);
            mLineActors[slicer]->GetProperty()->SetOpacity(0.995);

            mCurrentSlicerManager->GetSlicer(slicer)->GetRenderer()->AddActor(mLineActors[slicer]);
         }
      }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::InitializeLine()
{ 
  if(mCurrentSlicerManager) {
      
      mLinesPolyData = vtkSmartPointer<vtkPolyData>::New();
      
      vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
      double pos[4];
      pos[0] = pos[1] = pos[2] = pos[3] = 0;
      pts->InsertNextPoint(pos);
      pts->InsertNextPoint(pos);
      mLinesPolyData->SetPoints(pts);
      
      vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
      vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
      line->GetPointIds()->SetId(0, 0); // the second 0 is the index of the 1st point in mLinesPolyData's points (pts)
      line->GetPointIds()->SetId(1, 1); // the second 1 is the index of the 2nd point in mLinesPolyData's points (pts)
      lines->InsertNextCell(line);
      mLinesPolyData->SetLines(lines);
       
      unsigned char red[3] = { 255, 0, 0 };
      vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
      colors->SetNumberOfComponents(3);
#if VTK_MAJOR_VERSION >= 8 || (VTK_MAJOR_VERSION == 7 && VTK_MINOR_VERSION >= 1)
      colors->InsertNextTypedTuple(red);
#else
      colors->InsertNextTupleValue(red);
#endif
      mLinesPolyData->GetCellData()->SetScalars(colors);
      
      for(int i=0;i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
        mLineActors.push_back(vtkSmartPointer<vtkActor>::New());
      }
   }       
}    
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolProfile::SetPoints()
{ 
  if (!mPoint1Selected && !mPoint2Selected)
    return;
  
  if(mCurrentSlicerManager) {
      if(mCurrentSlicerManager->GetSelectedSlicer() != -1) {
          /*double *pos;
          pos = new double [4];
          pos[0] = pos[1] = pos[2] = pos[3] = 0;
          
          int i(0);
          while (i<3) {
            pos[i] = mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetCursorPosition()[i];
            ++i;
          }
          if (mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() == 4) {
            pos[3] = mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetTSlice();
          } */
          
          double p0[4], p1[4];
          p0[0] = mPoint1[0]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetSpacing()[0] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetOrigin()[0];
          p0[1] = mPoint1[1]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetSpacing()[1] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetOrigin()[1];
          p0[2] = mPoint1[2]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetSpacing()[2] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetOrigin()[2];
          p0[3] = mPoint1[3];

          p1[0] = mPoint2[0]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetSpacing()[0] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetOrigin()[0];
          p1[1] = mPoint2[1]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetSpacing()[1] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetOrigin()[1];
          p1[2] = mPoint2[2]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetSpacing()[2] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetImage()->GetVTKImages()[mCurrentSlicerManager->GetTSlice()]->GetOrigin()[2];
          p1[3] = mPoint2[3];
              
          vtkSmartPointer<vtkPoints> pts = mLinesPolyData->GetPoints();
          pts->SetPoint(0,p0);
          pts->SetPoint(1,p1);
    }
  }
}
//------------------------------------------------------------------------------


