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

#ifndef VVIMAGETOITK_H
#define VVIMAGETOITK_H

// std
#include <vector>

// vv
#include "vvImage.h"

// itk
#include <itkImage.h>
#include <itkJoinSeriesImageFilter.h>
#include "itkVTKImageToImageFilter.h"

//--------------------------------------------------------------------
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
//--------------------------------------------------------------------


//--------------------------------------------------------------------
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
      assert(!vv_image->IsTimeSequence()); //This case isn't implemented
      typedef ImageType  ConnectorImageType;
      typedef itk::VTKImageToImageFilter <ConnectorImageType> ConnectorType;
      typename ConnectorType::Pointer connector = ConnectorType::New();
      connector->SetInput(vv_image->GetVTKImages()[0]);
      connector->Update();
      return connector->GetOutput();
    }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
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
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<unsigned int Dim, class PixelType> 
typename itk::Image<PixelType,Dim>::ConstPointer 
ItkImageFromVtk(vtkImageData * input)
{
  typedef itk::Image< PixelType, Dim > OutputImageType;
  typedef itk::Image< PixelType,  Dim >    ConnectorImageType;
  typedef itk::VTKImageToImageFilter <ConnectorImageType> ConnectorType;
  typename ConnectorType::Pointer connector = ConnectorType::New();
  connector->SetInput(input);
  connector->Update();
  return connector->GetOutput();
}
//--------------------------------------------------------------------



#endif
