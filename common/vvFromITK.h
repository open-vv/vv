/*=========================================================================

 Program:   vv
 Module:    $RCSfile: vvFromITK.h,v $
 Language:  C++
 Date:      $Date: 2010/01/06 13:32:01 $
 Version:   $Revision: 1.1 $
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

#ifndef vvImageFromITK_h
#define vvImageFromITK_h

#include "vvImage.h"
#include <itkImage.h>
#include <itkExtractImageFilter.h>
#include "itkImageToVTKImageFilter.h"

/**Converts the itk image to vv, handling the 4D problem
 * The time_sequence boolean specifies that the image is to be interpreted as a time sequence,
 * even if its dim is < 4. */
template<unsigned int Dim, class PixelType> vvImage::Pointer vvImageFromITK(typename itk::Image<PixelType,Dim>::Pointer input, bool time_sequence=false)
{
    assert(Dim < 5 && Dim > 0); // We don't handle anything higher than 4-dimensional (for the moment :-p)
    vvImage::Pointer vv_image=vvImage::New();
    vv_image->Init(); //Delete any existing images
    typedef itk::Image< PixelType, Dim > InputImageType;

    if (Dim == 4 || time_sequence) //The time sequence case: create a series of VTK images
    {
        typedef itk::Image< PixelType,  Dim - 1 >    ConnectorImageType;
        typedef itk::ImageToVTKImageFilter <ConnectorImageType> ConnectorType;
        typedef itk::ExtractImageFilter<InputImageType,ConnectorImageType> FilterType;

        typename FilterType::Pointer filter = FilterType::New();
        typename ConnectorType::Pointer connector = ConnectorType::New();

        //extract the 3D slices and put them in a std::vector<vtkImageData*>
        typename InputImageType::RegionType inputRegion = input->GetLargestPossibleRegion();
        typename InputImageType::SizeType inputSize = inputRegion.GetSize();

        typename InputImageType::SizeType extractedRegionSize = inputSize;
        typename InputImageType::RegionType extractedRegion;
        extractedRegionSize[Dim - 1] = 0;
        extractedRegion.SetSize(extractedRegionSize);

        filter->SetInput(input);
        connector->SetInput(filter->GetOutput());

        typename InputImageType::IndexType start = inputRegion.GetIndex();

        for (unsigned int i = 0; i < inputSize[Dim - 1]; i++) {
            start[Dim - 1] = i;
            extractedRegion.SetIndex(start);
            filter->SetExtractionRegion(extractedRegion);
            try {
                filter->Update();
            }
            catch ( itk::ExceptionObject & err ) {
                std::cerr << "Error while setting vvImage from ITK (Dim==4) [Extract phase]"
                          << " " << err << std::endl;
                return vv_image;
            }
            try {
                connector->Update();
            }
            catch ( itk::ExceptionObject & err ) {
                std::cerr << "Error while setting vvImage from ITK (Dim==4) [Connect phase]"
                          << " " << err << std::endl;
                return vv_image;
            }
            vtkImageData *image = vtkImageData::New();
            image->DeepCopy(connector->GetOutput());
            vv_image->AddImage(image);
        }
    }
    else //Dim == 1,2,3 and not time_sequence
    {
        typedef itk::Image< PixelType,  Dim >    ConnectorImageType;
        typedef itk::ImageToVTKImageFilter <ConnectorImageType> ConnectorType;
        typename ConnectorType::Pointer connector = ConnectorType::New();
        connector->SetInput(input);

        try {
            connector->Update();
        }
        catch ( itk::ExceptionObject & err ) {
            std::cerr << "Error while setting vvImage from ITK (Dim==3)"
                      << " " << err << std::endl;
            return vv_image;
        }
        vtkImageData *image = vtkImageData::New();
        image->DeepCopy(connector->GetOutput());
        vv_image->AddImage(image);
    }
    return vv_image;
}

#endif //vvImageFromITK
