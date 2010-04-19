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
#ifndef VVBINARYIMAGEOVERLAYACTOR_H
#define VVBINARYIMAGEOVERLAYACTOR_H

#include "clitkCommon.h"
#include "vvSlicer.h"

class vtkImageClip;
class vtkMarchingSquares;
class vtkActor;
class vvImage;
class vtkImageMapToRGBA;
class vtkImageActor;

//------------------------------------------------------------------------------
class vvBinaryImageOverlayActor
{
  //  Q_OBJECT
 public:
  vvBinaryImageOverlayActor();
  ~vvBinaryImageOverlayActor();

  void setSlicer(vvSlicer * slicer);
  void update(int slicer, int slice);
  void hideActors();
  void showActors();
  void setColor(double r, double g, double b);
  void setImage(vvImage::Pointer image);
  void initialize();

 protected:
  vvSlicer * mSlicer;
  int mSlice;
  int mTSlice;
  int mPreviousTSlice;
  int mPreviousSlice;
  vvImage::Pointer mImage;
  std::vector<double> mColor;
  double mAlpha;
  
  std::vector<vtkImageMapToRGBA *> mMapperList;
  std::vector<vtkImageActor*> mImageActorList;

  void ComputeExtent(int orientation, 
		     int slice, 
		     int * inExtent,
		     int * outExtent);
  void ComputeExtent(int * inExtent, 
		     int * outExtent, 
		     vtkImageData * image, 
		     vtkImageData * overlay);
  void SetDisplayExtentAndCameraPosition(int orientation, 
					 int slice, 
					 int * extent, 
					 vtkImageActor * actor, 
					 double position);

}; // end class vvBinaryImageOverlayActor
//------------------------------------------------------------------------------

#endif

