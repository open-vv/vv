/*=========================================================================

 Program:   vv
 Language:  C++
 Author :   Joel Schaerer (joel.schaerer@insa-lyon.fr)

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

=========================================================================*/

#include <itkImage.h>
#include <itkMaximumProjectionImageFilter.h>

#include "clitkCommon.h"
#include "vvSlicerManager.h"
#include "vvSlicer.h"
#include "vvToITK.h"
#include "vvFromITK.h"
#include "vvMaximumIntensityProjection.h"

void vvMaximumIntensityProjection::Compute(vvSlicerManager * slicer_manager)
{
#define TRY_TYPE(TYPE)							\
if (clitk::IsSameType<TYPE>(image->GetScalarTypeAsString())) { this->Update_WithPixelType<TYPE>(image); return; }
    std::string list = clitk::CreateListOfTypes<short>();
    vvImage::Pointer image=slicer_manager->GetSlicer(0)->GetImage();
    TRY_TYPE(float);
    TRY_TYPE(short);
    std::cerr << "Error, I don't know the type '" << image->GetScalarTypeAsString() << "' for the input image. "
        << std::endl << "Known types are " << list << std::endl;
    exit(0);
#undef TRY_TYPE
}

template <class PixelType>
void vvMaximumIntensityProjection::Update_WithPixelType(vvImage::Pointer image)
{
    switch(image->GetNumberOfDimensions())
    {
        case 3:
            Update_WithDimAndPixelType<PixelType,3>(image);
            break;;
        case 4:
            Update_WithDimAndPixelType<PixelType,4>(image);
            break;;
        default:
            DD("Error: dimension not handled.");
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
