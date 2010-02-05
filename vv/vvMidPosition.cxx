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
#include <QApplication>

#include <itkWarpImageFilter.h>
#include <itkVectorResampleImageFilter.h>

#include "vvMidPosition.h"
#include "clitkCommon.h"
#include "vvFromITK.h"
#include "vvToITK.h"
#include "clitkInvertVFFilter.h"

vvMidPosition::vvMidPosition() :
    slicer_manager(0), error(false),
    reference_image_index(0),
    p_bar("Computing mid-position...","Cancel", 0,6),
    progress(0)
{
}

void vvMidPosition::Update()
{
    this->start();
    while (this->isRunning())
    {
        this->wait(50);
        this->update_progress();
        qApp->processEvents();
    }
}

//Common typedefs
typedef itk::Vector<float,3> VFPixelType;
typedef itk::Image<VFPixelType,4> VFType;
typedef itk::Image<VFPixelType,3> OutputVFType;

///Internal functions

///This function averages a vector field along the temporal dimension
// the progress parameter is a reference to a counter used to update the progress dialog
itk::Image<itk::Vector<float,3>,3>::Pointer AverageField(itk::Image<itk::Vector<float,3>,4>::ConstPointer vf,int &progress);
///Warps an image frame using the vf passed in parameter
template<class ImagePixelType> vvImage::Pointer WarpRefImage(OutputVFType::Pointer vf,vvImage::Pointer image,int reference_image_index);

void vvMidPosition::run()
{
    error=true;
    if(slicer_manager->GetImage()->GetNumberOfDimensions() != 4)
        error_message="Computation of midposition is only supported for 4D image sequences.";
    else if (!slicer_manager->GetVF())
        error_message="A VF is required for midposition computation";
    else if (slicer_manager->GetVF()->GetScalarTypeAsString() != "float")
        error_message="Please use a vector field of type float.";
    else
    {
        VFType::ConstPointer vf = vvImageToITK<VFType>(slicer_manager->GetVF());
        OutputVFType::Pointer avg_vf=AverageField(vf,this->progress);
        clitk::InvertVFFilter<OutputVFType,OutputVFType>::Pointer inv_filter=
            clitk::InvertVFFilter<OutputVFType,OutputVFType>::New();
        inv_filter->SetInput(avg_vf);
        inv_filter->Update();
        progress++;
        if (slicer_manager->GetImage()->GetScalarTypeAsString() == "short")
            this->output=WarpRefImage<short>(inv_filter->GetOutput(),slicer_manager->GetImage(),reference_image_index);
        else
        {
            error_message="Unsupported image pixel type.";
            return;
        }
        progress++;
        error=false;
    }
}

template<class ImagePixelType>
vvImage::Pointer WarpRefImage(OutputVFType::Pointer vf,vvImage::Pointer image,int reference_image_index)
{
    typedef itk::Image<ImagePixelType,3> ImageType;
    typedef itk::WarpImageFilter<ImageType,ImageType,itk::Image<VFPixelType,3> > FilterType;

    typename ImageType::ConstPointer input = vvSingleFrameToITK<3,ImagePixelType>(image,reference_image_index);

    //We resample the VF because itk's warp filter doesn't like it when the vf and the image have
    //different spacings
	typename itk::VectorResampleImageFilter<OutputVFType, OutputVFType >::Pointer 
        resampler =itk::VectorResampleImageFilter<OutputVFType, OutputVFType >::New();
	resampler->SetInput(vf);
	resampler->SetOutputSpacing(input->GetSpacing());
	resampler->SetOutputOrigin(vf->GetOrigin());
	//Calculate the new size so that it contains the vf
	typename ImageType::SizeType newSize;
	for (unsigned int i=0 ; i <3; i++)
	  newSize[i]=(unsigned int) (vf->GetLargestPossibleRegion().GetSize()[i]*vf->GetSpacing()[i]/input->GetSpacing()[i]);
	resampler->SetSize( newSize);

    typename FilterType::Pointer warp_filter = FilterType::New();
    warp_filter->SetInput(input);
    warp_filter->SetDeformationField(resampler->GetOutput());
    warp_filter->SetOutputSpacing(input->GetSpacing());
    warp_filter->SetOutputOrigin(input->GetOrigin());
    warp_filter->SetOutputSize(input->GetLargestPossibleRegion().GetSize());
    warp_filter->SetEdgePaddingValue(-1000);
    warp_filter->Update();
    return vvImageFromITK<3,ImagePixelType>(warp_filter->GetOutput());
}

itk::Image<itk::Vector<float,3>,3>::Pointer AverageField(itk::Image<itk::Vector<float,3>,4>::ConstPointer vf, int& progress)
{
    progress++;

    VFType::RegionType region4D=vf->GetLargestPossibleRegion();
    VFType::RegionType::SizeType size4D=region4D.GetSize();
    VFType::IndexType index4D=region4D.GetIndex();
    VFType::SpacingType spacing4D=vf->GetSpacing();
    VFType::PointType origin4D=vf->GetOrigin();

    OutputVFType::RegionType region;
    OutputVFType::RegionType::SizeType size;
    OutputVFType::IndexType index;
    OutputVFType::SpacingType spacing;
    OutputVFType::PointType origin;
    for (unsigned int i=0; i< 3; i++)
    {
        size[i]=size4D[i];
        index[i]=index4D[i];
        spacing[i]=spacing4D[i];
        origin[i]=origin4D[i];
    }
    region.SetSize(size);
    region.SetIndex(index);
    OutputVFType::Pointer output= OutputVFType::New();
    output->SetRegions(region);
    output->SetSpacing(spacing);
    output->SetOrigin(origin);
    output->Allocate();
    progress++;
    

    // Region iterators
    typedef itk::ImageRegionConstIterator<VFType> IteratorType;
    std::vector<IteratorType> iterators(size4D[3]);
    for (unsigned int i=0; i< size4D[3]; i++)
    {
        VFType::RegionType regionIt=region4D;
        VFType::RegionType::SizeType sizeIt=regionIt.GetSize();
        sizeIt[3]=1;
        regionIt.SetSize(sizeIt);
        VFType::IndexType indexIt=regionIt.GetIndex();
        indexIt[3]=i;
        regionIt.SetIndex(indexIt);
        iterators[i]=IteratorType(vf, regionIt);
    }
    progress++;

    typedef itk::ImageRegionIterator<OutputVFType> OutputIteratorType;
    OutputIteratorType avIt(output, output->GetLargestPossibleRegion());

    // Average
    VFPixelType vector;
    VFPixelType zeroVector=itk::NumericTraits<VFPixelType>::Zero;

    while (!(iterators[0]).IsAtEnd())
    {
        vector=zeroVector;
        for (unsigned int i=0; i<size4D[3]; i++)
        {
            vector+=iterators[i].Get();
            ++(iterators[i]);
        }
        vector/=size4D[3];
        avIt.Set(vector);
        ++avIt;
    }
    progress++;
    return output;
}


void vvMidPosition::update_progress()
{
    p_bar.setValue(progress);
    p_bar.show();
}

