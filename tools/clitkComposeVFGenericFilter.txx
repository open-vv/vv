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
#ifndef __clitkComposeVFGenericFilter_txx
#define __clitkComposeVFGenericFilter_txx
#include "clitkComposeVFGenericFilter.h"

#include "clitkBSplineDeformableTransform.h"
#include "clitkBSplineDeformableTransformInitializer.h"
#if ITK_VERSION_MAJOR >= 4
#include "itkTransformToDisplacementFieldSource.h"
#else
#include "itkTransformToDeformationFieldSource.h"
#endif

namespace clitk
{

  template<unsigned int Dimension>
  void ComposeVFGenericFilter::UpdateWithDim(std::string PixelType)
  {
    if (PixelType=="double")
      {
	UpdateWithDimAndPixelType<Dimension, double>();
      }     
    else
      {
	UpdateWithDimAndPixelType<Dimension, float>();
      }
  }

  template<class DisplacementFieldType>
  typename DisplacementFieldType::Pointer ComposeVFGenericFilter::CoeffsToDVF(std::string fileName, std::string likeFileName)
  {
    typedef clitk::BSplineDeformableTransform<double, DisplacementFieldType::ImageDimension, DisplacementFieldType::ImageDimension> TransformType;
    typedef typename TransformType::CoefficientImageType CoefficientImageType;

    typedef itk::ImageFileReader<CoefficientImageType> CoeffReaderType;
    typename CoeffReaderType::Pointer reader = CoeffReaderType::New();
    reader->SetFileName(fileName);
    reader->Update();

    typename TransformType::Pointer transform = TransformType::New();
    transform->SetCoefficientImage(reader->GetOutput());
    
#if ITK_VERSION_MAJOR >= 4
        typedef itk::TransformToDisplacementFieldSource<DisplacementFieldType, double> ConvertorType;
#else
        typedef itk::TransformToDeformationFieldSource<DisplacementFieldType, double> ConvertorType;
#endif

    typedef itk::ImageIOBase ImageIOType;
    typename ImageIOType::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(likeFileName.c_str(), itk::ImageIOFactory::ReadMode);
    imageIO->SetFileName(likeFileName);
    imageIO->ReadImageInformation();

    typename ConvertorType::Pointer convertor= ConvertorType::New();
    typename ConvertorType::SizeType output_size;
    typename ConvertorType::SpacingType output_spacing;
    typename ConvertorType::OriginType output_origin;
    typename ConvertorType::DirectionType output_direction;
    for (unsigned int i = 0; i < DisplacementFieldType::ImageDimension; i++) {
      output_size[i] = imageIO->GetDimensions(i);
      output_spacing[i] = imageIO->GetSpacing(i);
      output_origin[i] = imageIO->GetOrigin(i);
      for (unsigned int j = 0; j < DisplacementFieldType::ImageDimension; j++)
        output_direction[i][j] = imageIO->GetDirection(i)[j];
    }
    
    if (m_Verbose) {
      std::cout << "Interpolating coefficients with grid:" << std::endl;
      std::cout << output_size << output_spacing << std::endl;
    }
    
    convertor->SetNumberOfThreads(1);
    convertor->SetTransform(transform);
    convertor->SetOutputOrigin(output_origin);
    convertor->SetOutputSpacing(output_spacing);
    convertor->SetOutputSize(output_size);
    convertor->SetOutputDirection(output_direction);
    convertor->Update();

    return convertor->GetOutput();
  }
  
  template<unsigned int Dimension, class PixelType>
  void ComposeVFGenericFilter::UpdateWithDimAndPixelType()
  {
    typedef itk::Vector<PixelType, Dimension> DisplacementType;
    typedef itk::Image<DisplacementType, Dimension> ImageType;
    typename ImageType::Pointer input1, input2;

    //Define the image type
    if (m_Type == 1) {
      input1 = this->CoeffsToDVF<ImageType>(m_InputName1, m_LikeImage);
      input2 = this->CoeffsToDVF<ImageType>(m_InputName2, m_LikeImage);
    }
    else {
      //Read the input1
      typedef itk::ImageFileReader<ImageType> ImageReaderType;
      typename  ImageReaderType::Pointer reader1= ImageReaderType::New();
      reader1->SetFileName(m_InputName1);
      reader1->Update();
      input1 =reader1->GetOutput();

      //Read the input2
      typename  ImageReaderType::Pointer reader2= ImageReaderType::New();
      reader2->SetFileName(m_InputName2);
      reader2->Update();
      input2=reader2->GetOutput();
    }

      //Create the ComposeVFFilter
      typedef clitk::ComposeVFFilter<ImageType,ImageType> FilterType;
      typename FilterType::Pointer filter =FilterType::New();
      filter->SetInput1(input1);
      filter->SetInput2(input2);
      filter->SetVerbose(m_Verbose);
      filter->Update();

      //Write the output
      typedef itk::ImageFileWriter<ImageType> WriterType;
      typename WriterType::Pointer writer = WriterType::New();
      writer->SetFileName(m_OutputName);
      writer->SetInput(filter->GetOutput());
      writer->Update();
    
  }
}

#endif
