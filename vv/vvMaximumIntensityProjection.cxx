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
