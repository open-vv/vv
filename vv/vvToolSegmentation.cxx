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
#include "vvToolSegmentation.h"
#include "vvSlicerManager.h"
#include "vvSlicer.h"
#include "vvToolInputSelectorWidget.h"
#include "vvImageWriter.h"

// clitk
#include "clitkConnectedComponentLabeling_ggo.h"
#include "clitkConnectedComponentLabelingGenericFilter.h"

// Qt
#include <QFileDialog>
#include <QMessageBox>

// vtk
#include "vtkImageContinuousErode3D.h"
#include "vtkImageContinuousDilate3D.h"

//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolSegmentation);
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::Initialize()
{
  SetToolName("Segmentation");
  SetToolMenuName("Interactive Segmentation");
  SetToolIconFilename(":/common/icons/ducky.ico");
  SetToolTip("Image interactive segmentation (trial).");
  SetToolExperimental(true);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolSegmentation::vvToolSegmentation(vvMainWindowBase * parent, Qt::WindowFlags f)
  :vvToolWidgetBase(parent,f),
   vvToolBase<vvToolSegmentation>(parent),
   Ui::vvToolSegmentation()
{
  // GUI Initialization
  Ui_vvToolSegmentation::setupUi(mToolWidget);
  setAttribute(Qt::WA_DeleteOnClose);

  // Set how many inputs are needed for this tool
  AddInputSelector("Select one image");
  
  // Init
  mRefMaskImage = NULL;
  mCurrentMode = Mode_Default;
  mKernelValue = 3; // FIXME must be odd. If even -> not symmetrical
  mDefaultLUTColor = vtkSmartPointer<vtkLookupTable>::New();
  mDefaultLUTColor->SetNumberOfTableValues(256);
#include "vvDefaultLut.h"
  
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolSegmentation::~vvToolSegmentation()
{
  DD("destructor");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolSegmentation::close()
{
  DD("close");
  mRefMaskActor->RemoveActors();
  DD("la");
  mCurrentMaskActor->RemoveActors();
  DD("here");
  QWidget::close();  
  DD("toto");
  mCurrentSlicerManager->Render();
  return true;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::InputIsSelected(vvSlicerManager * m)
{
  DD("InputIsSelected");
  mCurrentSlicerManager = m;
  mCurrentImage = mCurrentSlicerManager->GetImage();

  // Refuse if non 3D image
  if (mCurrentImage->GetNumberOfDimensions() != 3) {
    QMessageBox::information(this,tr("Sorry only 3D yet"), tr("Sorry only 3D yet"));
    close();
    return;
  }

  // Change gui
  //mLabelInputInfo->setText(QString("%1").arg(m->GetFileName().c_str()));

  // Open mask
  OpenBinaryImage();
  
  // If cancel: stop
  if (mRefMaskActor == NULL) {
    close();
    return;
  }

  // Update gui
  mToolInputSelectionWidget->hide();

  // Connect mouse position
  connect(mCurrentSlicerManager, SIGNAL(MousePositionUpdatedSignal(int)),
          this, SLOT(MousePositionChanged(int)));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::apply()
{
  DD("apply");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::OpenBinaryImage()
{
  DD("OpenBinaryImage");

  // Load browser and select image
  QString Extensions = "Images files ( *.mha *.mhd *.hdr *.his)";
  Extensions += ";;All Files (*)";
  QString filename =
    QFileDialog::getOpenFileName(this,tr("Open binary image"),
                                 mMainWindowBase->GetInputPathName(),Extensions);
  DD(filename.toStdString());
  if (filename.size() == 0) return;
  
  // Open Image
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  vvImageReader::Pointer reader = vvImageReader::New();
  std::vector<std::string> filenames;
  filenames.push_back(filename.toStdString());
  reader->SetInputFilenames(filenames);
  reader->Update(vvImageReader::IMAGE);
  QApplication::restoreOverrideCursor();
  
  if (reader->GetLastError().size() != 0) {
    std::cerr << "Error while reading " << filename.toStdString() << std::endl;
    QString error = "Cannot open file \n";
    error += reader->GetLastError().c_str();
    QMessageBox::information(this,tr("Reading problem"),error);
    return;
  }

  mRefMaskImage = reader->GetOutput();
  int dim = mRefMaskImage->GetNumberOfDimensions();
  if (dim != 3 ) {
    QMessageBox::information(this,tr("Sorry only 3D yet"), tr("Sorry only 3D yet"));
    close();
    return;
  }

  reader = vvImageReader::New();
  reader->SetInputFilenames(filenames);
  reader->Update(vvImageReader::IMAGE);
  mCurrentMaskImage = reader->GetOutput();

  // Add a new roi actor
  mRefMaskActor = CreateMaskActor(mRefMaskImage, 0, 0, true);
  mRefMaskActor->SetContourVisible(true);
  mRefMaskActor->SetVisible(false);
  mRefMaskActor->SetContourColor(0,1,0); // green contour
  mRefMaskActor->UpdateColor();
  mRefMaskActor->Update();

  mCurrentMaskActor = CreateMaskActor(mCurrentMaskImage, 1, 1, false);
  mCurrentMaskActor->SetOverlayColor(1,0,0); // red roi
  mRefMaskActor->UpdateColor();
  mCurrentMaskActor->Update();

  // Prepare widget to get keyboard event. With this method, the key
  //  only work when the mouse focus is on the dialog
  DD("here installe");
  this->installEventFilter(this);
  //grabKeyboard();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//void vvToolSegmentation::keyPressEvent(QKeyEvent * event)
bool vvToolSegmentation::eventFilter(QObject *object, QEvent * e)
{
  // DD("key");
  //vvToolWidgetBase::keyPressEvent(event);
  
  if (/*object == form &&*/ e->type() == QEvent::KeyPress) {
    QKeyEvent * event = static_cast<QKeyEvent *>(e);
    
    if (event->text() == "e") {
      Erode();
    }
    if (event->text() == "d") {
      Dilate(); // FIXME -> extend image BB !!
    }
    if (event->text() == "l") {
      Labelize(); 
    }
    if (event->text() == "r") { // "Remove" one label
      if (mCurrentMode == Mode_CCL) RemoveLabel();
    }
    if (event->text() == "s") {
      vvImageWriter::Pointer writer = vvImageWriter::New();
      writer->SetOutputFileName("a.mha");
      writer->SetInput(mCurrentMaskImage);
      writer->Update();
    }
    //mMainWindow->keyPressEvent(event);
    vvToolWidgetBase::keyPressEvent(event);
  }         
  return QObject::eventFilter(object, e);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::Erode()
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  vtkImageContinuousErode3D* erode = vtkImageContinuousErode3D::New();
  erode->SetKernelSize(mKernelValue,mKernelValue,mKernelValue);
  vtkImageData* image = mCurrentMaskImage->GetVTKImages()[0];
  erode->SetInput(image);
  erode->Update();
  image->DeepCopy(erode->GetOutput());
  image->Update();
  UpdateAndRenderNewMask();
  erode->Delete();
  QApplication::restoreOverrideCursor();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::Dilate()
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  vtkImageContinuousDilate3D* dilate = vtkImageContinuousDilate3D::New();
  dilate->SetKernelSize(mKernelValue,mKernelValue,mKernelValue);
  vtkImageData* image = mCurrentMaskImage->GetVTKImages()[0];
  dilate->SetInput(image);
  dilate->Update();
  image->DeepCopy(dilate->GetOutput());
  image->Update();
  UpdateAndRenderNewMask();
  dilate->Delete();
  QApplication::restoreOverrideCursor();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::UpdateAndRenderNewMask()
{
  bool visible = mCurrentMaskActor->IsVisible();
  bool cvisible = mCurrentMaskActor->IsContourVisible();
  mCurrentMaskActor->SetVisible(false);
  mCurrentMaskActor->SetContourVisible(false);
  mCurrentMaskActor->UpdateImage();
  mCurrentMaskActor->SetVisible(visible);
  mCurrentMaskActor->SetContourVisible(cvisible);

  mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolSegmentation::Labelize()
{
  DD("Labelize");
  // Waiting cursos
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  
  // Build CCL filter
  vtkImageData* image = mCurrentMaskImage->GetVTKImages()[0];
  typedef args_info_clitkConnectedComponentLabeling ArgsInfoType;
  ArgsInfoType a;
  cmdline_parser_clitkConnectedComponentLabeling_init(&a);
  a.inputBG_arg = 0;
  a.full_flag = false;  // FIXME set by gui
  a.minSize_arg = 100;  // FIXME set by gui 
  typedef clitk::ConnectedComponentLabelingGenericFilter<ArgsInfoType> FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetArgsInfo(a);
  filter->SetInputVVImage(mCurrentMaskImage);
  filter->SetIOVerbose(true);  
  filter->Update();
  DD(filter->GetOriginalNumberOfObjects());
  DD(filter->GetSizeOfObjectsInPixels().size());
  mCurrentCCLImage = filter->GetOutputVVImage();
  DDV(filter->GetSizeOfObjectsInPixels(), filter->GetSizeOfObjectsInPixels().size());
  DD("filter done");

  /*
  // DEBUG
  vvImageWriter::Pointer writer = vvImageWriter::New();
  writer->SetInput(mCurrentCCLImage);
  writer->SetOutputFileName("bidon-ccl.mha");
  writer->Update(); 
  DD(mCurrentCCLImage->IsScalarTypeInteger());
  */
  
  // Create actors 
  int n = filter->GetSizeOfObjectsInPixels().size();
  for(int i=1; i<std::min(n,10); i++) { // Start at 1 because 0 is BG. FIXME max 10
    DD(i);
    QSharedPointer<vvROIActor> actor = CreateMaskActor(mCurrentCCLImage, i, i+1, false); 
    mCurrentCCLActors.push_back( actor );
    actor->Update();    
  }
  mCurrentMaskActor->SetVisible(false);
  mCurrentMaskActor->Update();
  mCurrentSlicerManager->Render();
  
  // UpdateAndRender();
  mCurrentMode = Mode_CCL;
  QApplication::restoreOverrideCursor();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QSharedPointer<vvROIActor> vvToolSegmentation::CreateMaskActor(vvImage::Pointer image, int i, int colorID, bool BGMode)
{
  static int depth = 1;
  depth += 1;
  QSharedPointer<vvROIActor> actor = QSharedPointer<vvROIActor>(new vvROIActor);
  double * color = mDefaultLUTColor->GetTableValue(colorID % mDefaultLUTColor->GetNumberOfTableValues ());
  std::vector<double> c;
  c.push_back(color[0]);
  c.push_back(color[1]);
  c.push_back(color[2]);
  clitk::DicomRT_ROI::Pointer roi = clitk::DicomRT_ROI::New();
  roi->SetFromBinaryImage(image, i, std::string("toto"), c, std::string("titi"));
  if (BGMode) {
    actor->SetBGMode(true);
  }
  else {
    DD("FG mode");
    roi->SetForegroundValueLabelImage(i); // FG mode
    actor->SetBGMode(false); // FG mode
  }
  actor->SetROI(roi);
  actor->SetSlicerManager(mCurrentSlicerManager);
  actor->Initialize(depth+i, true); // +1 to start at 1 not 0
  actor->SetContourVisible(false);
  actor->SetVisible(true);
  return actor;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::MousePositionChanged(int slicer)
{
  if (mCurrentMode == Mode_Default) return; // Do nothing in this case

  double x = mCurrentSlicerManager->GetSlicer(slicer)->GetCurrentPosition()[0];
  double y = mCurrentSlicerManager->GetSlicer(slicer)->GetCurrentPosition()[1];
  double z = mCurrentSlicerManager->GetSlicer(slicer)->GetCurrentPosition()[2];
  vtkImageData * image = mCurrentCCLImage->GetFirstVTKImageData();
  double Xover = (x - image->GetOrigin()[0]) / image->GetSpacing()[0];
  double Yover = (y - image->GetOrigin()[1]) / image->GetSpacing()[1];
  double Zover = (z - image->GetOrigin()[2]) / image->GetSpacing()[2];
  int ix, iy, iz;
  
  if (Xover >= image->GetWholeExtent()[0] &&
      Xover <= image->GetWholeExtent()[1] &&
      Yover >= image->GetWholeExtent()[2] &&
      Yover <= image->GetWholeExtent()[3] &&
      Zover >= image->GetWholeExtent()[4] &&
      Zover <= image->GetWholeExtent()[5]) {
    double valueOver = 
      mCurrentSlicerManager->GetSlicer(0)->GetScalarComponentAsDouble(image, Xover, Yover, Zover, ix, iy, iz, 0);
    // DD(Xover); DD(Yover); DD(Zover);
    // DD(ix); DD(iy); DD(iz);
    // DD(valueOver);
  }
  else {
    // DD("out of mask");
  }

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::RemoveLabel() {
  DD("RemoveLabel");
}
//------------------------------------------------------------------------------
