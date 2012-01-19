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
#ifndef clitkCoeffsToDVF_h
#define clitkCoeffsToDVF_h

#include "clitkBSplineDeformableTransform.h"
#if ITK_VERSION_MAJOR >= 4
#include "itkTransformToDisplacementFieldSource.h"
#else
#include "itkTransformToDeformationFieldSource.h"
#endif

//-------------------------------------------------------------------
// Convert Coefficient image to DVF
//-------------------------------------------------------------------
template<class DisplacementFieldType>
typename DisplacementFieldType::Pointer
CoeffsToDVF(std::string fileName, std::string likeFileName, bool verbose = false)
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
  
  if (verbose) {
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

#endif
