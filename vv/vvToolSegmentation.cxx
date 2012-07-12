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
  mKernelValue = 3; // FIXME must be odd. If even -> not symmetrical
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolSegmentation::~vvToolSegmentation()
{
  DD("destructor");
  mRefMaskActor->RemoveActors();
  QWidget::close();  
  mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolSegmentation::close()
{
  DD("close");
  mRefMaskActor->RemoveActors();
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
  mRefMaskActor = QSharedPointer<vvROIActor>(new vvROIActor);
  mCurrentMaskActor = QSharedPointer<vvROIActor>(new vvROIActor);
  std::vector<double> color;
  color.push_back(1);
  color.push_back(0);
  color.push_back(0);
  clitk::DicomRT_ROI::Pointer roi = clitk::DicomRT_ROI::New();
  roi->SetFromBinaryImage(mRefMaskImage, 1, std::string("toto"), color, filename.toStdString());
  mRefMaskActor->SetBGMode(true);
  mRefMaskActor->SetROI(roi);
  mRefMaskActor->SetSlicerManager(mCurrentSlicerManager);
  mRefMaskActor->Initialize(10, true);
  mRefMaskActor->SetContourVisible(true);
  mRefMaskActor->SetVisible(false);
  mRefMaskActor->Update();

  clitk::DicomRT_ROI::Pointer roi2 = clitk::DicomRT_ROI::New();
  roi2->SetFromBinaryImage(mCurrentMaskImage, 1, std::string("toto"), color, filename.toStdString());
  mCurrentMaskActor->SetBGMode(true);
  mCurrentMaskActor->SetROI(roi2);
  mCurrentMaskActor->SetSlicerManager(mCurrentSlicerManager);
  mCurrentMaskActor->Initialize(10, true);
  mCurrentMaskActor->Update();

  // Prepare widget to get keyboard event
  grabKeyboard();  
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::keyPressEvent(QKeyEvent * event)
{
  vvToolWidgetBase::keyPressEvent(event);
  //DD("key");
  
  if (event->text() == "e") {
    Erode();
  }
  if (event->text() == "d") {
    Dilate(); // FIXME -> extend image BB !!
  }
  if (event->text() == "s") {
    vvImageWriter::Pointer writer = vvImageWriter::New();
    writer->SetOutputFileName("a.mha");
    writer->SetInput(mCurrentMaskImage);
    writer->Update();
  }
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
  UpdateAndRender();
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
  UpdateAndRender();
  dilate->Delete();
  QApplication::restoreOverrideCursor();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSegmentation::UpdateAndRender()
{
  bool visible = mCurrentMaskActor->IsVisible();
  bool cvisible = mCurrentMaskActor->IsContourVisible();
  mCurrentMaskActor->SetVisible(false);
  mCurrentMaskActor->SetContourVisible(false);
  // mCurrentSlicerManager->Render();

  //mCurrentMaskActor->RemoveActors();
  mCurrentMaskActor->UpdateImage();
  mCurrentMaskActor->SetVisible(visible);
  mCurrentMaskActor->SetContourVisible(cvisible);
  mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------
