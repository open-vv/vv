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
#include "vvToolHistogram.h"
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

#ifdef Q_OS_OSX
# include "vvOSXHelper.h"
#endif

//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolHistogram);
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolHistogram::Initialize()
{ 
  SetToolName("Histogram");
  SetToolMenuName("Intensity Histogram");
  SetToolIconFilename(":/common/icons/histogram.png");
  SetToolTip("Display the histogram of the image.");
  SetToolExperimental(false);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolHistogram::vvToolHistogram(vvMainWindowBase * parent, Qt::WindowFlags f)
  :vvToolWidgetBase(parent,f),
   vvToolBase<vvToolHistogram>(parent),
   Ui::vvToolHistogram()
{ 
  // GUI Initialization
  Ui_vvToolHistogram::setupUi(mToolWidget);

  // Connect signals & slots
  connect(mSaveHistogramButton, SIGNAL(clicked()), this, SLOT(SaveAs()));

  // Initialize some widget
  HistogramWidget->hide();

  mView = vtkSmartPointer<vtkContextView>::New();
  vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
  chart->SetAutoSize(false);
  chart->SetRenderEmpty(true);
  mView->GetScene()->AddItem(chart);
  this->HistogramWidget->GetRenderWindow()->GetRenderers()->RemoveAllItems();
  this->HistogramWidget->GetRenderWindow()->AddRenderer(mView->GetRenderer());
  HistogramWidget->show();

#ifdef Q_OS_OSX
  disableGLHiDPI(HistogramWidget->winId());
#endif

  // Main filter
  mFilter = clitk::HistogramImageGenericFilter::New();

  // Set how many inputs are needed for this tool
  AddInputSelector("Select one image", mFilter);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolHistogram::~vvToolHistogram()
{ 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolHistogram::computeHistogram()
{
    if (!mCurrentSlicerManager) close();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    GetArgsInfoFromGUI();
    HistogramWidget->hide();

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
    arrX->SetName("Intensity");
    arrY->SetName("#Voxels");

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

    //Upper and Lower lines for window/level
    vtkSmartPointer<vtkTable> tableWindowLevel = vtkSmartPointer<vtkTable>::New();
    vtkSmartPointer<vtkFloatArray> arrXUpperWindowLevel = vtkSmartPointer<vtkFloatArray>::New();
    vtkSmartPointer<vtkFloatArray> arrXLowerWindowLevel = vtkSmartPointer<vtkFloatArray>::New();
    vtkSmartPointer<vtkFloatArray> arrYWindowLevel = vtkSmartPointer<vtkFloatArray>::New();
    arrXUpperWindowLevel->InsertNextTuple1(mMaxWindowLevel);
    arrXUpperWindowLevel->InsertNextTuple1(mMaxWindowLevel);
    arrXLowerWindowLevel->InsertNextTuple1(mMinWindowLevel);
    arrXLowerWindowLevel->InsertNextTuple1(mMinWindowLevel);
    arrYWindowLevel->InsertNextTuple1(0.0);
    arrYWindowLevel->InsertNextTuple1(arrY->GetMaxNorm());
    arrXUpperWindowLevel->SetName("IntensityUp");
    arrXLowerWindowLevel->SetName("IntensityLow");
    arrYWindowLevel->SetName("#Voxels");
    tableWindowLevel->AddColumn(arrXUpperWindowLevel);
    tableWindowLevel->AddColumn(arrXLowerWindowLevel);
    tableWindowLevel->AddColumn(arrYWindowLevel);
    vtkPlot *upperWindowLine = chart->AddPlot(vtkChart::LINE);
    vtkPlot *lowerWindowLine = chart->AddPlot(vtkChart::LINE);
#if VTK_MAJOR_VERSION <= 5
    upperWindowLine->SetInput(tableWindowLevel, 0, 2);
    lowerWindowLine->SetInput(tableWindowLevel, 1, 2);
#else
    upperWindowLine->SetInputData(tableWindowLevel, 0, 2);
    lowerWindowLine->SetInputData(tableWindowLevel, 1, 2);
#endif
    upperWindowLine->SetColor(255, 0, 0, 255);
    lowerWindowLine->SetColor(255, 0, 0, 255);
    upperWindowLine->SetWidth(1.0);
    lowerWindowLine->SetWidth(1.0);

    chart->GetAxis(vtkAxis::LEFT)->SetTitle("#Voxels");
    chart->GetAxis(vtkAxis::BOTTOM)->SetTitle("Intensity");

    this->HistogramWidget->GetRenderWindow()->GetRenderers()->RemoveAllItems();
    this->HistogramWidget->GetRenderWindow()->AddRenderer(mView->GetRenderer());
    HistogramWidget->show();

    //mView->GetInteractor()->Start();

    QApplication::restoreOverrideCursor();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolHistogram::changeWindowLevel()
{
  vtkChartXY* chart = vtkChartXY::New();
  chart = static_cast<vtkChartXY*>(mView->GetScene()->GetItem(0));
  vtkFloatArray* upperArray = vtkFloatArray::New();
  upperArray = static_cast<vtkFloatArray*>(chart->GetPlot(1)->GetInput()->GetColumn(0));
  upperArray->SetTuple1(0, mMaxWindowLevel);
  upperArray->SetTuple1(1, mMaxWindowLevel);
  vtkFloatArray* lowerArray = vtkFloatArray::New();
  lowerArray = static_cast<vtkFloatArray*>(chart->GetPlot(2)->GetInput()->GetColumn(1));
  lowerArray->SetTuple1(0, mMinWindowLevel);
  lowerArray->SetTuple1(1, mMinWindowLevel);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolHistogram::windowLevelChanged()
{
  computeMinMax();
  changeWindowLevel();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolHistogram::RemoveVTKObjects()
{ 
  if (mCurrentSlicerManager)
  {
    connect(mCurrentSlicerManager, SIGNAL(callAddLandmark(float,float,float,float)), mCurrentSlicerManager, SLOT(AddLandmark(float,float,float,float)));

    mCurrentSlicerManager->Render();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolHistogram::close()
{ 
  //RemoveVTKObjects();

  return vvToolWidgetBase::close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolHistogram::closeEvent(QCloseEvent *event)
{ 
  RemoveVTKObjects();
  event->accept();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolHistogram::reject()
{ 
  // DD("vvToolHistogram::reject");
  RemoveVTKObjects();
  return vvToolWidgetBase::reject();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolHistogram::InputIsSelected(vvSlicerManager * m)
{ 
  mCurrentSlicerManager = m;

  mSaveHistogramButton->setEnabled(true);
  mTextFileName = "Histogram.txt";

  //Compute Min & Max for Window/Level
  computeMinMax();

  computeHistogram();

  disconnect(mCurrentSlicerManager, SIGNAL(callAddLandmark(float,float,float,float)), mCurrentSlicerManager, SLOT(AddLandmark(float,float,float,float)));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolHistogram::computeMinMax()
{
  mMinWindowLevel = mCurrentSlicerManager->GetColorLevel() - mCurrentSlicerManager->GetColorWindow()/2.0;
  mMaxWindowLevel = mCurrentSlicerManager->GetColorLevel() + mCurrentSlicerManager->GetColorWindow()/2.0;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolHistogram::computeWindowLevel()
{
  double window = mMaxWindowLevel - mMinWindowLevel;
  double level = (mMaxWindowLevel + mMinWindowLevel)/2.0;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolHistogram::GetArgsInfoFromGUI()
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
  cmdline_parser_clitkHistogramImage_init(&mArgsInfo); // Initialisation to default

  mArgsInfo.verbose_flag = false;

  // Required (even if not used)
  mArgsInfo.input_given = 0;
  mArgsInfo.output_given = 0;

  mArgsInfo.input_arg = new char;
  mArgsInfo.output_arg = new char;

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolHistogram::apply()
{ 
  close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolHistogram::SaveAs()
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
        
        if (!mCurrentSlicerManager) {
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
        int i(0);
        fileOpen << "Value represents the number of voxels around the corresponding intensity (by default the windows size around intensity is log(range))" << endl;
        fileOpen << "Intensity" << "\t" << "Value" << endl;
   
        while (i<arrX->GetNumberOfTuples()) {
            fileOpen << arrX->GetTuple(i)[0] << "\t" << arrY->GetTuple(i)[0] << endl;
            ++i;
        }

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
