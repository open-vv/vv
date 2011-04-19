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
#ifndef VVIMAGEFROMITK_H
#define VVIMAGEFROMITK_H

// vv
#include "vvImage.h"

// itk
#include <itkExtractImageFilter.h>

//------------------------------------------------------------------------------
/**Converts the itk image to vv, handling the 4D problem
 * The time_sequence boolean specifies that the image is to be interpreted as a time sequence,
 * even if its dim is < 4. */
template<unsigned int Dim, class PixelType> vvImage::Pointer vvImageFromITK(typename itk::Image<PixelType,Dim>::Pointer input, bool time_sequence=false)
{
    assert(Dim < 5 && Dim > 0); // We don't handle anything higher than 4-dimensional (for the moment :-p)
    vvImage::Pointer vv_image=vvImage::New();
    typedef itk::Image< PixelType, Dim > InputImageType;

    if (Dim == 4 || time_sequence) //The time sequence case: create a series of VTK images
    {
        typedef itk::Image< PixelType,  Dim - 1 >    ItkImageType;
        typedef itk::ExtractImageFilter<InputImageType, ItkImageType> FilterType;

        //extract the 3D slices and put them in a std::vector<vtkImageData*>
        input->UpdateOutputInformation();
        typename InputImageType::RegionType inputRegion = input->GetLargestPossibleRegion();
        typename InputImageType::SizeType inputSize = inputRegion.GetSize();
        typename InputImageType::IndexType start = inputRegion.GetIndex();
        typename InputImageType::SizeType extractedRegionSize = inputSize;
        typename InputImageType::RegionType extractedRegion;
        extractedRegionSize[Dim - 1] = 0;
        extractedRegion.SetSize(extractedRegionSize);

        for (unsigned int i = 0; i < inputSize[Dim - 1]; i++) {
            start[Dim - 1] = i;
            extractedRegion.SetIndex(start);

            typename FilterType::Pointer filter = FilterType::New();
            filter->SetDirectionCollapseToSubmatrix();
            filter->SetExtractionRegion(extractedRegion);
            filter->SetInput(input);
            filter->ReleaseDataFlagOn();
            vv_image->AddItkImage<ItkImageType>(filter->GetOutput());
        }
        vv_image->SetTimeSpacing(input->GetSpacing()[Dim-1]);
        vv_image->SetTimeOrigin(input->GetOrigin()[Dim-1]);
    }
    else //Dim == 1,2,3 and not time_sequence
    {
        vv_image->AddItkImage<InputImageType>(input);
    }
    return vv_image;
}
//------------------------------------------------------------------------------

#endif //vvImageFromITK
