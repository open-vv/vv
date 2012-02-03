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
#include "clitkRegionGrowing_ggo.h"
#include "clitkRegionGrowingGenericFilter.h"

// Qt
#include <QFileDialog>
#include <QMessageBox>

// vtk
#include "vtkImageContinuousErode3D.h"
#include "vtkImageContinuousDilate3D.h"
#include "vtkRenderWindow.h"

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
  mCurrentLabelUnderMousePointer = 0.0;
  mCurrentMousePositionInMM.resize(3);
  //  mCurrentMousePositionInPixel.resize(3);
  mRefMaskImage = NULL;
  mCurrentState = State_Default;
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
  DD("remo ref");
  if (mRefMaskActor) mRefMaskActor->RemoveActors();
  DD("remo mask");
  if (mCurrentMaskActor) mCurrentMaskActor->RemoveActors();
  for(int i=0; i<mCurrentCCLActors.size(); i++) {
    if (mCurrentCCLActors[i]) mCurrentCCLActors[i]->RemoveActors();
  }
  DD("wclose");
  QWidget::close();  
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
  connect(mCurrentSlicerManager, SIGNAL(KeyPressedSignal(std::string)),
          this, SLOT(KeyPressed(std::string)));
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

  // Add a new roi actor for the current mask
  mCurrentMaskActor = CreateMaskActor(mCurrentMaskImage, 1, 0, false);
  mCurrentMaskActor->Update(); // default color is red
  UpdateMaskSize(mCurrentMaskImage, mCurrentMaskSizeInPixels, mCurrentMaskSizeInCC);  

  // Add a mask actor for the reference
  mRefMaskActor = CreateMaskActor(mRefMaskImage, 0, 1, true);
  mRefMaskActor->SetContourVisible(true);
  mRefMaskActor->SetVisible(false);
  mRefMaskActor->SetContourColor(0,1,0); // green contour
  mRefMaskActor->UpdateColor();
  mRefMaskActor->Update();
  UpdateMaskSize(mRefMaskImage, mRefMaskSizeInPixels, mRefMaskSizeInCC);  

  // Update GUI
  UpdateMaskSizeLabels();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::UpdateMaskSizeLabels()
{
  QString s("%1 pix (%2 cm3)");
  s = s.arg(mRefMaskSizeInPixels).arg(mRefMaskSizeInCC);
  mRefMaskSizeLabel->setText(s);
  QString s2("%1 pix (%2 cm3)");
  s2 = s2.arg(mCurrentMaskSizeInPixels).arg(mCurrentMaskSizeInCC);
  mCurrentMaskSizeLabel->setText(s2);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::UpdateMaskSize(vvImage::Pointer image, long & pix, double & cc)
{
  pix = ComputeNumberOfPixels(image, GetForegroundValue());
  double vol = image->GetSpacing()[0]*image->GetSpacing()[1]*image->GetSpacing()[2];
  cc = pix * vol / (10*10*10);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
long vvToolSegmentation::ComputeNumberOfPixels(vvImage::Pointer image, double value) 
{
  int n=0;
  vtkImageData * im = image->GetFirstVTKImageData();
  char * pPix = (char*)im->GetScalarPointer(); // FIXME char ?
  for(uint i=0; i<im->GetNumberOfPoints(); i++) {
    if (pPix[i] == value) n++;
  }
  DD(n);
  return n;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::KeyPressed(std::string KeyPress)
{ 
  if (KeyPress == "G") {
    RegionGrowing();
  }
  if (KeyPress == "e") {
    Erode();
  }
  if (KeyPress == "d") {
    Dilate(); // FIXME -> extend image BB !!
  }
  if (KeyPress == "L") {
    Labelize(); 
  }
  if (KeyPress == "m") {
    Merge(); 
    UpdateAndRenderNewMask();
  }
  if (KeyPress == "s") { // Supress "Remove" one label
    if (mCurrentState == State_CCL) RemoveLabel();
  }
  if (KeyPress == "t") { // display remove ref contour
    mRefMaskActor->SetContourVisible(!mRefMaskActor->IsContourVisible());
    mRefMaskActor->UpdateColor();
    mCurrentSlicerManager->Render();
  }
  if (KeyPress == "w") {
    vvImageWriter::Pointer writer = vvImageWriter::New();
    writer->SetOutputFileName("a.mha");
    writer->SetInput(mCurrentMaskImage);
    writer->Update();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::RegionGrowing()
{
  DD("RegionGrowing");
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  // Merge labels if needed
  Merge();
  // Get mouse location
  DD(mCurrentLabelUnderMousePointer); 
  DDV(mCurrentMousePositionInMM, 3);
  //  DDV(mCurrentMousePositionInPixel, 3);

  // Build RG filter parameters
  typedef args_info_clitkRegionGrowing ArgsInfoType;
  ArgsInfoType a;
  cmdline_parser_clitkRegionGrowing_init(&a);
  // FIXME parameters
  a.type_arg = 4;  // FIXME set by gui 
  DD(a.lower_arg);
  a.lower_arg = 200;
  a.upper_arg = 3000;
  a.seed_given = 3;
  a.seed_arg = new int[3];

  DDV(mCurrentMousePositionInMM, 3);
  vtkImageData * image = mCurrentImage->GetFirstVTKImageData();
  double x = (mCurrentMousePositionInMM[0] - image->GetOrigin()[0]) / image->GetSpacing()[0];
  double y = (mCurrentMousePositionInMM[1] - image->GetOrigin()[1]) / image->GetSpacing()[1];
  double z = (mCurrentMousePositionInMM[2] - image->GetOrigin()[2]) / image->GetSpacing()[2];
  a.seed_arg[0] = x;
  a.seed_arg[1] = y;
  a.seed_arg[2] = z;
  a.verbose_flag = 1;

  // Build RG filter parameters
  typedef clitk::RegionGrowingGenericFilter<args_info_clitkRegionGrowing> FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetArgsInfo(a);
  filter->SetInputVVImage(mCurrentImage);
  filter->SetIOVerbose(true);  
  filter->Update();
  mCurrentMaskImage = filter->GetOutputVVImage();
  DD("filter done");

  mCurrentMaskActor->RemoveActors();
  mCurrentMaskActor = CreateMaskActor(mCurrentMaskImage, 1, 0, false);
  mCurrentMaskActor->Update(); // default color is red
  UpdateAndRenderNewMask();
  DD("end");
  
  // mouse pointer
  QApplication::restoreOverrideCursor();
}  
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::Merge()
{
  if (mCurrentState != State_CCL) return;
  
  DD("Merge");
  // Remove actors
  for(int i=0; i<mCurrentCCLActors.size(); i++) {
    if (mCurrentCCLActors[i]) {
      mCurrentCCLActors[i]->SetVisible(false);
      mCurrentCCLActors[i]->RemoveActors();
    }
  }
  mCurrentCCLActors.clear();

  // Compute new mask
  vtkImageData * ccl  = mCurrentCCLImage->GetFirstVTKImageData();
  vtkImageData * mask = mCurrentMaskImage->GetFirstVTKImageData();
  int * pCCL = (int*)ccl->GetScalarPointer();
  char * pPix = (char*)mask->GetScalarPointer();
  for(uint i=0; i<ccl->GetNumberOfPoints(); i++) {
    if (pCCL[i] == 0) pPix[i] = GetBackgroundValue(); // copy BG. In CCL BG is always 0
  }

  // Display new mask and remove ccl
  mCurrentCCLImage->Reset();
  mCurrentMaskActor->RemoveActors(); // kill it
  mCurrentMaskActor = CreateMaskActor(mCurrentMaskImage, 1, 0, false); // renew
  mCurrentMaskActor->Update();
  mCurrentMaskActor->SetVisible(true); 
  // mCurrentSlicerManager->Render();
  mCurrentState = State_Default;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::Erode()
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  // Merge labels if needed
  Merge();
  // Get image and start erosion
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
  // Merge labels if needed
  Merge();
  // Get image and start dilatation
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
  UpdateMaskSize(mCurrentMaskImage, mCurrentMaskSizeInPixels, mCurrentMaskSizeInCC);
  UpdateMaskSizeLabels();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolSegmentation::Labelize()
{
  if (mCurrentState == State_CCL) return; // Do nothing in this case
  DD("Labelize");
  // Waiting cursos
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  mCurrentMaskActor->SetVisible(false);
  
  // Build CCL filter
  vtkImageData* image = mCurrentMaskImage->GetVTKImages()[0];
  typedef args_info_clitkConnectedComponentLabeling ArgsInfoType;
  ArgsInfoType a;
  cmdline_parser_clitkConnectedComponentLabeling_init(&a);
  a.inputBG_arg = GetBackgroundValue();
  a.full_flag = false;  // FIXME set by gui
  a.minSize_arg = 100;  // FIXME set by gui 
  typedef clitk::ConnectedComponentLabelingGenericFilter<ArgsInfoType> FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetArgsInfo(a);
  filter->SetInputVVImage(mCurrentMaskImage); // FIXME Check type is ok ? convert float ?
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
  mCurrentCCLActors.clear();
  for(int i=1; i<=std::min(n,10); i++) { // Start at 1 because 0 is BG. FIXME max by gui
    QSharedPointer<vvROIActor> actor = CreateMaskActor(mCurrentCCLImage, i, i+1, false); 
    mCurrentCCLActors.push_back( actor );
    actor->Update();    
  }
  //  mCurrentMaskActor->Update();
  mCurrentSlicerManager->Render();
  
  // UpdateAndRender();
  mCurrentState = State_CCL;
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
  // DD("MousePositionChanged ");
  // DD(slicer);
  double x = mCurrentSlicerManager->GetSlicer(slicer)->GetCurrentPosition()[0];
  double y = mCurrentSlicerManager->GetSlicer(slicer)->GetCurrentPosition()[1];
  double z = mCurrentSlicerManager->GetSlicer(slicer)->GetCurrentPosition()[2];
  mCurrentMousePositionInMM[0] = x;
  mCurrentMousePositionInMM[1] = y;
  mCurrentMousePositionInMM[2] = z;
  // DDV(mCurrentMousePositionInMM, 3);

  //  vtkImageData * image = mCurrentCCLImage->GetFirstVTKImageData();
  vtkImageData * image = mCurrentMaskImage->GetFirstVTKImageData();
  double Xover = (x - image->GetOrigin()[0]) / image->GetSpacing()[0];
  double Yover = (y - image->GetOrigin()[1]) / image->GetSpacing()[1];
  double Zover = (z - image->GetOrigin()[2]) / image->GetSpacing()[2];
  int ix, iy, iz;
  
  // mCurrentMousePositionInPixel[0] = Xover;
  // mCurrentMousePositionInPixel[1] = Yover;
  // mCurrentMousePositionInPixel[2] = Zover;
  // DDV(mCurrentMousePositionInPixel, 3);

  if (Xover >= image->GetWholeExtent()[0] &&
      Xover <= image->GetWholeExtent()[1] &&
      Yover >= image->GetWholeExtent()[2] &&
      Yover <= image->GetWholeExtent()[3] &&
      Zover >= image->GetWholeExtent()[4] &&
      Zover <= image->GetWholeExtent()[5]) {
    if (mCurrentState == State_Default) { // inside the mask
      mCurrentLabelUnderMousePointer = 1;
      return; 
    }
    else { // inside the label image
      vtkImageData * image = mCurrentCCLImage->GetFirstVTKImageData();
      mCurrentLabelUnderMousePointer = 
        mCurrentSlicerManager->GetSlicer(0)->GetScalarComponentAsDouble(image, Xover, Yover, Zover, ix, iy, iz, 0);
      return;
    }
  }
  else {
    // DD("out of mask");
    mCurrentLabelUnderMousePointer = 0; // BG is always 0 in CCL
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::RemoveLabel() {
  DD("RemoveLabel");
  if (mCurrentLabelUnderMousePointer == 0) return;
  // First actor=0 and is label 1. Label 0 is not an actor, it is BG
  int actorNumber = mCurrentLabelUnderMousePointer-1; 
  // Set actor invisible
  mCurrentCCLActors[actorNumber]->SetVisible(false);
  mCurrentSlicerManager->Render();
  // Set image label
  vtkImageData * image = mCurrentCCLImage->GetFirstVTKImageData();
  int * pPix = (int*)image->GetScalarPointer();
  int n = 0;
  for(uint i=0; i<image->GetNumberOfPoints(); i++) {
    if (pPix[i] == mCurrentLabelUnderMousePointer) pPix[i] = 0;
    if (pPix[i] != 0) n++; // count the number of pixels in the foreground
  }
  // Update mask size
  mCurrentMaskSizeInPixels = n;
  mCurrentMaskSizeInCC = mCurrentMaskImage->GetSpacing()[0] * mCurrentMaskImage->GetSpacing()[1] * mCurrentMaskImage->GetSpacing()[2] * n / (10*10*10);
  UpdateMaskSizeLabels();
}
//------------------------------------------------------------------------------
