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
vvImageContour::vvImageContour()
{
  mTSlice = -1;
  mSlice = 0;
  mHiddenImageIsUsed = false;
  mDisplayModeIsPreserveMemory = true;
  SetPreserveMemoryModeEnabled(true);
  mPreviousOrientation = -1;
  mDepth = 1.0;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvImageContour::~vvImageContour()
{
  for (unsigned int i = 0; i < mSlicer->GetImage()->GetVTKImages().size(); i++) {
    mSlicer->GetRenderer()->RemoveActor(mSquaresActorList[i]);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::SetSlicer(vvSlicer * slicer) {
  mSlicer = slicer;  
  // Create an actor for each time slice
  for (unsigned int numImage = 0; numImage < mSlicer->GetImage()->GetVTKImages().size(); numImage++) {
    CreateNewActor(numImage);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::SetImage(vvImage * image) {
  for (unsigned int numImage = 0; numImage < image->GetVTKImages().size(); numImage++) {
    mClipperList[numImage]->SetInput(image->GetVTKImages()[numImage]);
  }
  mHiddenImageIsUsed = true;
  mHiddenImage = image;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::SetPreserveMemoryModeEnabled(bool b) {
  // FastCache mode work only if threshold is always the same
  if (mDisplayModeIsPreserveMemory == b) return;
  mDisplayModeIsPreserveMemory = b;
  if (!b) {
    clitkExceptionMacro("TODO : not implemented yet");
    HideActors();
    InitializeCacheMode();
  }
  else {
    for(unsigned int d=0; d<mListOfCachedContourActors.size(); d++)
      mListOfCachedContourActors[d].clear();
    mListOfCachedContourActors.clear();
    ShowActors();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::SetColor(double r, double g, double b) {
  for(unsigned int i=0; i<mSquaresActorList.size(); i++) {
    mSquaresActorList[i]->GetProperty()->SetColor(r,g,b);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::SetLineWidth(double w)
{
  for(unsigned int i=0; i<mSquaresActorList.size(); i++) {
    mSquaresActorList[i]->GetProperty()->SetLineWidth(w);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::HideActors() {
  if (!mSlicer) return;
  mSlice = mSlicer->GetSlice();
  for(unsigned int i=0; i<mSquaresActorList.size(); i++) {
    mSquaresActorList[i]->VisibilityOff();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::ShowActors() {
  if (!mSlicer) return;
  mSlice = mSlicer->GetSlice();
  mTSlice = mSlicer->GetTSlice();
  mSquaresActorList[mTSlice]->VisibilityOn();
  Update(mValue);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::Update(double value) {
  if (!mSlicer) return;
  if (mPreviousValue == value) {
    if (mPreviousSlice == mSlicer->GetSlice()) {
      if (mPreviousTSlice == mSlicer->GetTSlice()) {
        if (mPreviousOrientation == ComputeCurrentOrientation()) {
          return; // Nothing to do
        }
      }
    }
  }

  // Get current threshold value
  mValue = value;

  // Get current slice
  mSlice = mSlicer->GetSlice();

  if (mDisplayModeIsPreserveMemory) {
    UpdateWithPreserveMemoryMode();
  }
  else {
    UpdateWithFastCacheMode();
  }

  //mSlicer->Render(); //DS ---> REMOVE ??

  mPreviousTSlice = mSlicer->GetTSlice();
  mPreviousSlice  = mSlicer->GetSlice();
  mPreviousValue  = value;
  mPreviousOrientation = ComputeCurrentOrientation();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::UpdateWithPreserveMemoryMode() {
  // Only change actor visibility if tslice change
  mPreviousTslice = mTSlice;
  mTSlice = mSlicer->GetTSlice();

  vtkMarchingSquares * mSquares = mSquaresList[mTSlice];
  vtkPolyDataMapper* mapper = mSquaresMapperList[mTSlice];
  vtkImageClip * mClipper = mClipperList[mTSlice];
  vtkActor * mSquaresActor = mSquaresActorList[mTSlice];
  int orientation = ComputeCurrentOrientation();

  UpdateActor(mSquaresActor, mapper, mSquares, mClipper, mValue, orientation, mSlice);

  if (mPreviousTslice != mTSlice) {
    if (mPreviousTslice != -1) mSquaresActorList[mPreviousTslice]->VisibilityOff();
  }
  
  mSlicer->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::InitializeCacheMode() {
clitkExceptionMacro("TODO : not implemented yet");
  mPreviousSlice = mPreviousOrientation = 0;
  int dim = mSlicer->GetImage()->GetNumberOfDimensions();

  mListOfCachedContourActors.resize(dim);
  for(int d=0; d<dim; d++) {
    int size = mSlicer->GetImage()->GetSize()[d];
    mListOfCachedContourActors[d].resize(size);
    for(int j=0; j<size; j++) {
      mListOfCachedContourActors[d][j] = NULL;
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
int vvImageContour::ComputeCurrentOrientation() {
  // Get extent of image in the slicer
  int* extent = mSlicer->GetImageActor()->GetDisplayExtent();

  // Compute orientation
  int orientation;
  for (orientation = 0; orientation < 6; orientation = orientation+2) {
    if (extent[orientation] == extent[orientation+1]) {
      break;
    }
  }
  orientation = orientation/2;
  return orientation;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::UpdateWithFastCacheMode() {
clitkExceptionMacro("TODO : not implemented yet");

  // Compute orientation
  int orientation = ComputeCurrentOrientation();

  if ((mPreviousSlice == mSlice) && (mPreviousOrientation == orientation)) return;

  vtkActor * actor = mListOfCachedContourActors[orientation][mSlice];
  if (actor != NULL) {
    mListOfCachedContourActors[orientation][mSlice]->VisibilityOn();
  } else {
    CreateNewActor(0);
    //SR: commented out, this code is never reached anyway
    //UpdateActor(mSquaresActor, mSquares, mClipper, mValue, orientation, mSlice);
    //mListOfCachedContourActors[orientation][mSlice] = mSquaresActor;
    //mSquaresActor->VisibilityOn();
  }

  if (mListOfCachedContourActors[mPreviousOrientation][mPreviousSlice] != NULL)
    mListOfCachedContourActors[mPreviousOrientation][mPreviousSlice]->VisibilityOff();
  mPreviousSlice = mSlice;
  mPreviousOrientation = orientation;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::CreateNewActor(int numImage) {
  vtkSmartPointer<vtkActor> squaresActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkImageClip> clipper = vtkSmartPointer<vtkImageClip>::New();
  vtkSmartPointer<vtkMarchingSquares> squares = vtkSmartPointer<vtkMarchingSquares>::New();
  vtkSmartPointer<vtkPolyDataMapper> squaresMapper = vtkSmartPointer<vtkPolyDataMapper>::New();

  if (mHiddenImageIsUsed)
    clipper->SetInput(mHiddenImage->GetVTKImages()[0]);
  else
    clipper->SetInput(mSlicer->GetImage()->GetVTKImages()[numImage]);
  
  squares->SetInput(clipper->GetOutput());
  squaresMapper->SetInput(squares->GetOutput());
  squaresMapper->ScalarVisibilityOff();
  squaresActor->SetMapper(squaresMapper);
  squaresActor->GetProperty()->SetColor(1.0,0,0);
  squaresActor->SetPickable(0);
  squaresActor->VisibilityOff();
  mSlicer->GetRenderer()->AddActor(squaresActor);

  mSquaresActorList.push_back(squaresActor);
  mClipperList.push_back(clipper);
  mSquaresList.push_back(squares);
  mSquaresMapperList.push_back(squaresMapper);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::UpdateActor(vtkActor * actor, 
                                 vtkPolyDataMapper * mapper, 
                                 vtkMarchingSquares * squares, 
                                 vtkImageClip * clipper, 
                                 double threshold, int orientation, int slice) {
  // Set parameter for the MarchigSquare
  squares->SetValue(0, threshold);

  // Get image extent
  int* extent = mSlicer->GetImageActor()->GetDisplayExtent();

  // Change extent if needed
  int* extent2;
  if (mHiddenImageIsUsed) {
    extent2 = new int[6];
    int * extent3;
    extent3 = mHiddenImage->GetFirstVTKImageData()->GetExtent();

    for(int i=0; i<6; i++) extent2[i] = extent3[i];

    double s = (double)extent[orientation*2]*(double)mSlicer->GetImage()->GetSpacing()[orientation]; // in mm
    s = s+mSlicer->GetImage()->GetOrigin()[orientation]; // from origin
    s = s-mHiddenImage->GetFirstVTKImageData()->GetOrigin()[orientation]; // from corner second image
    s = s/mHiddenImage->GetFirstVTKImageData()->GetSpacing()[orientation]; // in voxel

    // Rint to the closest slice
    extent2[orientation*2+1] = extent2[orientation*2] = (int)lrint(s);

    // Do not display a contour if there is no contour on this slice
    // DD(extent2[orientation*2+1]);
    // DD(extent3[orientation*2+1]);
    // DD(extent2[orientation*2]);
    // DD(extent3[orientation*2]);
    if ((extent2[orientation*2+1] > extent3[orientation*2+1]) ||
        (extent2[orientation*2] < extent3[orientation*2])) {
      actor->VisibilityOff();
      return;
    }
    else actor->VisibilityOn();

  } else {
    extent2 = extent;
    actor->VisibilityOn();
  }
 
  clipper->SetOutputWholeExtent(extent2[0],extent2[1],extent2[2],
                                extent2[3],extent2[4],extent2[5]);

  if (mHiddenImageIsUsed) delete extent2;

  // Move the actor to be visible
  double position[3] = {0, 0, 0};
  position[orientation] = -mDepth;
  actor->SetPosition(position);
  
  mapper->Update();
}
//------------------------------------------------------------------------------


