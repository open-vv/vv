/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolCropImage.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/24 20:35:13 $
  Version:   $Revision: 1.6 $
  Author :   David Sarrut (david.sarrut@creatis.insa-lyon.fr)

  Copyright (C) 2008
  Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
  CREATIS-LRMN http://www.creatis.insa-lyon.fr

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, version 3 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  =========================================================================*/

#include "vvToolCropImage.h"
#include <QComboBox>
#include <QCursor>
#include <QApplication>
#include <vtkImageClip.h>
#include <vtkImageTranslateExtent.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include "vvSlicer.h"

//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolCropImage);
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolCropImage::vvToolCropImage(vvMainWindowBase * parent, Qt::WindowFlags f):
  vvToolWidgetBase(parent, f), 
  vvToolBase<vvToolCropImage>(parent), 
  Ui::vvToolCropImage() {

  // GUI Initialization
  Ui_vvToolCropImage::setupUi(mToolWidget);

  // Set how many inputs are needed for this tool
  AddInputSelector("Select one image");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolCropImage::~vvToolCropImage() {

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolCropImage::close() { 
  for(int i=0; i<6; i++) mReducedExtent[i] = mInitialExtent[i];
  UpdateExtent();
  return vvToolWidgetBase::close(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::reject() { 
  for(int i=0; i<6; i++) mReducedExtent[i] = mInitialExtent[i];
  UpdateExtent();
  return vvToolWidgetBase::reject(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::sliderXMinValueChanged(int s) {
  xmaxSlider->setMinimum(xminSlider->value());
  mReducedExtent[0] = xminSlider->value();
  UpdateExtent();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::sliderXMaxValueChanged(int s) {
  xminSlider->setMaximum(xmaxSlider->value());
  mReducedExtent[1] = xmaxSlider->value();
  UpdateExtent();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::sliderYMinValueChanged(int s) {
  ymaxSlider->setMinimum(yminSlider->value());
  mReducedExtent[2] = yminSlider->value();
  UpdateExtent();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::sliderYMaxValueChanged(int s) {
  yminSlider->setMaximum(ymaxSlider->value());
  mReducedExtent[3] = ymaxSlider->value();
  UpdateExtent();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::sliderZMinValueChanged(int s) {
  zmaxSlider->setMinimum(zminSlider->value());
  mReducedExtent[4] = zminSlider->value();
  UpdateExtent();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::sliderZMaxValueChanged(int s) {
  zminSlider->setMaximum(zmaxSlider->value());
  mReducedExtent[5] = zmaxSlider->value();
  UpdateExtent();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void vvToolCropImage::sliderTMinValueChanged(int s) {
//   tmaxSlider->setMinimum(tminSlider->value());
//   mReducedExtent[6] = tminSlider->value();
//   UpdateExtent();
// }
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void vvToolCropImage::sliderTMaxValueChanged(int s) {
//   tminSlider->setMaximum(tmaxSlider->value());
//   mReducedExtent[7] = tmaxSlider->value();
//   UpdateExtent();
// }
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::UpdateExtent() {
  for(int i=0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
    mCurrentSlicerManager->GetSlicer(i)->SetReducedExtent(mReducedExtent);
    mCurrentSlicerManager->GetSlicer(i)->ForceUpdateDisplayExtent();    
    mCurrentSlicerManager->GetSlicer(i)->Render();    
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::InputIsSelected(vvSlicerManager * slicer) {

  // Change interface according to number of dimension
  mExtentSize = 2*slicer->GetDimension();
  // if (slicer->GetDimension()<4) {
  //   mTLabel1->setHidden(true);
  //   mTLabel2->setHidden(true);
  //   tminSlider->setHidden(true);
  //   tmaxSlider->setHidden(true);
  //   spin_tmin->setHidden(true);
  //   spin_tmax->setHidden(true);
  // }
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
  for(int i=0; i<6; i++) mInitialExtent[i] = mReducedExtent[i];
  for(int i=0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
    //DD(i);
    //DD(mReducedExtent[i]);
    //DD(mInitialExtent[i]);
    mCurrentSlicerManager->GetSlicer(i)->EnableReducedExtent(true);
  }
  
  // Not now ....
  //  mIntensitySlider->SetImage(mCurrentImage);
  //mIntensitySlider->setEnabled(false);

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

  // if (slicer->GetDimension() >3) {
  //   tminSlider->setMaximum(imsize[3]-1);
  //   tmaxSlider->setMaximum(imsize[3]-1);
  //   tmaxSlider->setValue(imsize[3]-1);
  // }

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
  
  // if (slicer->GetDimension() >3) {
  //   spin_tmin->setMaximum(imsize[3]-1);
  //   spin_tmax->setMaximum(imsize[3]-1);
  //   spin_tmax->setValue(imsize[3]-1);
  // }  
  
  // Connect
  connect(xminSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderXMinValueChanged(int)));
  connect(xmaxSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderXMaxValueChanged(int)));
  connect(yminSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderYMinValueChanged(int)));
  connect(ymaxSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderYMaxValueChanged(int)));
  connect(zminSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderZMinValueChanged(int)));
  connect(zmaxSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderZMaxValueChanged(int)));
  // connect(tminSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderTMinValueChanged(int)));
  // connect(tmaxSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderTMaxValueChanged(int)));
  
  //  connect(mIntensitySlider, SIGNAL(valueChanged(double)), this, SLOT(autoCropValueChanged(double)));
  UpdateExtent();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::autoCropValueChanged(double v) {
  // DD(v);
  //  vvImageToITKImageVector -> TODO a generic method
  // then sliceriterator on each dimension from in to max
  // ==> make a clitkGenericFilter even for the regular apply ...
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::apply() {

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  vvImage::Pointer mResult=vvImage::New();
  vvImage::Pointer image= mCurrentSlicerManager->GetImage();
  for (std::vector<vtkImageData*>::const_iterator i=image->GetVTKImages().begin();
       i!=image->GetVTKImages().end();i++)
    {
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
}
//------------------------------------------------------------------------------

