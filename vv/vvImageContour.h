/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvImageContour.h,v $
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

#ifndef VVIMAGECONTOUR_H
#define VVIMAGECONTOUR_H

#include <vtkMarchingCubes.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkImageClip.h>

#include "clitkCommon.h"
#include "vvSlicer.h"

//------------------------------------------------------------------------------
class vvImageContour
{
  //  Q_OBJECT
    public:
  vvImageContour();
  ~vvImageContour();

  void setSlicer(vvSlicer * slicer);
  // vtkActor * getActor() { return mSquaresActor;}
  void update(int value);
  void removeActors();

 protected:
  vvSlicer * mSlicer;
  int mSlice;
  int mTSlice;

  std::vector<vtkImageClip*> mClipperList;
  std::vector<vtkMarchingSquares*> mSquaresList;
  std::vector<vtkActor*> mSquaresActorList;

}; // end class vvImageContour
//------------------------------------------------------------------------------

#endif

