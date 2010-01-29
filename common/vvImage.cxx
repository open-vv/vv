#ifndef VVIMAGE_CXX
#define VVIMAGE_CXX

/*-------------------------------------------------------------------------

Program:   vv
Language:  C++
Author :   Pierre Seroul (pierre.seroul@gmail.com)

Copyright (C) 2008
Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
CREATIS-LRMN http://www.creatis.insa-lyon.fr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

-------------------------------------------------------------------------*/

#include "vvImage.h"
#include "vtkImageData.h"
#include "clitkCommon.h"
#include <cassert>

//--------------------------------------------------------------------
vvImage::vvImage() {
    mVtkImages.resize(0);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
vvImage::~vvImage() {
    for (unsigned int i = 0; i < mVtkImages.size(); i++) {
        if (mVtkImages[i] != NULL)
            mVtkImages[i]->Delete();
    }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void vvImage::SetImage(std::vector<vtkImageData*> images) {
    for (unsigned int i = 0; i < mVtkImages.size(); i++) {
        if (mVtkImages[i] != NULL)
            mVtkImages[i]->Delete();
    }
    mVtkImages.resize(0);
    for (unsigned int i = 0; i < images.size(); i++) {
        mVtkImages.push_back(images[i]);
    }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void vvImage::AddImage(vtkImageData* image) {
    mVtkImages.push_back(image);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void vvImage::Init() {
    for (unsigned int i = 0; i < mVtkImages.size(); i++) {
        if (mVtkImages[i] != NULL)
            mVtkImages[i]->Delete();
    }
    mVtkImages.resize(0);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
int vvImage::GetNumberOfSpatialDimensions() {
    int dim=GetNumberOfDimensions();
    if (IsTimeSequence())
        return dim-1;
    else
        return dim;
}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
int vvImage::GetNumberOfDimensions() const {
    if (mVtkImages.size())
    {
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
    range[0]=temp[0];range[1]=temp[1];
    for (unsigned int i=1;i<mVtkImages.size();i++)
    {
        temp = mVtkImages[i]->GetScalarRange();
        if (temp[0] < range[0]) range[0]=temp[0];
        if (temp[1] > range[1]) range[1]=temp[1];
    }
}

//--------------------------------------------------------------------
std::string vvImage::GetScalarTypeAsString() {
    return mVtkImages[0]->GetScalarTypeAsString();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
int vvImage::GetNumberOfScalarComponents() {
    return mVtkImages[0]->GetNumberOfScalarComponents();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
int vvImage::GetScalarSize() {
    return mVtkImages[0]->GetScalarSize();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
std::vector<double> vvImage::GetSpacing() {
    std::vector<double> spacing;
    int dim = this->GetNumberOfDimensions();
    for (int i = 0; i < dim; i++)
    {
        if (i == 3)
            spacing.push_back(1);
        else
            spacing.push_back(mVtkImages[0]->GetSpacing()[i]);
    }
    return spacing;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
std::vector<double> vvImage::GetOrigin() const {
    std::vector<double> origin;
    int dim = this->GetNumberOfDimensions();
    for (int i = 0; i < dim; i++)
    {
        if (i == 3)
            origin.push_back(0);
        else
            origin.push_back(mVtkImages[0]->GetOrigin()[i]);
    }
    return origin;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
std::vector<int> vvImage::GetSize() {
    std::vector<int> size0;
    int dim = this->GetNumberOfDimensions();
    for (int i = 0; i < dim; i++)
    {
        if (i == 3)
            size0.push_back(mVtkImages.size());
        else
            size0.push_back(mVtkImages[0]->GetDimensions()[i]);
    }
    return size0;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
unsigned long vvImage::GetActualMemorySize() {
    unsigned long size = 0;
    for (unsigned int i = 0; i < mVtkImages.size(); i++) {
        size += mVtkImages[i]->GetActualMemorySize();
    }
    return size;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
bool vvImage::IsTimeSequence() {
  return mVtkImages.size()>1;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::vector<vtkImageData*>& vvImage::GetVTKImages() {
  return mVtkImages;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
vtkImageData* vvImage::GetFirstVTKImageData() { 
  return mVtkImages[0]; 
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
bool vvImage::IsScalarTypeInteger() {
  assert(mVtkImages.size()> 0);
  int t = mVtkImages[0]->GetScalarType();
  return IsScalarTypeInteger(t);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
bool vvImage::IsScalarTypeInteger(int t) {
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
  }
  else {
    return false;
  }
}
//--------------------------------------------------------------------


#endif // VVIMAGE_CXX
