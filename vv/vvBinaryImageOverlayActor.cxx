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

#include "vvBinaryImageOverlayActor.h"
#include "vvImage.h"
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkMarchingSquares.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkImageMapToRGBA.h>
#include <vtkLookupTable.h>

//------------------------------------------------------------------------------
vvBinaryImageOverlayActor::vvBinaryImageOverlayActor() {
  mTSlice = -1;
  mSlice = 0;
  mColor.resize(3);
  mAlpha = 0.6;
  mImage = 0;
  mSlicer = 0;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvBinaryImageOverlayActor::~vvBinaryImageOverlayActor() {
  for (unsigned int i = 0; i < mSlicer->GetImage()->GetVTKImages().size(); i++) {
    mSlicer->GetRenderer()->RemoveActor(mImageActorList[i]);
  }
  mImageActorList.clear();
  mMapperList.clear();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::setColor(double r, double g, double b) {
  mColor[0] = r;
  mColor[1] = g;
  mColor[2] = b;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::setSlicer(vvSlicer * slicer) {
  mSlicer = slicer;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::initialize() {
  if (!mSlicer) {
    std::cerr << "ERROR. Please use setSlicer before setSlicer in vvBinaryImageOverlayActor." << std::endl;
    exit(0);
  }
  
  if (!mImage) {
    std::cerr << "ERROR. Please use setImage before setSlicer in vvBinaryImageOverlayActor." << std::endl;
    exit(0);
  }
  // Create an actor for each time slice
  for (unsigned int numImage = 0; numImage < mSlicer->GetImage()->GetVTKImages().size(); numImage++) {
    DD(numImage);

    // how many intensity ? 
    

    vtkImageMapToRGBA * mOverlayMapper = vtkImageMapToRGBA::New();
    mOverlayMapper->SetInput(mImage->GetVTKImages()[0]); // DS TODO : to change if it is 4D !!!
    vtkLookupTable * lut = vtkLookupTable::New();
    DD(lut->IsOpaque ());
    lut->SetRange(0,1);
    lut->SetNumberOfTableValues(2);
    lut->SetTableValue(mBackgroundValue, 0, 0, 0, 0.0);   // BG
    lut->SetTableValue(1, mColor[0], mColor[1], mColor[2], mAlpha); // FG
    DD(mColor[0]);
    mOverlayMapper->SetLookupTable(lut);
    
    vtkImageActor * mOverlayActor = vtkImageActor::New();
    mOverlayActor->SetInput(mOverlayMapper->GetOutput());
    mOverlayActor->SetPickable(0);
    mOverlayActor->SetVisibility(true);
    //mOverlayActor->SetOpacity(1.0);

    mMapperList.push_back(mOverlayMapper);
    mImageActorList.push_back(mOverlayActor);
    mSlicer->GetRenderer()->AddActor(mOverlayActor);  
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::setImage(vvImage::Pointer image, double bg) {
  mImage = image;
  mBackgroundValue = bg;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::hideActors() {
  if (!mSlicer) return;
  mSlice = mSlicer->GetSlice();
  for(unsigned int i=0; i<mImageActorList.size(); i++) {
    mImageActorList[i]->VisibilityOff();
  }
}
//------------------------------------------------------------------------------

  
//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::showActors() {
  if (!mSlicer) return;
  mSlice = mSlicer->GetSlice();
  mTSlice = mSlicer->GetTSlice();
  //  for(unsigned int i=0; i<mSquaresActorList.size(); i++) {
  mImageActorList[mTSlice]->VisibilityOn();
  update(0, mSlice);
  //}
}
//------------------------------------------------------------------------------

  
//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::update(int slicer, int slice) {
  if (!mSlicer) return;

  if (mPreviousSlice == mSlicer->GetSlice()) {
    if (mPreviousTSlice == mSlicer->GetTSlice()) {
      //DD("=========== NOTHING");
      return; // Nothing to do
    }
  }

  // Get current slice
  mSlice = mSlicer->GetSlice();
  mTSlice = mSlicer->GetTSlice();

  // Update extent
  int * imageExtent = mSlicer->GetExtent();
  int orientation = mSlicer->GetOrientation();
  int maskExtent[6];
  ComputeExtent(orientation, mSlice, imageExtent, maskExtent);
  //ComputeExtent(imageExtent, maskExtent, mSlicer->GetImage()->GetFirstVTKImageData(), mImage->GetFirstVTKImageData());
  ComputeExtent(maskExtent, maskExtent, mSlicer->GetImage()->GetFirstVTKImageData(), mImage->GetFirstVTKImageData());
  // std::cout << "maskExtent = " << maskExtent[0] << " " << maskExtent[1] << " " << maskExtent[2] << " " 
  //  	    << maskExtent[3] << " " << maskExtent[4] << " " << maskExtent[5] << std::endl;
  mSlicer->ClipDisplayedExtent(maskExtent, mMapperList[mTSlice]->GetInput()->GetWholeExtent());
  // std::cout << "maskExtent = " << maskExtent[0] << " " << maskExtent[1] << " " << maskExtent[2] << " " 
  // 	    << maskExtent[3] << " " << maskExtent[4] << " " << maskExtent[5] << std::endl;
  SetDisplayExtentAndCameraPosition(orientation, mSlice, maskExtent, mImageActorList[mTSlice], 0.0);
  
  // set previous slice
  mPreviousTSlice = mSlicer->GetTSlice();
  mPreviousSlice  = mSlicer->GetSlice();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::ComputeExtent(int orientation, 
					      int slice, 
					      int * inExtent,
					      int * outExtent) {
  //  std::cout << "InExtent = " << inExtent[0] << " " << inExtent[1] << " " << inExtent[2] << " " 
  //	    << inExtent[3] << " " << inExtent[4] << " " << inExtent[5] << std::endl;
  //  DD(orientation);
  switch (orientation) {
  case vtkImageViewer2::SLICE_ORIENTATION_XY:
    for(int i=0; i<4; i++) outExtent[i] = inExtent[i];
    outExtent[4] = outExtent[5] = slice; 
    break;
  case vtkImageViewer2::SLICE_ORIENTATION_XZ:
    for(int i=0; i<2; i++) outExtent[i] = inExtent[i];
    for(int i=4; i<6; i++) outExtent[i] = inExtent[i];
    outExtent[2] = outExtent[3] = slice; 
    break;
  case vtkImageViewer2::SLICE_ORIENTATION_YZ:
    for(int i=2; i<6; i++) outExtent[i] = inExtent[i];
    outExtent[0] = outExtent[1] = slice; 
    break;
  }
  // std::cout << "OutExtent = " << outExtent[0] << " " << outExtent[1] << " " << outExtent[2] << " " 
  // 	    << outExtent[3] << " " << outExtent[4] << " " << outExtent[5] << std::endl;
}
//------------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvBinaryImageOverlayActor::ComputeExtent(int * inExtent, int * outExtent, vtkImageData * image, vtkImageData * overlay)
{
  outExtent[0] = (( image->GetOrigin()[0] + inExtent[0]*image->GetSpacing()[0] ) - overlay->GetOrigin()[0]) /
    overlay->GetSpacing()[0];
  outExtent[1] = (( image->GetOrigin()[0] + inExtent[1]*image->GetSpacing()[0] ) - overlay->GetOrigin()[0]) /
    overlay->GetSpacing()[0];
  outExtent[2] = (( image->GetOrigin()[1] + inExtent[2]*image->GetSpacing()[1] ) - overlay->GetOrigin()[1]) /
    overlay->GetSpacing()[1];
  outExtent[3] = (( image->GetOrigin()[1] + inExtent[3]*image->GetSpacing()[1] ) - overlay->GetOrigin()[1]) /
    overlay->GetSpacing()[1];
  outExtent[4] = (( image->GetOrigin()[2] + inExtent[4]*image->GetSpacing()[2] ) - overlay->GetOrigin()[2]) /
    overlay->GetSpacing()[2];
  outExtent[5] = (( image->GetOrigin()[2] + inExtent[5]*image->GetSpacing()[2] ) - overlay->GetOrigin()[2]) /
    overlay->GetSpacing()[2];
}
//----------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::SetDisplayExtentAndCameraPosition(int orientation, 
								  int slice, 
								  int * extent, 
								  vtkImageActor * actor, 
								  double position) {
  actor->SetDisplayExtent(extent);
  
  // Set position
  if (orientation == vtkImageViewer2::SLICE_ORIENTATION_XY) {
    if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[2] > slice)
      actor->SetPosition(0,0, position);
    else
      actor->SetPosition(0,0, -position);    
  }
  if (orientation == vtkImageViewer2::SLICE_ORIENTATION_XZ) {
    if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[1] > slice)
      actor->SetPosition(0,position,0);
    else
      actor->SetPosition(0,-position,0);    
  }
  if (orientation == vtkImageViewer2::SLICE_ORIENTATION_YZ) {
    if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[0] > slice)
      actor->SetPosition(position,0, 0);
    else
      actor->SetPosition(-position,0, 0);    
  }
  
}
//------------------------------------------------------------------------------




