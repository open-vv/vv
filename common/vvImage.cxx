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
#ifndef VVIMAGE_CXX
#define VVIMAGE_CXX

// vv
#include "vvImage.h"

// clitk
#include "clitkCommon.h"

// vtk
#include <vtkImageData.h>
#include <vtkTransform.h>
#include <vtkDataObject.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
#include <vtkVersion.h>

// std
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
  mImageDimension = 0;
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
  mVtkImages.clear();
  Init();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void vvImage::AddVtkImage(vtkImageData* input)
{
  // RP: 20/12/2011
  // Note that we're simply adding a new image to the vector.
  // mItkToVtkConverters is therefore not being updated, but 
  // up to here it's not being used anyway...
  mImageDimension = 0;
#if VTK_MAJOR_VERSION <= 5
  int* extent = input->GetWholeExtent();
#else
  int* extent = input->GetInformation()->Get(vtkDataObject::DATA_EXTENT());
#endif
  
  if (extent[4] != extent[5])
    mImageDimension = 3;
  else if (extent[3] != extent[4])
    mImageDimension = 2;
  else if (extent[0] != extent[1])
    mImageDimension = 1;
  
  mVtkImages.push_back(input);
}

//--------------------------------------------------------------------

//--------------------------------------------------------------------
int vvImage::GetNumberOfSpatialDimensions()
{
  return mImageDimension;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
int vvImage::GetNumberOfDimensions() const
{
  if (mVtkImages.size()) {
    if (IsTimeSequence())
      return mImageDimension+1;
    else
      return mImageDimension;
  }
  return 0;
}
//--------------------------------------------------------------------

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

//--------------------------------------------------------------------
std::string vvImage::GetScalarTypeAsITKString()
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
bool vvImage::IsTimeSequence() const
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
const std::vector< vtkSmartPointer<vtkTransform> >& vvImage::GetTransform()
{
  return this->mTransform;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
bool vvImage::HaveSameSizeAndSpacingThan(vvImage * other)
{
  bool same = true;
  for(int i=0; i<GetNumberOfDimensions(); i++) {
    if ( GetSize()[i] != other->GetSize()[i]) same = false;
    if ( GetSpacing()[i] != other->GetSpacing()[i]) same = false;
  }
  return same;
}
//--------------------------------------------------------------------
itk::MetaDataDictionary* vvImage::GetFirstMetaDataDictionary()
{
    return mDictionary[0];
}
//--------------------------------------------------------------------


#endif // VVIMAGE_CXX
