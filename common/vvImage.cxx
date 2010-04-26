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
vvImage::vvImage() {
  Init();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void vvImage::Init() {
    mTimeSpacing = 1;
    mTimeOrigin = 0;
    
    if (CLITK_EXPERIMENTAL)
    {
        mVtkImageReslice = vtkSmartPointer<vtkImageReslice>::New();
	transform = vtkSmartPointer<vtkTransform>::New();  
        mVtkImageReslice->SetInterpolationModeToLinear();
        mVtkImageReslice->AutoCropOutputOn();
	mVtkImageReslice->SetBackgroundColor(-1000,-1000,-1000,1);
	mVtkImageReslice->SetResliceTransform(transform);
    }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
vvImage::~vvImage() {
    Reset();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void vvImage::Reset() {
    if (CLITK_EXPERIMENTAL)
    {
        for (unsigned int i = 0; i < mVtkImages.size(); i++)
            mVtkImages[i] = (vtkImageData*)mVtkImageReslice->GetInput(i);
        mVtkImageReslice->RemoveAllInputs();
    }
    for (unsigned int i = 0; i < mVtkImages.size(); i++)
        mVtkImages[i]->Delete();
    mVtkImages.resize(0);
    Init();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void vvImage::SetImage(std::vector< vtkImageData* > images) {
    Reset();
    for (unsigned int i = 0; i < images.size(); i++)
        AddImage(images[i]);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void vvImage::AddImage(vtkImageData* image) {
    if (CLITK_EXPERIMENTAL)
    {
        mVtkImageReslice->SetInput(mVtkImages.size(), image);
        mVtkImageReslice->Update();
        mVtkImages.push_back( mVtkImageReslice->GetOutput( mVtkImages.size() ) );
    }
    else
        mVtkImages.push_back(image);
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
      if (i == 3)
        spacing.push_back(mTimeSpacing);
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
      if (i == 3)
        origin.push_back(mTimeOrigin);
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

//--------------------------------------------------------------------

void vvImage::SetVTKImageReslice(vtkImageReslice* reslice) {
   mVtkImageReslice=reslice;
    
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
vtkAbstractTransform * vvImage::GetTransform()
{
    return mVtkImageReslice->GetResliceTransform();
}
//--------------------------------------------------------------------

//-------------------------------------------------------------------
void vvImage::SetRotateX(int xvalue)
{
    transform->PostMultiply();
    transform->Translate(-origin[0],-origin[1],-origin[2]);
    transform->RotateX(xvalue);
    transform->Translate(origin[0],origin[1],origin[2]);
     mVtkImageReslice->Update();
}

//--------------------------------------------------------------------

//-------------------------------------------------------------------
void vvImage::SetRotateY(int yvalue)
{
    transform->PostMultiply();
    transform->Translate(-origin[0],-origin[1],-origin[2]);
    transform->RotateY(yvalue);
    transform->Translate(origin[0],origin[1],origin[2]);
     mVtkImageReslice->Update();
}
//--------------------------------------------------------------------

//-------------------------------------------------------------------
void vvImage::SetRotateZ(int zvalue)
{
    transform->PostMultiply();
    transform->Translate(-origin[0],-origin[1],-origin[2]);
    transform->RotateZ(zvalue);
    transform->Translate(origin[0],origin[1],origin[2]);
     mVtkImageReslice->Update();
}
//--------------------------------------------------------------------

//-------------------------------------------------------------------
void vvImage::SetTranslationX(int xvalue)
{ 
    transform->Translate(xvalue,0,0);
     mVtkImageReslice->Update();
}
//--------------------------------------------------------------------

//-------------------------------------------------------------------

void vvImage::SetTranslationY(int yvalue)
{ 
    transform->Translate(0,yvalue,0);
     mVtkImageReslice->Update();
}
//--------------------------------------------------------------------

//-------------------------------------------------------------------
void vvImage::SetTranslationZ(int zvalue)
{ 
    transform->Translate(0,0,zvalue);
     mVtkImageReslice->Update();
}
//-------------------------------------------------------------------

//-------------------------------------------------------------------
void vvImage::SetOrigin(double value[3])
{  
  origin=new double[mVtkImageReslice->GetOutputDimensionality()];
  origin[0]=value[0];
  origin[1]=value[1];
  origin[2]=value[2];
}

//--------------------------------------------------------------------
void vvImage::SetTransform(vtkAbstractTransform  *transform)
{
  mVtkImageReslice->SetResliceTransform(transform);
  mVtkImageReslice->Update();
}
//--------------------------------------------------------------------

#endif // VVIMAGE_CXX
