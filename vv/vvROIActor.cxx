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

#include "vvROIActor.h"
#include "vvImageContour.h"
#include "vvSlicerManager.h"
#include "vvBinaryImageOverlayActor.h"
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
//#include <vtkRenderWindow.h>
#include <vtkMarchingSquares.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//------------------------------------------------------------------------------
vvROIActor::vvROIActor() {
  mImageContour.clear();
  mOverlayActors.clear();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvROIActor::~vvROIActor() {
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::SetROI(const clitk::DicomRT_ROI * s) {
  mROI = s;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::SetSlicerManager(vvSlicerManager * s) {
  mSlicerManager = s;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::Initialize() {
  if (mROI->GetImage()) {
    mImageContour.clear();
    mOverlayActors.clear();
    for(int i=0;i<mSlicerManager->NumberOfSlicers(); i++) {
      mImageContour.push_back(new vvImageContour);
      mImageContour[i]->setSlicer(mSlicerManager->GetSlicer(i));
      mImageContour[i]->setImage(mROI->GetImage());
      //      mImageContour[i]->setColor(1.0, 0.0, 0.0);
      mImageContour[i]->setColor(mROI->GetDisplayColor()[0], 
				 mROI->GetDisplayColor()[1], 
				 mROI->GetDisplayColor()[2]);
      mImageContour[i]->setPreserveMemoryModeEnabled(false);        
      
      mOverlayActors.push_back(new vvBinaryImageOverlayActor);
      mOverlayActors[i]->setImage(mROI->GetImage(), mROI->GetBackgroundValueLabelImage());
      mOverlayActors[i]->setColor(mROI->GetDisplayColor()[0], 
				  mROI->GetDisplayColor()[1], 
				  mROI->GetDisplayColor()[2]);
      mOverlayActors[i]->setSlicer(mSlicerManager->GetSlicer(i));
      mOverlayActors[i]->initialize();
    }
    
    connect(mSlicerManager,SIGNAL(UpdateSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
    //connect(mSlicerManager,SIGNAL(UpdateTSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
    connect(mSlicerManager, SIGNAL(AVerticalSliderHasChanged(int, int)), SLOT(UpdateSlice(int, int)));
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::Update() {
  for(int i=0; i<mSlicerManager->NumberOfSlicers(); i++) {
    UpdateSlice(i, mSlicerManager->GetSlicer(i)->GetSlice());
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::UpdateSlice(int slicer, int slices) {
   if (!mROI->GetImage())  return;

  if (!mSlicerManager) {
    std::cerr << "Error. No mSlicerManager ?" << std::endl;
    exit(0);
  }

  // CONTOUR HERE 
  // mImageContour[slicer]->update(1.0); 

  // Refresh overlays
  mOverlayActors[slicer]->update(slicer, slices);

  // Do not used the following line : TOO SLOW.
  // mSlicerManager->GetSlicer(slicer)->GetRenderWindow()->Render(); 
}
//------------------------------------------------------------------------------
