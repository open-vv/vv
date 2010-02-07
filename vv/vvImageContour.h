/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvImageContour.h,v $
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

#ifndef VVIMAGECONTOUR_H
#define VVIMAGECONTOUR_H

#include "vtkMarchingSquares.h"
#include "vtkImageClip.h"
#include "vtkMarchingCubes.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"

#include "vvSlicer.h"

//------------------------------------------------------------------------------
class vvImageContour
{
  //  Q_OBJECT
    public:
  vvImageContour();
  ~vvImageContour();

  void setSlicer(vvSlicer * slicer);
  vtkActor * getActor() { return mSquaresActor;}
  void update();

 protected:
  vvSlicer * mSlicer;

  vtkImageClip* mClipper;
  vtkMarchingSquares* mSquares;
  vtkPolyDataMapper* mSquaresMapper;
  vtkActor* mSquaresActor;

}; // end class vvImageContour
//------------------------------------------------------------------------------

#endif

