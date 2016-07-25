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
#include "vvWipeImage.h"
#include "vvSlicer.h"

// clitk
#include "clitkCropImage_ggo.h"
#include "clitkCropImageGenericFilter.h"
#include "clitkExceptionObject.h"

// qt
#include <QComboBox>
#include <QCursor>
#include <QApplication>
#include <QMessageBox>
#include <QSignalMapper>

// vtk
#include <vtkVersion.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>

//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvWipeImage);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vvWipeImage::vvWipeImage(vvMainWindowBase * parent, Qt::WindowFlags f):
  vvToolWidgetBase(parent, f),
  vvToolBase<vvWipeImage>(parent),
  Ui::vvWipeImage()
{
    vtkSmartPointer<vtkImageRectilinearWipe> mWipe = vtkSmartPointer<vtkImageRectilinearWipe>::New();
    mWipe->SetWipe(0);
    mWipe->SetPosition(256,256);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvWipeImage::~vvWipeImage()
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvWipeImage::setInput(int number, vvImage::Pointer image)
{
  if (image->GetVTKImages().size()) {
    mImage = image;
    mWipe->SetInputData(number, mImage->GetVTKImages()[0]); //[0] pour du 4D ?
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvWipeImage::closeEvent(QCloseEvent *event)
{
  mCurrentSlicerManager->GetImage()->GetTransform()[0]->SetMatrix(mConcatenedTransform);
  for (int i=0; i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
    mCurrentSlicerManager->GetSlicer(i)->ResetCamera();
    mCurrentSlicerManager->GetSlicer(i)->Render();
    mCurrentSlicerManager->UpdateLinkedNavigation( mCurrentSlicerManager->GetSlicer(i) );
  }
  vvToolWidgetBase::closeEvent(event);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvWipeImage::close()
{
  return vvToolWidgetBase::close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvWipeImage::reject()
{
  return vvToolWidgetBase::reject();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvWipeImage::crossPointerChanged()
{
  mWipe->SetPosition(256,256);
  UpdateWipe();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvWipeImage::UpdateWipe()
{
  for(int i=0; i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
    //mCurrentSlicerManager->GetSlicer(i)->SetReducedExtent(mReducedExtent);
    mCurrentSlicerManager->GetSlicer(i)->ForceUpdateDisplayExtent();
    mCurrentSlicerManager->GetSlicer(i)->Render();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvWipeImage::InputIsSelected(vvSlicerManager * slicer)
{
  //Save the current transformation
  mConcatenedTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  mConcatenedTransform->DeepCopy(slicer->GetSlicer(0)->GetConcatenatedTransform()->GetMatrix());
  vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
  matrix->Identity();
  mCurrentSlicerManager->GetImage()->GetTransform()[0]->SetMatrix(matrix);
  for (int i=0; i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
    mCurrentSlicerManager->GetSlicer(i)->ResetCamera();
    mCurrentSlicerManager->GetSlicer(i)->Render();
    mCurrentSlicerManager->UpdateLinkedNavigation( mCurrentSlicerManager->GetSlicer(i) );
  }

  // Change interface according to number of dimension
#if VTK_MAJOR_VERSION <= 5
  int *a = mCurrentImage->GetFirstVTKImageData()->GetWholeExtent();
#else
  int *a = mCurrentImage->GetFirstVTKImageData()->GetInformation()->Get(vtkDataObject::DATA_EXTENT());
#endif

//   Set initial sliders values
  int w_ext[6], imsize[3];
  mCurrentSlicerManager->GetSlicer(0)->GetRegisterExtent(w_ext);

  QSignalMapper* signalMapper = new QSignalMapper(this);
  connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(sliderValueChanged(int)));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvWipeImage::apply()
{
  if (!mCurrentSlicerManager) close();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  // Typedef 
  typedef args_info_clitkCropImage ArgsInfoType;
  typedef clitk::CropImageGenericFilter CropFilterType;
  
  // Get options
  ArgsInfoType mArgsInfo;
  cmdline_parser_clitkCropImage_init(&mArgsInfo); // Initialisation to default
  int n = mCurrentSlicerManager->GetDimension()*2;  // 2D and 3D only
  mArgsInfo.boundingBox_given = n;
  mArgsInfo.boundingBox_arg = new int[n];
  
  // We MUST reset initial extend to input image before using the
  // filter to retrieve the correct image size  ;
  // Main filter
  CropFilterType::Pointer filter = CropFilterType::New();
  filter->SetInputVVImage(mCurrentImage);
  filter->SetArgsInfo(mArgsInfo);

  // Go ! (not threaded) 
  try{
    filter->Update();
  }
  catch(clitk::ExceptionObject & e) {
    DD(e.what());
    QApplication::restoreOverrideCursor();
    delete [] mArgsInfo.boundingBox_arg;
    close();
  }
  std::ostringstream croppedImageName;
  croppedImageName << "Cropped_" << mCurrentSlicerManager->GetSlicer(0)->GetFileName() << ".mhd";
  // Retrieve result and display it
  vvImage::Pointer output = filter->GetOutputVVImage();
  
  output->GetTransform()[0]->SetMatrix(mConcatenedTransform);

  AddImage(output,croppedImageName.str());
  
  // End
  QApplication::restoreOverrideCursor();
  delete [] mArgsInfo.boundingBox_arg;
  close();
}
//------------------------------------------------------------------------------

