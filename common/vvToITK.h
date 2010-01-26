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
#ifndef vvImageToITK_h
#define vvImageToITK_h

#include <vector>
#include "vvImage.h"
#include <itkImage.h>
#include <itkJoinSeriesImageFilter.h>
#include "itkVTKImageToImageFilter.h"

///Converts the vv image to itk, handling the 4D problem
template<unsigned int Dim, class PixelType> std::vector<typename itk::Image<PixelType,Dim>::ConstPointer> vvImageToITKImageVector(vvImage::Pointer vv_image) ///Converts the vv image to itk, handling the 4D problem
{
    assert(Dim < 5 && Dim > 1); // We don't handle anything higher than 4-dimensional (for the moment :-p)
    assert(vv_image->GetVTKImages().size() > 0); //we assume there is something to convert
    typedef itk::Image< PixelType, Dim > OutputImageType;
    std::vector<typename itk::Image<PixelType,Dim>::ConstPointer> result;

    typedef itk::Image< PixelType, Dim >    ConnectorImageType;
    typedef itk::VTKImageToImageFilter<ConnectorImageType> ConnectorType;
    for (unsigned int i = 0; i < vv_image->GetVTKImages().size(); i++)
    {
        typename ConnectorType::Pointer connector = ConnectorType::New();
        connector->SetInput(vv_image->GetVTKImages()[i]);
        connector->Update();
        result.push_back(connector->GetOutput());
    }
    return result;
}

///Converts the vv image to itk, handling the 4D problem
template<class ImageType> typename ImageType::ConstPointer vvImageToITK(vvImage::Pointer vv_image) ///Converts the vv image to itk, handling the 4D problem
{
    const unsigned int Dim=ImageType::ImageDimension;
    assert(Dim < 5 && Dim > 0); // We don't handle anything higher than 4-dimensional (for the moment :-p)
    typedef ImageType OutputImageType;

    if (Dim==4)
    {
        typedef itk::Image< typename ImageType::PixelType,  3 >    ConnectorImageType;
        typedef itk::VTKImageToImageFilter<ConnectorImageType> ConnectorType;
        typedef itk::JoinSeriesImageFilter<ConnectorImageType,OutputImageType> FilterType;


        typename FilterType::Pointer filter = FilterType::New();
        filter->SetOrigin(vv_image->GetOrigin()[3]);
        filter->SetSpacing(vv_image->GetSpacing()[3]);

        for (int i = 0; i < vv_image->GetSize()[3]; i++)
        {
            typename ConnectorType::Pointer connector = ConnectorType::New();
            connector->SetInput(vv_image->GetVTKImages()[i]);
            connector->Update();
            filter->PushBackInput(connector->GetOutput());
        }
        filter->Update();
        return filter->GetOutput();
    }
    else //Dim == 1,2,3
    {
        assert(not vv_image->IsTimeSequence()); //This case isn't implemented
        typedef ImageType  ConnectorImageType;
        typedef itk::VTKImageToImageFilter <ConnectorImageType> ConnectorType;
        typename ConnectorType::Pointer connector = ConnectorType::New();
        connector->SetInput(vv_image->GetVTKImages()[0]);

        connector->Update();
        return connector->GetOutput();
    }
}

///Converts a single time frame of a vv image to itk.
template<unsigned int Dim, class PixelType> typename itk::Image<PixelType,Dim>::ConstPointer vvSingleFrameToITK(vvImage::Pointer vv_image,int frame) ///Converts the vv image to itk, handling the 4D problem
{
    assert(Dim < 4 && Dim > 0);
    typedef itk::Image< PixelType, Dim > OutputImageType;
    typedef itk::Image< PixelType,  Dim >    ConnectorImageType;
    typedef itk::VTKImageToImageFilter <ConnectorImageType> ConnectorType;
    typename ConnectorType::Pointer connector = ConnectorType::New();
    connector->SetInput(vv_image->GetVTKImages()[frame]);
    connector->Update();
    return connector->GetOutput();
}

#endif
