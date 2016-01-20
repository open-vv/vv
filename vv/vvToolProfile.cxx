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

// vv
#include "vvToolProfile.h"
#include "vvProgressDialog.h"
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

#include <vtkLine.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkBox.h>
#include <vtkInformation.h>

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
  delete [] mPoint1;
  delete [] mPoint2;
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
      mCurrentSlicerManager->GetLandmarks()->RemoveLandmarkWithLabel("P1", mPoint1[3]);
  }
  
  mPoint1Selected = false;
  if(mCurrentSlicerManager) {
      if(mCurrentSlicerManager->GetSelectedSlicer() != -1) {
          double *pos;
          int *index;
          pos = new double [4];
          pos[0] = pos[1] = pos[2] = pos[3] = 0;
          index = new int [mCurrentSlicerManager->GetImage()->GetNumberOfDimensions()];
          
          int i(0);
          while (i<mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() && i<3) {
            pos[i] = mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetCursorPosition()[i];
            index[i] = (int) (pos[i] - mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[i])/mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[i];
            position += QString::number(pos[i],'f',1) + " ";
            mPoint1[i] = index[i];
            ++i;
          }
          if (mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() == 4) {
            pos[3] = mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetTSlice();
            index[3] = (int)pos[3];
            position += QString::number(pos[3],'f',1) + " ";
            mPoint1[3] = index[3];
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
      mCurrentSlicerManager->GetLandmarks()->RemoveLandmarkWithLabel("P2", mPoint2[3]);
  }
  
  mPoint2Selected = false;
  if(mCurrentSlicerManager) {
      if(mCurrentSlicerManager->GetSelectedSlicer() != -1) {
          double *pos;
          int *index;
          pos = new double [4];
          pos[0] = pos[1] = pos[2] = pos[3] = 0;;
          index = new int [mCurrentSlicerManager->GetImage()->GetNumberOfDimensions()];
          
          int i(0);
          while (i<mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() &&i<3) {
            pos[i] = mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetCursorPosition()[i];
            index[i] = (int) (pos[i] - mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[i])/mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[i];
            position += QString::number(pos[i],'f',1) + " ";
            mPoint2[i] = index[i];
            ++i;
          }
          if (mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() == 4) {
            pos[3] = mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetTSlice();
            index[3] = (int)pos[3];
            position += QString::number(pos[3],'f',1) + " ";
            mPoint2[3] = index[3];
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
    //mImageLine = mFilter->GetOutputVVImage();
    
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
  this->ProfileWidget->GetRenderWindow()->GetRenderers()->RemoveAllItems();
  this->ProfileWidget->GetRenderWindow()->AddRenderer(mView->GetRenderer());
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
  for(int i=0;i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
    mCurrentSlicerManager->GetSlicer(i)->GetRenderer()->RemoveActor(mLineActors[i]);
  }

  if (mPoint1Selected)
    mCurrentSlicerManager->GetLandmarks()->RemoveLandmarkWithLabel("P1", mPoint1[3]);
  if (mPoint2Selected)
    mCurrentSlicerManager->GetLandmarks()->RemoveLandmarkWithLabel("P2", mPoint2[3]);

    
  if (mCurrentSlicerManager)
    mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolProfile::close()
{ 
  //RemoveVTKObjects();
  
  connect(mCurrentSlicerManager, SIGNAL(callAddLandmark(float,float,float,float)), mCurrentSlicerManager, SLOT(AddLandmark(float,float,float,float)));
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

  mPoint1 = new int[4];
  mPoint1[0] = mPoint1[1] = mPoint1[2] = mPoint1[3] = 0;
  mPoint2 = new int[4];
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
        ofstream fileOpen(mTextFileName.c_str(), std::ofstream::trunc);
  
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
        
        fileOpen << "Id" << "\t" << "Value" << "\t" ;
        fileOpen << "x(vox)" << "\t" << "y(vox)" << "\t";
        if (mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() >=3)
            fileOpen << "z(vox)" << "\t";
        if (mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() >=4)
            fileOpen << "t" << "\t";
        fileOpen << "x(mm)" << "\t" << "y(mm)" << "\t";
        if (mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() >=3)
            fileOpen << "z(mm)" << "\t";
        if (mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() >=4)
            fileOpen << "t" << "\t";
        fileOpen << endl;
   
        while (i<arrX->GetNumberOfTuples()) {
            fileOpen << arrX->GetTuple(i)[0] << "\t" << arrY->GetTuple(i)[0] << "\t" ;
      
            coords->GetTuple(i, tuple);
            for (int j=0; j<mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() ; ++j) {
                fileOpen << tuple[j] << "\t" ;
            }
            int j(0);
            while (j<mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() && j<3) {
                fileOpen << tuple[j]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[j]+mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[j] << "\t" ;
                ++j;
            }
            if (mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() == 4) {
                fileOpen << tuple[3] << "\t" ;
            }
            fileOpen << endl;
            ++i;
        }
  
        delete [] tuple;

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
            vtkSmartPointer<vtkBox> clippingBox = vtkSmartPointer<vtkBox>::New();
            double extent[6];
            for (int j=0; j<6; ++j) {
                extent[j] = mCurrentSlicerManager->GetSlicer(slicer)->GetExtent()[j];
            }
            extent[2*mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()] = mCurrentSlicerManager->GetSlicer(slicer)->GetImageActor()->GetBounds()[ mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()*2 ]-fabs(mCurrentSlicerManager->GetSlicer(slicer)->GetInput()->GetSpacing()[mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()]);
            extent[2*mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()+1] = mCurrentSlicerManager->GetSlicer(slicer)->GetImageActor()->GetBounds()[ mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()*2+1 ]+fabs(mCurrentSlicerManager->GetSlicer(slicer)->GetInput()->GetSpacing()[mCurrentSlicerManager->GetSlicer(slicer)->GetOrientation()]);
            clippingBox->SetBounds(extent);
            
            vtkSmartPointer<vtkClipPolyData> clipper = vtkSmartPointer<vtkClipPolyData>::New();
            clipper->SetClipFunction(clippingBox);
#if VTK_MAJOR_VERSION <= 5
            clipper->SetInput(mLinesPolyData);
#else
            clipper->SetInputData(mLinesPolyData);
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
      colors->InsertNextTupleValue(red);
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
          p0[0] = mPoint1[0]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[0] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[0];
          p0[1] = mPoint1[1]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[1] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[1];
          p0[2] = mPoint1[2]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[2] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[2];
          p0[3] = mPoint1[3];

          p1[0] = mPoint2[0]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[0] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[0];
          p1[1] = mPoint2[1]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[1] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[1];
          p1[2] = mPoint2[2]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[2] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[2];
          p1[3] = mPoint2[3];
              
          vtkSmartPointer<vtkPoints> pts = mLinesPolyData->GetPoints();
          pts->SetPoint(0,p0);
          pts->SetPoint(1,p1);
    }
  }
}
//------------------------------------------------------------------------------


