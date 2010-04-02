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

#include "vvImageContour.h"
#include "vvImage.h"
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkMarchingSquares.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//------------------------------------------------------------------------------
vvImageContour::vvImageContour() {
  mTSlice = -1;
  mSlice = 0;
  mHiddenImageIsUsed = false;
  mDisplayModeIsPreserveMemory = true;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvImageContour::~vvImageContour() {
  for (unsigned int i = 0; i < mSlicer->GetImage()->GetVTKImages().size(); i++) {
    mSlicer->GetRenderer()->RemoveActor(mSquaresActorList[i]);
  }
  mSquaresActorList.clear();
  mSquaresList.clear();
  mClipperList.clear();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::setSlicer(vvSlicer * slicer) {
  mSlicer = slicer;  

  for (unsigned int numImage = 0; numImage < mSlicer->GetImage()->GetVTKImages().size(); numImage++) {
    vtkImageClip * mClipper = vtkImageClip::New();
    vtkMarchingSquares * mSquares = vtkMarchingSquares::New();
    //    vtkPolyDataMapper * mSquaresMapper = vtkPolyDataMapper::New();
    vtkActor * mSquaresActor = vtkActor::New();

    createNewActor(&mSquaresActor, &mSquares, &mClipper);
    /*
    mClipper->SetInput(mSlicer->GetImage()->GetVTKImages()[numImage]);
    mSquares->SetInput(mClipper->GetOutput());
    mSquaresMapper->SetInput(mSquares->GetOutput());
    mSquaresMapper->ScalarVisibilityOff();
    mSquaresActor->SetMapper(mSquaresMapper);
    mSquaresActor->GetProperty()->SetColor(1.0,0,0);
    mSquaresActor->SetPickable(0);
    mSquaresActor->VisibilityOff();
    mSlicer->GetRenderer()->AddActor(mSquaresActor);
    */
    mSquaresActorList.push_back(mSquaresActor);
    mSquaresList.push_back(mSquares);
    mClipperList.push_back(mClipper);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::setImage(vvImage::Pointer image) {
  DD("vvImageContour::setImage");
  for (unsigned int numImage = 0; numImage < image->GetVTKImages().size(); numImage++) {
    mClipperList[numImage]->SetInput(image->GetVTKImages()[numImage]);
  }
  mHiddenImageIsUsed = true;
  mHiddenImage = image;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::setPreserveModeEnabled(bool b) {
  DD("setPreserveModeEnabled");
  DD(b);
  DD(mDisplayModeIsPreserveMemory);
  if (mDisplayModeIsPreserveMemory == b) return;
  mDisplayModeIsPreserveMemory = b;
  if (!b) {
    initializeCacheMode();
  }
  else {
    for(unsigned int d=0; d<mListOfCachedContourActors.size(); d++)
      mListOfCachedContourActors[d].clear();
    mListOfCachedContourActors.clear();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::setColor(double r, double g, double b) {
  for(unsigned int i=0; i<mSquaresActorList.size(); i++) {
    mSquaresActorList[i]->GetProperty()->SetColor(r,g,b);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::hideActors() {
  if (!mSlicer) return;
  mSlice = mSlicer->GetSlice();
  for(unsigned int i=0; i<mSquaresActorList.size(); i++) {
    mSquaresActorList[i]->VisibilityOff();
  }
}
//------------------------------------------------------------------------------

  
//------------------------------------------------------------------------------
void vvImageContour::showActors() {
  if (!mSlicer) return;
  mSlice = mSlicer->GetSlice();
  mTSlice = mSlicer->GetTSlice();
  //  for(unsigned int i=0; i<mSquaresActorList.size(); i++) {
  mSquaresActorList[mTSlice]->VisibilityOn();
  update(mValue);
  //}
}
//------------------------------------------------------------------------------

  
//------------------------------------------------------------------------------
void vvImageContour::update(double value) {
  if (!mSlicer) return;
  // Get current threshold value
  mValue= value;
  // Get current slice
  mSlice = mSlicer->GetSlice();

  if (mDisplayModeIsPreserveMemory) {
    updateWithPreserveMemoryMode();
  }
  else {
    updateWithFastCacheMode();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::updateWithPreserveMemoryMode() {

  // Only change actor visibility if tslice change
  if (mTSlice != mSlicer->GetTSlice()) {
    if (mTSlice != -1) 
      mSquaresActorList[mTSlice]->VisibilityOff();
    mTSlice = mSlicer->GetTSlice();
    mSquaresActorList[mTSlice]->VisibilityOn();
  }
  
  vtkMarchingSquares * mSquares = mSquaresList[mTSlice];
  vtkImageClip * mClipper = mClipperList[mTSlice];
  vtkActor * mSquaresActor = mSquaresActorList[mTSlice];
  int orientation = computeCurrentOrientation();
  // DD(orientation);

  updateActor(mSquaresActor, mSquares, mClipper, mValue, orientation, mSlice);

  return;
  
  // Do it
  mSquares->SetValue(0, mValue );

  int* extent = mSlicer->GetImageActor()->GetDisplayExtent();
  // DD(extent[0]);
  // DD(extent[1]);
  // DD(extent[2]);
  // DD(extent[3]);
  // DD(extent[4]);
  // DD(extent[5]);

  // int* extent2 = mClipper->GetInput()->GetDisplayExtent();
  // DD(extent2[0]);
  // DD(extent2[1]);
  int* extent2 = new int[6];
  if (mHiddenImageIsUsed) {
    int * extent3;
    extent3 = mHiddenImage->GetFirstVTKImageData()->GetExtent();
    for(int i=0; i<6; i++) extent2[i] = extent3[i];
    // DD(extent2[0]);
    // DD(extent2[1]);
    // DD(extent2[2]);
    // DD(extent2[3]);
    // DD(extent2[4]);
    // DD(extent2[5]);
    for(int i=0; i<6; i+=2) {
      if (extent[i] != extent[i+1]) { 
        // extent[i] = extent2[i]; 
        // extent[i+1] = extent2[i+1]; 
      }
      else {
        // DD(extent[i]);
        // DD(mSlicer->GetImage()->GetSpacing()[i/2]);
        // DD(mHiddenImage->GetFirstVTKImageData()->GetSpacing()[i/2]);

        double s = (double)extent[i]*(double)mSlicer->GetImage()->GetSpacing()[i/2]; // in mm
        // DD(s);
        s = s+mSlicer->GetImage()->GetOrigin()[i/2]; // from origin
        // DD(s);
        s = s-mHiddenImage->GetFirstVTKImageData()->GetOrigin()[i/2]; // from corner second image
        s = s/mHiddenImage->GetFirstVTKImageData()->GetSpacing()[i/2]; // in voxel
        // DD(s);

        if (s == floor(s)) { 
          extent2[i] = extent2[i+1] = (int)floor(s);
        }
        else {
          extent2[i] = (int)floor(s);
          extent2[i+1] = extent2[i];
        }
        // DD(extent2[i]);
      }
    }
  }
  else extent2 = extent;
  // DD(extent2[0]);
  // DD(extent2[1]);
  // DD(extent2[2]);
  // DD(extent2[3]);
  // DD(extent2[4]);
  // DD(extent2[5]);
  

  mClipper->SetOutputWholeExtent(extent2[0],extent2[1],extent2[2],
				 extent2[3],extent2[4],extent2[5]);
  int i;
  for (i = 0; i < 6;i = i+2) {
    if (extent[i] == extent[i+1]) {
      break;
    }
  }
  
  switch (i)
    {
    case 0:
      if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[0] > mSlice)
        {
	  mSquaresActor->SetPosition(1,0,0);
        }
      else
        {
	  mSquaresActor->SetPosition(-1,0,0);
        }
      break;
    case 2:
      if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[1] > mSlice)
        {
	  mSquaresActor->SetPosition(0,1,0);
        }
      else
        {
	  mSquaresActor->SetPosition(0,-1,0);
        }
      break;
    case 4:
      if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[2] > mSlice)
        {
	  mSquaresActor->SetPosition(0,0,1);
        }
      else
        {
	  mSquaresActor->SetPosition(0,0,-1);
        }
      break;
    }
  mSquares->Update();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::initializeCacheMode() {
  DD("vvImageContour::initializeCacheMode");
    
  mPreviousSlice = mPreviousOrientation = 0;
  int dim;
  if (mHiddenImageIsUsed) dim = mHiddenImage->GetNumberOfDimensions();
  else dim = mSlicer->GetImage()->GetNumberOfDimensions();
  DD(dim);

  mListOfCachedContourActors.resize(dim);
  for(int d=0; d<dim; d++) {
    DD(d);
    int size;
    if (mHiddenImageIsUsed) size = mHiddenImage->GetSize()[d];
    else size = mSlicer->GetImage()->GetSize()[d];
    DD(size);
    mListOfCachedContourActors[d].resize(size);
    for(int j=0; j<size; j++) {
      mListOfCachedContourActors[d][j] = NULL;
      DD(mListOfCachedContourActors.size());
      DD(mListOfCachedContourActors[d].size());
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
int vvImageContour::computeCurrentOrientation() {
  // Get extent of image in the slicer
  int* extent = mSlicer->GetImageActor()->GetDisplayExtent();
  
  // Compute orientation
  int orientation;
  for (orientation = 0; orientation < 6;orientation = orientation+2) {
    if (extent[orientation] == extent[orientation+1]) {
      break;
    }
  }
  orientation = orientation/2;
  // DD(orientation);
  return orientation;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::updateWithFastCacheMode() {
  DD("vvImageContour::updateWithFastCacheMode");

  // Compute orientation
  int orientation = computeCurrentOrientation();

  // Turn off previous actor
  DD(mPreviousOrientation);
  DD(mPreviousSlice);
  if (mListOfCachedContourActors[mPreviousOrientation][mPreviousSlice] != NULL)
    mListOfCachedContourActors[mPreviousOrientation][mPreviousSlice]->VisibilityOff();
  mPreviousSlice = mSlice;
  mPreviousOrientation = orientation;

  // Display actor if it exist
  vtkActor * actor = mListOfCachedContourActors[orientation][mSlice];
  if (actor != NULL) {
    DD("Actor exist");
    mListOfCachedContourActors[orientation][mSlice]->VisibilityOn();
  }
  else {
    vtkImageClip * mClipper;
    vtkMarchingSquares * mSquares;
    vtkActor * mSquaresActor;
    createNewActor(&mSquaresActor, &mSquares, &mClipper);
    updateActor(mSquaresActor, mSquares, mClipper, mValue, orientation, mSlice);
    mListOfCachedContourActors[orientation][mSlice] = mSquaresActor;
    mSquaresActor->VisibilityOn();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::createNewActor(vtkActor ** actor, 
				    vtkMarchingSquares ** squares, 
				    vtkImageClip ** clipper) {
  // DD("vvImageContour::CreateNewActor");
  vtkActor * mSquaresActor = (*actor = vtkActor::New());
  vtkImageClip * mClipper = (*clipper = vtkImageClip::New());
  vtkMarchingSquares * mSquares = (*squares = vtkMarchingSquares::New());
  vtkPolyDataMapper * mSquaresMapper = vtkPolyDataMapper::New();
  
  if (mHiddenImageIsUsed) 
    mClipper->SetInput(mHiddenImage->GetVTKImages()[0]);
  else 
    mClipper->SetInput(mSlicer->GetImage()->GetVTKImages()[0]);
  mSquares->SetInput(mClipper->GetOutput());
  mSquaresMapper->SetInput(mSquares->GetOutput());
  mSquaresMapper->ScalarVisibilityOff();
  mSquaresActor->SetMapper(mSquaresMapper);
  mSquaresActor->GetProperty()->SetColor(1.0,0,0);
  mSquaresActor->SetPickable(0);
  mSquaresActor->VisibilityOff();
  mSlicer->GetRenderer()->AddActor(mSquaresActor);  
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::updateActor(vtkActor * actor, 
				 vtkMarchingSquares * squares, 
				 vtkImageClip * clipper, 
				 int threshold, int orientation, int slice) {
  // DD("Update Actor according to extend/threshold");
  
  int* extent = mSlicer->GetImageActor()->GetDisplayExtent();
  clipper->SetOutputWholeExtent(extent[0],extent[1],extent[2],
				 extent[3],extent[4],extent[5]);
  squares->SetValue(0, threshold);

  switch (orientation)  {
  case 0: if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[0] > slice) {
      actor->SetPosition(1,0,0);
    }
    else {
      actor->SetPosition(-1,0,0);
    }
    break;
  case 1: if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[1] > slice) {
      actor->SetPosition(0,1,0);
    }
    else {
      actor->SetPosition(0,-1,0);
    }
    break;
  case 2: if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[2] > slice) {
      actor->SetPosition(0,0,1);
    }
    else {
      actor->SetPosition(0,0,-1);
    }
    break;
  }
  squares->Update();
}
//------------------------------------------------------------------------------


