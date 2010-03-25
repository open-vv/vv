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

