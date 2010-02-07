/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvImageContour.cxx,v $
  Language:  C++
  Date:      $Date: 2010/02/07 09:24:46 $
  Version:   $Revision: 1.1 $
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

//------------------------------------------------------------------------------
vvImageContour::vvImageContour() {
  mClipper = vtkImageClip::New();
  mSquares = vtkMarchingSquares::New();
  mSquaresMapper = vtkPolyDataMapper::New();
  mSquaresActor = vtkActor::New();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvImageContour::~vvImageContour() {

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::setSlicer(vvSlicer * slicer) {
  mSlicer = slicer;
  mClipper->SetInput(slicer->GetImage()->GetFirstVTKImageData());
  mSquares->SetInput(mClipper->GetOutput());
  mSquaresMapper->SetInput(mSquares->GetOutput());
  mSquaresMapper->ScalarVisibilityOff();
  mSquaresActor->SetMapper(mSquaresMapper);
  mSquaresActor->GetProperty()->SetColor(1.0,0,0);
  mSquaresActor->SetPickable(0);
  mSlicer->GetRenderer()->AddActor(mSquaresActor1);
  mSquares->Update();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::update() {
  int slice = mSlicer->GetSlice();
  int tslice = mCurrentSliceManager->GetSlicer(0)->GetTSlice();
  DD(tslice);
  DD(slice);

  //  mClipper->SetInput(mCurrentSliceManager->GetSlicer(0)->GetInput());

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
      if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[0] > slice)
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
      if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[1] > slice)
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
      if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[2] > slice)
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
  //  mSquares2->Update();
}
//------------------------------------------------------------------------------

