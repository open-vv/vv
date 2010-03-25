#include "vvImageContour.h"
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
    vtkPolyDataMapper * mSquaresMapper = vtkPolyDataMapper::New();
    vtkActor * mSquaresActor = vtkActor::New();

    mClipper->SetInput(mSlicer->GetImage()->GetVTKImages()[numImage]);
    mSquares->SetInput(mClipper->GetOutput());
    mSquaresMapper->SetInput(mSquares->GetOutput());
    mSquaresMapper->ScalarVisibilityOff();
    mSquaresActor->SetMapper(mSquaresMapper);
    mSquaresActor->GetProperty()->SetColor(1.0,0,0);
    mSquaresActor->SetPickable(0);
    mSquaresActor->VisibilityOff();
    mSlicer->GetRenderer()->AddActor(mSquaresActor);
    
    mSquaresActorList.push_back(mSquaresActor);
    mSquaresList.push_back(mSquares);
    mClipperList.push_back(mClipper);
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
  mValue= value;
  if (!mSlicer) return;

  // how to not update if not visible ?

  mSlice = mSlicer->GetSlice();
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

  // Do it
  mSquares->SetValue(0,value);

  int* extent = mSlicer->GetImageActor()->GetDisplayExtent();
  mClipper->SetOutputWholeExtent(extent[0],extent[1],extent[2],
				 extent[3],extent[4],extent[5]);
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

