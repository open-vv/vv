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
#ifndef VVIMAGE_CXX
#define VVIMAGE_CXX
#include "vvImage.h"
#include "clitkCommon.h"
#include <vtkTransform.h>
#include <vtkImageData.h>
#include <vtkImageReslice.h>
#include <cassert>

//--------------------------------------------------------------------
vvImage::vvImage()
{
  Init();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void vvImage::Init()
{
  mTimeSpacing = 1;
  mTimeOrigin = 0;

  if (CLITK_EXPERIMENTAL)
    mTransform = vtkSmartPointer<vtkTransform>::New();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
vvImage::~vvImage()
{
  Reset();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void vvImage::Reset()
{
  if (CLITK_EXPERIMENTAL)
    for (unsigned int i = 0; i < mVtkImages.size(); i++)
      mVtkImageReslice[i]->GetInput(0)->Delete();
  else
    for (unsigned int i = 0; i < mVtkImages.size(); i++)
      mVtkImages[i]->Delete();

  mVtkImages.resize(0);
  mVtkImageReslice.resize(0);

  Init();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void vvImage::SetImage(std::vector< vtkImageData* > images)
{
  Reset();
  for (unsigned int i = 0; i < images.size(); i++)
    AddImage(images[i]);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void vvImage::AddImage(vtkImageData* image)
{
  if (CLITK_EXPERIMENTAL) {
    mVtkImageReslice.push_back(vtkSmartPointer<vtkImageReslice>::New());

    mVtkImageReslice.back()->SetInterpolationModeToLinear();
    mVtkImageReslice.back()->AutoCropOutputOn();
    mVtkImageReslice.back()->SetBackgroundColor(-1000,-1000,-1000,1);
    mVtkImageReslice.back()->SetResliceTransform(mTransform);
    mVtkImageReslice.back()->SetInput(0, image);
    mVtkImageReslice.back()->Update();

    mVtkImages.push_back( mVtkImageReslice.back()->GetOutput(0) );
  } else
    mVtkImages.push_back(image);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
int vvImage::GetNumberOfSpatialDimensions()
{
  int dim=GetNumberOfDimensions();
  if (IsTimeSequence())
    return dim-1;
  else
    return dim;
}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
int vvImage::GetNumberOfDimensions() const
{
  if (mVtkImages.size()) {
    int dimension = 2;
    int extent[6];
    mVtkImages[0]->GetWholeExtent(extent);
    if (extent[5] - extent[4] >= 1)
      dimension++;
    if (mVtkImages.size() > 1)
      dimension++;
    return dimension;
  }
  return 0;
}
//--------------------------------------------------------------------
void vvImage::GetScalarRange(double* range)
{
  assert(mVtkImages.size());
  double * temp = mVtkImages[0]->GetScalarRange();
  range[0]=temp[0];
  range[1]=temp[1];
  for (unsigned int i=1; i<mVtkImages.size(); i++) {
    temp = mVtkImages[i]->GetScalarRange();
    if (temp[0] < range[0]) range[0]=temp[0];
    if (temp[1] > range[1]) range[1]=temp[1];
  }
}

//--------------------------------------------------------------------
std::string vvImage::GetScalarTypeAsString()
{
  // WARNING VTK pixel type different from ITK Pixel type
  std::string vtktype = mVtkImages[0]->GetScalarTypeAsString();
  if (vtktype == "unsigned char") return "unsigned_char";
  if (vtktype == "unsigned short") return "unsigned_short";
  if (vtktype == "unsigned int") return "unsigned_int";
  return vtktype;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
int vvImage::GetNumberOfScalarComponents()
{
  return mVtkImages[0]->GetNumberOfScalarComponents();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
int vvImage::GetScalarSize()
{
  return mVtkImages[0]->GetScalarSize();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
std::vector<double> vvImage::GetSpacing()
{
  std::vector<double> spacing;
  int dim = this->GetNumberOfDimensions();
  for (int i = 0; i < dim; i++) {
    if (i == 3)
      spacing.push_back(mTimeSpacing);
    else
      spacing.push_back(mVtkImages[0]->GetSpacing()[i]);
  }
  return spacing;
}
//--------------------------------------------------------------------
std::vector<double> vvImage::GetOrigin() const
{
  std::vector<double> origin;
  int dim = this->GetNumberOfDimensions();
  for (int i = 0; i < dim; i++) {
    if (i == 3)
      origin.push_back(mTimeOrigin);
    else
      origin.push_back(mVtkImages[0]->GetOrigin()[i]);
  }
  return origin;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
std::vector<int> vvImage::GetSize()
{
  std::vector<int> size0;
  int dim = this->GetNumberOfDimensions();
  for (int i = 0; i < dim; i++) {
    if (i == 3)
      size0.push_back(mVtkImages.size());
    else
      size0.push_back(mVtkImages[0]->GetDimensions()[i]);
  }
  return size0;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
unsigned long vvImage::GetActualMemorySize()
{
  unsigned long size = 0;
  for (unsigned int i = 0; i < mVtkImages.size(); i++) {
    size += mVtkImages[i]->GetActualMemorySize();
  }
  return size;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
bool vvImage::IsTimeSequence()
{
  return mVtkImages.size()>1;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::vector<vtkImageData*>& vvImage::GetVTKImages()
{
  return mVtkImages;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
vtkImageData* vvImage::GetFirstVTKImageData()
{
  return mVtkImages[0];
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
bool vvImage::IsScalarTypeInteger()
{
  assert(mVtkImages.size()> 0);
  int t = mVtkImages[0]->GetScalarType();
  return IsScalarTypeInteger(t);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
bool vvImage::IsScalarTypeInteger(int t)
{
  if ((t == VTK_BIT) ||
      (t == VTK_CHAR) ||
      (t == VTK_UNSIGNED_CHAR) ||
      (t == VTK_SHORT) ||
      (t == VTK_UNSIGNED_SHORT) ||
      (t == VTK_INT) ||
      (t == VTK_UNSIGNED_INT) ||
      (t == VTK_LONG) ||
      (t == VTK_UNSIGNED_LONG))    {
    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------

//--------------------------------------------------------------------
vtkSmartPointer<vtkTransform> vvImage::GetTransform()
{
  return mTransform;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void vvImage::SetTransform(vtkSmartPointer<vtkTransform> transform)
{
  mTransform = transform;
  this->UpdateReslice();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void vvImage::UpdateReslice()
{
  for (unsigned int i=0; i<mVtkImageReslice.size(); i++) {
    mVtkImageReslice[i]->Update();
  }
}
//--------------------------------------------------------------------

#endif // VVIMAGE_CXX
