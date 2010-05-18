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
#include <itkImage.h>
#include <itkMaximumProjectionImageFilter.h>

#include "clitkCommon.h"
#include "vvSlicerManager.h"
#include "vvSlicer.h"
#include "vvToITK.h"
#include "vvFromITK.h"
#include "vvMaximumIntensityProjection.h"
#include <QMessageBox>

void vvMaximumIntensityProjection::Compute(vvSlicerManager * slicer_manager)
{
#define TRY_TYPE(TYPE)							\
if (clitk::IsSameType<TYPE>(image->GetScalarTypeAsString())) { this->Update_WithPixelType<TYPE>(image); return; }
  std::string list = clitk::CreateListOfTypes<short>();
  vvImage::Pointer image=slicer_manager->GetSlicer(0)->GetImage();
  TRY_TYPE(float);
  TRY_TYPE(double);
  TRY_TYPE(int);
  TRY_TYPE(unsigned int);
  TRY_TYPE(short);
  TRY_TYPE(unsigned short);
  TRY_TYPE(char);
  TRY_TYPE(unsigned char);
  QMessageBox::warning(0,"Unsupported image type",QString("Error, I don't know the type")+QString(image->GetScalarTypeAsString().c_str()) +QString("' for the input image.\nKnown types are ") + QString(list.c_str()));
  error=true;
#undef TRY_TYPE
}

template <class PixelType>
void vvMaximumIntensityProjection::Update_WithPixelType(vvImage::Pointer image)
{
  switch(image->GetNumberOfDimensions()) {
  case 3:
    Update_WithDimAndPixelType<PixelType,3>(image);
    break;;
  case 4:
    Update_WithDimAndPixelType<PixelType,4>(image);
    break;;
  default:
    QMessageBox::warning(0,"Unsupported image dimension",QString("Unsupported image dimension. Supported dimensions are 3 and 4"));
    error=true;
  }
}

template <class PixelType,int Dim>
void vvMaximumIntensityProjection::Update_WithDimAndPixelType(vvImage::Pointer image)
{
  typedef itk::Image<PixelType,Dim> ImageType;
  typedef itk::Image<PixelType,Dim-1> OutputImageType;
  typedef itk::MaximumProjectionImageFilter<ImageType,OutputImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetProjectionDimension(Dim-1);
  typename ImageType::ConstPointer input = vvImageToITK<ImageType>(image);
  filter->SetInput(input);
  filter->Update();
  mOutputImage=vvImageFromITK<Dim-1,PixelType>(filter->GetOutput());
}
