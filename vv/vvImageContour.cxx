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
#include <vtkVersion.h>
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkMarchingSquares.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkInformation.h>

//------------------------------------------------------------------------------
vvImageContour::vvImageContour()
{ //out << __func__ << endl;
  mTSlice = -1;
  mSlice = 0;
  mHiddenImageIsUsed = false;
  mDisplayModeIsPreserveMemory = true;
  SetPreserveMemoryModeEnabled(true);
  mPreviousOrientation = -1;
  mDepth = 1.0;
  mSlice = 0;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvImageContour::~vvImageContour()
{ //out << __func__ << endl;
  mSquaresActorList.clear();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::RemoveActors()
{ //out << __func__ << endl;
  for (unsigned int i = 0; i < mSquaresActorList.size(); i++) {
    if (mSlicer != 0) {
      if (mSlicer!= NULL) {
        if (mSlicer->GetRenderer() != NULL) {
          if (mSquaresActorList[i] != NULL)  {
            mSlicer->GetRenderer()->RemoveActor(mSquaresActorList[i]);
          }
        }
      }   
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::SetSlicer(vvSlicer * slicer) 
{ //out << __func__ << endl;
  mSlicer = slicer;  
  // Create an actor for each time slice
  for (unsigned int numImage = 0; numImage < mSlicer->GetImage()->GetVTKImages().size(); numImage++) {
    CreateNewActor(numImage);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::SetImage(vvImage::Pointer image) 
{ //out << __func__ << endl;
  for (unsigned int numImage = 0; numImage < image->GetVTKImages().size(); numImage++) {
#if VTK_MAJOR_VERSION <= 5
    mClipperList[numImage]->SetInput(image->GetVTKImages()[numImage]);
#else
    mClipperList[numImage]->SetInputData(image->GetVTKImages()[numImage]);
#endif
  }
  mHiddenImageIsUsed = true;
  mHiddenImage = image;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::SetPreserveMemoryModeEnabled(bool b) 
{ //out << __func__ << endl;
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
void vvImageContour::SetColor(double r, double g, double b) 
{ //out << __func__ << endl;
  for(unsigned int i=0; i<mSquaresActorList.size(); i++) {
    mSquaresActorList[i]->GetProperty()->SetColor(r,g,b);
    mSquaresActorList[i]->GetProperty()->SetOpacity(0.995);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::SetLineWidth(double w)
{ //out << __func__ << endl;
  for(unsigned int i=0; i<mSquaresActorList.size(); i++) {
    mSquaresActorList[i]->GetProperty()->SetLineWidth(w);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::HideActors() 
{ //out << __func__ << endl;
  if (!mSlicer) return;
  mSlice = mSlicer->GetSlice();
  for(unsigned int i=0; i<mSquaresActorList.size(); i++) {
    mSquaresActorList[i]->VisibilityOff();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::ShowActors() 
{ //out << __func__ << endl;
  if (!mSlicer) return;
  mSlice = mSlicer->GetSlice();
  mTSlice = mSlicer->GetTSlice();
  mSquaresActorList[mTSlice]->VisibilityOn();
  Update(mValue);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::SetDepth(double d) 
{ //out << __func__ << endl;
  mDepth = d;
  // Move the actor to be visible
  double position[3] = {0, 0, 0};
  int orientation = ComputeCurrentOrientation();
  position[orientation] = -mDepth;

  for(unsigned int i=0; i<mSquaresActorList.size(); i++)
    mSquaresActorList[i]->SetPosition(position);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvImageContour::Update(double value) 
{ //out << __func__ << endl;
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
void vvImageContour::UpdateWithPreserveMemoryMode() 
{ //out << __func__ << endl;
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
void vvImageContour::InitializeCacheMode() 
{ //out << __func__ << endl;
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
int vvImageContour::ComputeCurrentOrientation() 
{ //out << __func__ << endl;
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
void vvImageContour::UpdateWithFastCacheMode() 
{ //out << __func__ << endl;
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
void vvImageContour::CreateNewActor(int numImage) 
{ //out << __func__ << endl;
  vtkSmartPointer<vtkActor> squaresActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkImageClip> clipper = vtkSmartPointer<vtkImageClip>::New();
  vtkSmartPointer<vtkMarchingSquares> squares = vtkSmartPointer<vtkMarchingSquares>::New();
  vtkSmartPointer<vtkPolyDataMapper> squaresMapper = vtkSmartPointer<vtkPolyDataMapper>::New();

  if (mHiddenImageIsUsed) {
#if VTK_MAJOR_VERSION <= 5
    clipper->SetInput(mHiddenImage->GetVTKImages()[0]);
#else
    clipper->SetInputData(mHiddenImage->GetVTKImages()[0]);
#endif
  } else {
#if VTK_MAJOR_VERSION <= 5
    clipper->SetInput(mSlicer->GetImage()->GetVTKImages()[numImage]);
#else
    clipper->SetInputData(mSlicer->GetImage()->GetVTKImages()[numImage]);
#endif
  }
#if VTK_MAJOR_VERSION <= 5
  squares->SetInput(clipper->GetOutput());
  squaresMapper->SetInput(squares->GetOutput());
#else
  squares->SetInputConnection(clipper->GetOutputPort(0));
  squaresMapper->SetInputConnection(squares->GetOutputPort(0));
#endif
  squaresMapper->ScalarVisibilityOff();
  squaresActor->SetMapper(squaresMapper);
  squaresActor->GetProperty()->SetColor(1.0,0,0);
  squaresActor->GetProperty()->SetOpacity(0.995);
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
                                 double threshold, int orientation, int slice) 
{ //out << __func__ << endl;
  // Set parameter for the MarchigSquare
  squares->SetValue(0, threshold);
  squares->Update();
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
  SetDepth(mDepth);
  // double position[3] = {0, 0, 0};
  // DD(mDepth);
  // position[orientation] = -mDepth;
  // actor->SetPosition(position);
  mapper->Update();
}
//------------------------------------------------------------------------------


