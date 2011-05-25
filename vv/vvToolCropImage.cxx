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
#include <QSignalMapper>

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

//same order of int* returned by VtkImageData::WholeExtent
enum sliderId {xmin, xmax, ymin, ymax, zmin, zmax, tmin, tmax};
//------------------------------------------------------------------------------
vvToolCropImage::vvToolCropImage(vvMainWindowBase * parent, Qt::WindowFlags f):
  vvToolWidgetBase(parent, f),
  vvToolBase<vvToolCropImage>(parent),
  Ui::vvToolCropImage(),mSliders(8)
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
  
  mSliders[xmin]=xminSlider;
  mSliders[xmax]=xmaxSlider;
  mSliders[ymin]=yminSlider;
  mSliders[ymax]=ymaxSlider;
  mSliders[zmin]=zminSlider;
  mSliders[zmax]=zmaxSlider;
  mSliders[tmin]=tminSlider;
  mSliders[tmax]=tmaxSlider;
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
  if(mCurrentSlicerManager){
    // Reset extends
    for(int i=0; i<mExtentSize; i++) mReducedExtent[i] = mInitialExtent[i];
      UpdateExtent();
  }
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
void vvToolCropImage::sliderValueChanged(int dim)
{
  int dimMin = dim;
  if(dim%2==0){//case we are minimum
    mSliders[dim+1]->setMinimum(mSliders[dim]->value());
  }else {
    mSliders[--dimMin]->setMaximum(mSliders[dim]->value());
  }
  mReducedExtent[dim] = mSliders[dim]->value() + mInitialExtent[dimMin];
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
  for(int dim=0; dim<slicer->GetDimension() && dim<3; ++dim){
    mSliders[dim*2]->setMaximum(imsize[dim]-1);
    mSliders[dim*2+1]->setMaximum(imsize[dim]-1);
    mSliders[dim*2+1]->setValue(imsize[dim]-1);
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

  QSignalMapper* signalMapper = new QSignalMapper(this);
  connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(sliderValueChanged(int)));
  for(unsigned int i=0; i<mSliders.size(); ++i){
    signalMapper->setMapping(mSliders[i], i);
    connect(mSliders[i], SIGNAL(valueChanged(int)), signalMapper, SLOT(map()));
  }
  
  
  

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
  typedef clitk::CropImageGenericFilter CropFilterType;
  
  // Get options
  ArgsInfoType mArgsInfo;
  cmdline_parser_clitkCropImage_init(&mArgsInfo); // Initialisation to default
  int n = mCurrentSlicerManager->GetDimension()*2;  // 2D and 3D only
  mArgsInfo.boundingBox_given = n;
  mArgsInfo.boundingBox_arg = new int[n];
  
  for(int dim=0; dim<mCurrentSlicerManager->GetDimension() && dim<3; ++dim){
    mArgsInfo.boundingBox_arg[dim*2] = mSliders[dim*2]->value();
    mArgsInfo.boundingBox_arg[dim*2+1] = mSliders[dim*2+1]->value();
  }
  if (n>6) { // 4D
    mArgsInfo.boundingBox_arg[6] = 0;
    mArgsInfo.boundingBox_arg[7] = mCurrentImage->GetSize()[3]-1;
  }
  // We MUST reset initial extend to input image before using the
  // filter to retrieve the correct image size
  for(int i=0; i<mExtentSize; i++) {
    mReducedExtent[i] = mInitialExtent[i];
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
    DD(e.what());
    QApplication::restoreOverrideCursor();
    close();
  }
  std::ostringstream croppedImageName;
  croppedImageName << "Cropped_" << mCurrentSlicerManager->GetSlicer(0)->GetFileName() << ".mhd";
  // Retrieve result and display it
  vvImage::Pointer output = filter->GetOutputVVImage();
  AddImage(output,croppedImageName.str());
  // End
  QApplication::restoreOverrideCursor();
  close();
}
//------------------------------------------------------------------------------

