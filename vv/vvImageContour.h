/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvImageContour.h,v $
  Language:  C++
  Date:      $Date: 2010/03/05 10:32:33 $
  Version:   $Revision: 1.5 $
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

#include "clitkCommon.h"
#include "vvSlicer.h"
class vtkImageClip;
class vtkMarchingSquares;
class vtkActor;

//------------------------------------------------------------------------------
class vvImageContour
{
  //  Q_OBJECT
 public:
  vvImageContour();
  ~vvImageContour();

  void setSlicer(vvSlicer * slicer);
  void update(double value);
  void hideActors();
  void showActors();
  void setColor(double r, double g, double b);

 protected:
  vvSlicer * mSlicer;
  int mSlice;
  int mTSlice;
  double mValue;

  std::vector<vtkImageClip*> mClipperList;
  std::vector<vtkMarchingSquares*> mSquaresList;
  std::vector<vtkActor*> mSquaresActorList;

}; // end class vvImageContour
//------------------------------------------------------------------------------

#endif

