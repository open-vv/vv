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
#include <QComboBox>
#include <QCursor>
#include <QApplication>
#include <vtkImageClip.h>
#include <vtkImageTranslateExtent.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include "vvCropDialog.h"
#include "vvSlicerManager.h"
#include "clitkCommon.h"

vvCropDialog::vvCropDialog(std::vector<vvSlicerManager*> sms,int current) :
  mSlicerManagers(sms)
{
  setupUi(this);
  for (unsigned int i=0; i<mSlicerManagers.size(); i++)
    inputSequenceBox->addItem(vtksys::SystemTools::GetFilenameName(mSlicerManagers[i]->GetFileName()).c_str());
  connect(inputSequenceBox,SIGNAL(currentIndexChanged(int)),this,SLOT(ImageChanged(int)));
  inputSequenceBox->setCurrentIndex(current);
  ImageChanged(current);
  connect(this,SIGNAL(accepted()),this,SLOT(ComputeCroppedImage()));
}

void vvCropDialog::ImageChanged(int newindex)
{
  std::vector<int> imsize=mSlicerManagers[newindex]->GetImage()->GetSize();
  xminSlider->setMaximum(imsize[0]-1);
  xmaxSlider->setMaximum(imsize[0]-1);
  xmaxSlider->setValue(imsize[0]-1);
  yminSlider->setMaximum(imsize[1]-1);
  ymaxSlider->setMaximum(imsize[1]-1);
  ymaxSlider->setValue(imsize[1]-1);
  zminSlider->setMaximum(imsize[2]-1);
  zmaxSlider->setMaximum(imsize[2]-1);
  zmaxSlider->setValue(imsize[2]-1);
  spin_xmin->setMaximum(imsize[0]-1);
  spin_xmax->setMaximum(imsize[0]-1);
  spin_xmax->setValue(imsize[0]-1);
  spin_ymin->setMaximum(imsize[1]-1);
  spin_ymax->setMaximum(imsize[1]-1);
  spin_ymax->setValue(imsize[1]-1);
  spin_zmin->setMaximum(imsize[2]-1);
  spin_zmax->setMaximum(imsize[2]-1);
  spin_zmax->setValue(imsize[2]-1);
}

void vvCropDialog::ComputeCroppedImage()
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  mResult=vvImage::New();
  vvSlicerManager * current=mSlicerManagers[inputSequenceBox->currentIndex()];
  vvImage::Pointer image=current->GetImage();
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
}
