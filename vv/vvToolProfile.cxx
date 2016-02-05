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
  connect(mCurrentSlicerManager, SIGNAL(callAddLandmark(float,float,float,float)), mCurrentSlicerManager, SLOT(AddLandmark(float,float,float,float)));
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
      //DisplayLine();
  }
  else
      mSaveProfileButton->setEnabled(false);
  
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
  if (mPoint1Selected)
    mCurrentSlicerManager->GetLandmarks()->RemoveLandmarkWithLabel("P1", mPoint1[3]);
  if (mPoint2Selected)
    mCurrentSlicerManager->GetLandmarks()->RemoveLandmarkWithLabel("P2", mPoint2[3]);

    
  if (mCurrentSlicerManager)
    mCurrentSlicerManager->Render();
    
  delete [] mPoint1;
  delete [] mPoint2;
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

  mPoint1 = new int[4];
  mPoint1[0] = mPoint1[1] = mPoint1[2] = mPoint1[3] = 0;
  mPoint2 = new int[4];
  mPoint2[0] = mPoint2[1] = mPoint2[2] = mPoint2[3] = 0;
  
  mSaveProfileButton->setEnabled(false);
  mTextFileName = "Profile.txt";
  mImageLine = vvImage::New();
  
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
  reject();
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
void vvToolProfile::DisplayLine()
{ 
  if (!mPoint1Selected && !mPoint2Selected)
    return;
  
  if(mCurrentSlicerManager) {
      if(mCurrentSlicerManager->GetSelectedSlicer() != -1) {
          double *pos;
          pos = new double [4];
          pos[0] = pos[1] = pos[2] = pos[3] = 0;
          
          int i(0);
          while (i<3) {
            pos[i] = mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetCursorPosition()[i];
            ++i;
          }
          if (mCurrentSlicerManager->GetImage()->GetNumberOfDimensions() == 4) {
            pos[3] = mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetTSlice();
          }
          
          /*double p0[4];
          if (mPoint1Selected) {
              p0[0] = mPoint1[0]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[0] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[0];
              p0[1] = mPoint1[1]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[1] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[1];
              p0[2] = mPoint1[2]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[2] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[2];
              p0[3] = mPoint1[3];
          }
          else {
              p0[0] = mPoint2[0]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[0] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[0];
              p0[1] = mPoint2[1]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[1] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[1];
              p0[2] = mPoint2[2]*mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetSpacing()[2] + mCurrentSlicerManager->GetSlicer(mCurrentSlicerManager->GetSelectedSlicer())->GetInput()->GetOrigin()[2];
              p0[3] = mPoint2[3];
          }
          vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
          pts->InsertNextPoint(p0);
          pts->InsertNextPoint(pos);
 
          vtkSmartPointer<vtkPolyData> linesPolyData = vtkSmartPointer<vtkPolyData>::New();
          linesPolyData->SetPoints(pts);
 
          vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
          line->GetPointIds()->SetId(0, 0); // the second 0 is the index of the Origin in linesPolyData's points
          line->GetPointIds()->SetId(1, 1); // the second 1 is the index of P0 in linesPolyData's points
 
          vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
          lines->InsertNextCell(line);
 
          linesPolyData->SetLines(lines);
 
          unsigned char red[3] = { 255, 0, 0 };
 
          vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
          colors->SetNumberOfComponents(3);
          colors->InsertNextTupleValue(red);
 
          linesPolyData->GetCellData()->SetScalars(colors);
 
 
          vtkSmartPointer<vtkPolyDataMapper> lineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
          lineMapper->SetInput(linesPolyData);
#else
          lineMapper->SetInputData(linesPolyData);
#endif
          vtkSmartPointer<vtkActor> lineActor = vtkSmartPointer<vtkActor>::New();
          lineActor->SetMapper(lineMapper);
          lineActor->GetProperty()->SetOpacity(0.995);  //in order to get VTK to turn on the alpha-blending in OpenGL
        
          for(int i=0;i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
              mCurrentSlicerManager->GetSlicer(i)->GetRenderer()->AddActor(lineActor);
          }*/
          
          mOverlayActors.clear();
          for(int i=0;i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
              mOverlayActors.push_back(vvBinaryImageOverlayActor::New());
              mOverlayActors[i]->SetImage(mImageLine, 0);
              mOverlayActors[i]->SetColor(1,0,0);
              mOverlayActors[i]->SetOpacity(0.995); //in order to get VTK to turn on the alpha-blending in OpenGL
              mOverlayActors[i]->SetSlicer(mCurrentSlicerManager->GetSlicer(i));
              mOverlayActors[i]->Initialize(true);
              mOverlayActors[i]->SetDepth(1);
              mOverlayActors[i]->ShowActors();
              mOverlayActors[i]->UpdateSlice(i, mCurrentSlicerManager->GetSlicer(i)->GetSlice(), false);
          }
      }
  }
  mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------

