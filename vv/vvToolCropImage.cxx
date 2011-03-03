/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/

// vv
#include "vvToolCropImage.h"
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

// vtk
#include <vtkImageClip.h>
#include <vtkImageTranslateExtent.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>

//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolCropImage);
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolCropImage::vvToolCropImage(vvMainWindowBase * parent, Qt::WindowFlags f):
  vvToolWidgetBase(parent, f),
  vvToolBase<vvToolCropImage>(parent),
  Ui::vvToolCropImage()
{

  // GUI Initialization
  Ui_vvToolCropImage::setupUi(mToolWidget);
  // mTLabel2->setHidden(false);
  // tminSlider->setHidden(false);
  // tmaxSlider->setHidden(false);
  // spin_tmin->setHidden(false);
  // spin_tmax->setHidden(false);
  // mLabelTimeCropping->setHidden(false);
  mTLabel1->setHidden(true);
  mTLabel2->setHidden(true);
  tminSlider->setHidden(true);
  tmaxSlider->setHidden(true);
  spin_tmin->setHidden(true);
  spin_tmax->setHidden(true);
  mLabelTimeCropping->setHidden(true);

  // Set how many inputs are needed for this tool
  AddInputSelector("Select one image");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolCropImage::~vvToolCropImage()
{

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::closeEvent(QCloseEvent *event)
{
  // Reset extends
  for(int i=0; i<mExtentSize; i++) mReducedExtent[i] = mInitialExtent[i];
  UpdateExtent();
  event->accept();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolCropImage::close()
{
  return vvToolWidgetBase::close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::reject()
{
  for(int i=0; i<mExtentSize; i++) mReducedExtent[i] = mInitialExtent[i];
  UpdateExtent();
  return vvToolWidgetBase::reject();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::sliderXMinValueChanged(int s)
{
  xmaxSlider->setMinimum(xminSlider->value());
  mReducedExtent[0] = xminSlider->value();
  UpdateExtent();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::sliderXMaxValueChanged(int s)
{
  xminSlider->setMaximum(xmaxSlider->value());
  mReducedExtent[1] = xmaxSlider->value();
  UpdateExtent();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::sliderYMinValueChanged(int s)
{
  ymaxSlider->setMinimum(yminSlider->value());
  mReducedExtent[2] = yminSlider->value();
  UpdateExtent();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::sliderYMaxValueChanged(int s)
{
  yminSlider->setMaximum(ymaxSlider->value());
  mReducedExtent[3] = ymaxSlider->value();
  UpdateExtent();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::sliderZMinValueChanged(int s)
{
  zmaxSlider->setMinimum(zminSlider->value());
  mReducedExtent[4] = zminSlider->value();
  UpdateExtent();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::sliderZMaxValueChanged(int s)
{
  zminSlider->setMaximum(zmaxSlider->value());
  mReducedExtent[5] = zmaxSlider->value();
  UpdateExtent();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::sliderTMinValueChanged(int s) {
  tmaxSlider->setMinimum(tminSlider->value());
  mReducedExtent[6] = tminSlider->value();
  UpdateExtent();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::sliderTMaxValueChanged(int s) {
  tminSlider->setMaximum(tmaxSlider->value());
  mReducedExtent[7] = tmaxSlider->value();
  UpdateExtent();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::UpdateExtent()
{
  for(int i=0; i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
    mCurrentSlicerManager->GetSlicer(i)->SetReducedExtent(mReducedExtent);
    mCurrentSlicerManager->GetSlicer(i)->ForceUpdateDisplayExtent();
    mCurrentSlicerManager->GetSlicer(i)->Render();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::InputIsSelected(vvSlicerManager * slicer)
{

  // Change interface according to number of dimension
  mExtentSize = 2*slicer->GetDimension();
   if (slicer->GetDimension()<4) {
     mTLabel1->setHidden(true);
     mTLabel2->setHidden(true);
     tminSlider->setHidden(true);
     tmaxSlider->setHidden(true);
     spin_tmin->setHidden(true);
     spin_tmax->setHidden(true);
     mLabelTimeCropping->setHidden(true);
   }
  if (slicer->GetDimension()<3) {
    mZLabel1->setHidden(true);
    mZLabel2->setHidden(true);
    zminSlider->setHidden(true);
    zmaxSlider->setHidden(true);
    spin_zmin->setHidden(true);
    spin_zmax->setHidden(true);
  }

  // Record initial extend
  mReducedExtent = new int[mExtentSize];
  mInitialExtent = new int[mExtentSize];
  mReducedExtent = mCurrentSlicerManager->GetImage()->GetFirstVTKImageData()->GetWholeExtent();
  for(int i=0; i<mExtentSize; i++) mInitialExtent[i] = mReducedExtent[i];
  for(int i=0; i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
    mCurrentSlicerManager->GetSlicer(i)->EnableReducedExtent(true);
  }

  // Set initial sliders values
  std::vector<int> imsize = mCurrentSlicerManager->GetImage()->GetSize();
  xminSlider->setMaximum(imsize[0]-1);
  xmaxSlider->setMaximum(imsize[0]-1);
  xmaxSlider->setValue(imsize[0]-1);

  yminSlider->setMaximum(imsize[1]-1);
  ymaxSlider->setMaximum(imsize[1]-1);
  ymaxSlider->setValue(imsize[1]-1);

  if (slicer->GetDimension() >2) {
    zminSlider->setMaximum(imsize[2]-1);
    zmaxSlider->setMaximum(imsize[2]-1);
    zmaxSlider->setValue(imsize[2]-1);
  }

  if (slicer->GetDimension() >3) {
    tminSlider->setMaximum(imsize[3]-1);
    tmaxSlider->setMaximum(imsize[3]-1);
    tmaxSlider->setValue(imsize[3]-1);
  }

  spin_xmin->setMaximum(imsize[0]-1);
  spin_xmax->setMaximum(imsize[0]-1);
  spin_xmax->setValue(imsize[0]-1);

  spin_ymin->setMaximum(imsize[1]-1);
  spin_ymax->setMaximum(imsize[1]-1);
  spin_ymax->setValue(imsize[1]-1);

  if (slicer->GetDimension() >2) {
    spin_zmin->setMaximum(imsize[2]-1);
    spin_zmax->setMaximum(imsize[2]-1);
    spin_zmax->setValue(imsize[2]-1);
  }

  if (slicer->GetDimension() >3) {
    spin_tmin->setMaximum(imsize[3]-1);
    spin_tmax->setMaximum(imsize[3]-1);
    spin_tmax->setValue(imsize[3]-1);
  }

  // Connect
  connect(xminSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderXMinValueChanged(int)));
  connect(xmaxSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderXMaxValueChanged(int)));
  connect(yminSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderYMinValueChanged(int)));
  connect(ymaxSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderYMaxValueChanged(int)));
  connect(zminSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderZMinValueChanged(int)));
  connect(zmaxSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderZMaxValueChanged(int)));
  connect(tminSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderTMinValueChanged(int)));
  connect(tmaxSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderTMaxValueChanged(int)));

  //  connect(mCurrentSlicerManager,SIGNAL(UpdateSlice(int,int)),this,SLOT(UpdateExtent()));
  //connect(mCurrentSlicerManager,SIGNAL(UpdateTSlice(int,int)),this,SLOT(UpdateExtent()));

  //  connect(mIntensitySlider, SIGNAL(valueChanged(double)), this, SLOT(autoCropValueChanged(double)));
  UpdateExtent();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::autoCropValueChanged(double v)
{
  //TODO
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::apply()
{
  if (!mCurrentSlicerManager) close();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  // Typedef 
  typedef args_info_clitkCropImage ArgsInfoType;
  typedef clitk::CropImageGenericFilter<ArgsInfoType> CropFilterType;
  
  // Get options
  ArgsInfoType mArgsInfo;
  cmdline_parser_clitkCropImage_init(&mArgsInfo); // Initialisation to default
  int n = mCurrentSlicerManager->GetDimension()*2;  // 2D and 3D only
  mArgsInfo.boundingBox_given = n;
  mArgsInfo.boundingBox_arg = new int[n];
  mArgsInfo.boundingBox_arg[0] = xminSlider->value();
  mArgsInfo.boundingBox_arg[1] = xmaxSlider->value();
  mArgsInfo.boundingBox_arg[2] = yminSlider->value();
  mArgsInfo.boundingBox_arg[3] = ymaxSlider->value();
  if (n>3) { // 3D
    mArgsInfo.boundingBox_arg[4] = zminSlider->value();
    mArgsInfo.boundingBox_arg[5] = zmaxSlider->value();
  }

  if (n>6) { // 4D
    // mArgsInfo.boundingBox_arg[6] = tminSlider->value();
    // mArgsInfo.boundingBox_arg[7] = tmaxSlider->value();
    mArgsInfo.boundingBox_arg[6] = 0;
    mArgsInfo.boundingBox_arg[7] = mCurrentImage->GetSize()[3]-1;
  }

  // We MUST reset initial extend to input image before using the
  // filter to retrieve the correct image size
  for(int i=0; i<mExtentSize; i++) {
    mReducedExtent[i] = mInitialExtent[i];
    // DD(mArgsInfo.boundingBox_arg[i]);
  }
  UpdateExtent();

  // Main filter
  CropFilterType::Pointer filter = CropFilterType::New();
  filter->SetInputVVImage(mCurrentImage);
  filter->SetArgsInfo(mArgsInfo);

  // Go ! (not threaded) 
  try{
    filter->Update();
  }
  catch(clitk::ExceptionObject & e) {
    // DD(e.what());
    QApplication::restoreOverrideCursor();
    close();
  }

  // Retrieve result and display it
  vvImage::Pointer output = filter->GetOutputVVImage();
  std::ostringstream osstream;
  osstream << "Croped_" << mCurrentSlicerManager->GetSlicer(0)->GetFileName() << ".mhd";
  AddImage(output,osstream.str());

  // End
  QApplication::restoreOverrideCursor();
  close();

  /** 
      // OLD approach with VTK

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  vvImage::Pointer mResult=vvImage::New();
  vvImage::Pointer image= mCurrentSlicerManager->GetImage();
  for (std::vector<vtkImageData*>::const_iterator i=image->GetVTKImages().begin();
       i!=image->GetVTKImages().end(); i++) {
    vtkSmartPointer<vtkImageClip> filter=vtkSmartPointer<vtkImageClip>::New();
    ///Vtk is very weird, you need to "translate the extent" to get the correct origin
    //http://markmail.org/message/vndc2tr6kcabiakp#query:vtkImageClip%20origin+page:1+mid:6na7y57floutklvz+state:results
    vtkSmartPointer<vtkImageTranslateExtent> translate=vtkSmartPointer<vtkImageTranslateExtent>::New();
    filter->SetInput(*i);
    filter->SetOutputWholeExtent(xminSlider->value(),xmaxSlider->value(),
                                 yminSlider->value(),ymaxSlider->value(),
                                 zminSlider->value(),zmaxSlider->value());
    translate->SetTranslation(-xminSlider->value(),-yminSlider->value(),-zminSlider->value());
    translate->SetInput(filter->GetOutput());
    filter->ClipDataOn(); //Really create a cropped copy of the image
    translate->Update();
    vtkImageData* output=vtkImageData::New();
    output->ShallowCopy(translate->GetOutput());
    mResult->AddImage(output);
  }
  QApplication::restoreOverrideCursor();
  std::ostringstream osstream;
  osstream << "Crop_" << mCurrentSlicerManager->GetSlicer(0)->GetFileName() << ".mhd";
  AddImage(mResult, osstream.str());
  close();

  **/
}
//------------------------------------------------------------------------------

