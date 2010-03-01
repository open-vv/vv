/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolCropImage.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/01 07:37:25 $
  Version:   $Revision: 1.2 $
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
  mReducedExtent = new int[6]; // TO CHANGE !! nb dim

  // Connect
  connect(xminSlider, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolCropImage::~vvToolCropImage() {

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::sliderMoved(int s) {
  DD(s);
  mReducedExtent[0] = xminSlider->value();
  for(int i=0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
    mCurrentSlicerManager->GetSlicer(i)->SetReducedExtent(mReducedExtent);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::InputIsSelected(vvSlicerManager * slicer) {
  DD("InputIsSelected"); 

  mReducedExtent = mCurrentSlicerManager->GetImage()->GetFirstVTKImageData()->GetWholeExtent();
  DD(mReducedExtent[0]);
  DD(mReducedExtent[1]);

  
  // TODO : set min/max
    std::vector<int> imsize = mCurrentSlicerManager->GetImage()->GetSize();
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
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCropImage::apply() {
  DD("apply");

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    vvImage::Pointer mResult=vvImage::New();
    //vvSlicerManager * current=mSlicerManagers[inputSequenceBox->currentIndex()];
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

  AddImage(mResult,"crop.mhd"); 

  close();
}
//------------------------------------------------------------------------------

