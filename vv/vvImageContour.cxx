/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvImageContour.cxx,v $
  Language:  C++
  Date:      $Date: 2010/02/24 11:42:42 $
  Version:   $Revision: 1.3 $
  Author :   David Sarrut (david.sarrut@creatis.insa-lyon.fr)

  Copyright (C) 2010
  Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
  CREATIS                   http://www.creatis.insa-lyon.fr

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

#include "vvImageContour.h"
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkMarchingSquares.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>

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
    // DD(numImage);
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
  //mSquares->Update();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::removeActors() {
 //  DD("removeActors");
  if (!mSlicer) return;
  mSlice = mSlicer->GetSlice();
 //  DD(mSlice);
  for(unsigned int i=0; i<mSquaresActorList.size(); i++) {
    mSquaresActorList[i]->VisibilityOff();
    //mSquaresActorList[i]->Update();
  }
}
//------------------------------------------------------------------------------

  
//------------------------------------------------------------------------------
void vvImageContour::update(int value) {
  //  DD(value);
  if (!mSlicer) return;
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
	  // mSquaresActor2->SetPosition(1,0,0);
        }
      else
        {
	  mSquaresActor->SetPosition(-1,0,0);
	  // mSquaresActor2->SetPosition(-1,0,0);
        }
      break;
    case 2:
      if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[1] > mSlice)
        {
	  mSquaresActor->SetPosition(0,1,0);
          //   mSquaresActor2->SetPosition(0,1,0);
        }
      else
        {
	  mSquaresActor->SetPosition(0,-1,0);
	  // mSquaresActor2->SetPosition(0,-1,0);
        }
      break;
    case 4:
      if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[2] > mSlice)
        {
	  mSquaresActor->SetPosition(0,0,1);
	  // mSquaresActor2->SetPosition(0,0,1);
        }
      else
        {
	  mSquaresActor->SetPosition(0,0,-1);
	  // mSquaresActor2->SetPosition(0,0,-1);
        }
      break;
    }
  mSquares->Update();
}
//------------------------------------------------------------------------------

