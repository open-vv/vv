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
#ifndef clitkInvertVFGenericFilter_txx
#define clitkInvertVFGenericFilter_txx

#include "itkVectorResampleImageFilter.h"
#include "clitkBSplineDeformableTransform.h"
#if ITK_VERSION_MAJOR >= 4
#include "itkTransformToDisplacementFieldSource.h"
#else
#include "itkTransformToDeformationFieldSource.h"
#endif

/* =================================================
 * @file   clitkInvertVFGenericFilter.txx
 * @author
 * @date
 *
 * @brief
 *
 ===================================================*/


namespace clitk
{

//-----------------------------------------------------------
// Constructor
//-----------------------------------------------------------
template<class args_info_type>
InvertVFGenericFilter<args_info_type>::InvertVFGenericFilter()
{
  m_Verbose=false;
  m_InputFileName="";
}


//-----------------------------------------------------------
// Update
//-----------------------------------------------------------
template<class args_info_type>
void InvertVFGenericFilter<args_info_type>::Update()
{
  // Read the Dimension and PixelType
  int Dimension;
  std::string PixelType;
  ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType);


  // Call UpdateWithDim
  if(Dimension==2) UpdateWithDim<2>(PixelType);
  else if(Dimension==3) UpdateWithDim<3>(PixelType);
  // else if (Dimension==4)UpdateWithDim<4>(PixelType);
  else {
    std::cout<<"Error, Only for 2 or 3  Dimensions!!!"<<std::endl ;
    return;
  }
}

//-------------------------------------------------------------------
// Update with the number of dimensions
//-------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dimension>
void
InvertVFGenericFilter<args_info_type>::UpdateWithDim(std::string PixelType)
{
  if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<< PixelType<<"..."<<std::endl;

  //    if(PixelType == "short"){
  //       if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
  //       UpdateWithDimAndPixelType<Dimension, signed short>();
  //     }
  //    else if(PixelType == "unsigned_short"){
  //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
  //       UpdateWithDimAndPixelType<Dimension, unsigned short>();
  //     }

  //     else if (PixelType == "unsigned_char"){
  //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
  //       UpdateWithDimAndPixelType<Dimension, unsigned char>();
  //     }

  //     else if (PixelType == "char"){
  //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
  //       UpdateWithDimAndPixelType<Dimension, signed char>();
  //     }
  //  else {
  if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
  UpdateWithDimAndPixelType<Dimension, itk::Vector<float, Dimension> >();
  // }
}

//-------------------------------------------------------------------
// Convert Coefficient image to DVF
//-------------------------------------------------------------------
template<class args_info_type>
template<class DisplacementFieldType>
typename DisplacementFieldType::Pointer
InvertVFGenericFilter<args_info_type>::CoeffsToDVF(std::string fileName, std::string likeFileName)
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


//-------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//-------------------------------------------------------------------
template<class args_info_type>
template <unsigned int Dimension, class  PixelType>
void
InvertVFGenericFilter<args_info_type>::UpdateWithDimAndPixelType()
{

  // ImageTypes
  typedef itk::Image<PixelType, Dimension> InputImageType;
  typedef itk::Image<PixelType, Dimension> OutputImageType;

  // Read the input
  typedef itk::ImageFileReader<InputImageType> InputReaderType;
  typename InputReaderType::Pointer reader = InputReaderType::New();
  reader->SetFileName( m_InputFileName);
  reader->Update();
  typename InputImageType::Pointer input = reader->GetOutput();

  // Filter
  typename OutputImageType::Pointer output;
  switch (m_ArgsInfo.type_arg) {

    // clitk filter
  case 0: {
    // Create the InvertVFFilter
    typedef clitk::InvertVFFilter<InputImageType,OutputImageType> FilterType;
    typename FilterType::Pointer filter =FilterType::New();
    if (m_ArgsInfo.like_given) {
      typename FilterType::SpacingType spacing;
      typename FilterType::SizeType size;
      itk::ImageIOBase::Pointer header = readImageHeader(m_ArgsInfo.like_arg);
      for(unsigned int i=0; i<InputImageType::ImageDimension; i++) {
        size[i] = header->GetDimensions(i);
        spacing[i] = header->GetSpacing(i);
      }

      typedef itk::VectorResampleImageFilter<InputImageType, OutputImageType> ResampleFilterType;
      typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();
      resampler->SetInput(input);
      resampler->SetOutputOrigin(input->GetOrigin());
      resampler->SetOutputDirection(input->GetDirection());
      resampler->SetOutputSpacing(spacing);
      resampler->SetSize(size);
      resampler->Update();
      spacing = resampler->GetOutput()->GetSpacing();
      size = resampler->GetOutput()->GetLargestPossibleRegion().GetSize();
      filter->SetInput(resampler->GetOutput());
    }
    else
      filter->SetInput(input);

    filter->SetVerbose(m_Verbose);
    if (m_ArgsInfo.threads_given) filter->SetNumberOfThreads(m_ArgsInfo.threads_arg);
    if (m_ArgsInfo.pad_given) {
      PixelType pad;
      if (m_ArgsInfo.pad_given !=  (pad.GetNumberOfComponents()) )
        pad.Fill(m_ArgsInfo.pad_arg[0]);
      else
        for(unsigned int i=0; i<Dimension; i++)
          pad[i]=m_ArgsInfo.pad_arg[i];
    }
    filter->SetThreadSafe(m_ArgsInfo.threadSafe_flag);
    filter->Update();
    output=filter->GetOutput();

    break;
  }

  case 1: {
    // Create the InvertVFFilter
    typedef clitk::InvertVFFilter<InputImageType,OutputImageType> FilterType;
    typename FilterType::Pointer filter =FilterType::New();
    if (m_ArgsInfo.like_given) {
      filter->SetInput(CoeffsToDVF<OutputImageType>(m_InputFileName, m_ArgsInfo.like_arg));
    }

    filter->SetVerbose(m_Verbose);
    if (m_ArgsInfo.threads_given) filter->SetNumberOfThreads(m_ArgsInfo.threads_arg);
    if (m_ArgsInfo.pad_given) {
      PixelType pad;
      if (m_ArgsInfo.pad_given !=  (pad.GetNumberOfComponents()) )
        pad.Fill(m_ArgsInfo.pad_arg[0]);
      else
        for(unsigned int i=0; i<Dimension; i++)
          pad[i]=m_ArgsInfo.pad_arg[i];
    }
    filter->SetThreadSafe(m_ArgsInfo.threadSafe_flag);
    filter->Update();
    output=filter->GetOutput();

    break;
  }

  case 2: {
    // Create the InverseDeformationFieldFilter
#if ITK_VERSION_MAJOR >= 4
    typedef itk::InverseDisplacementFieldImageFilter<InputImageType,OutputImageType> FilterType;
#else
    typedef itk::InverseDeformationFieldImageFilter<InputImageType,OutputImageType> FilterType;
#endif
    typename FilterType::Pointer filter =FilterType::New();
    filter->SetInput(input);
    filter->SetOutputOrigin(input->GetOrigin());
    filter->SetOutputSpacing(input->GetSpacing());
    filter->SetSize(input->GetLargestPossibleRegion().GetSize());
    filter->SetSubsamplingFactor(m_ArgsInfo.sampling_arg);
    filter->Update();
    output=filter->GetOutput();

    break;
  }

    
  }

  // Output
  typedef itk::ImageFileWriter<OutputImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(m_ArgsInfo.output_arg);
  writer->SetInput(output);
  writer->Update();

}


}//end clitk

#endif //#define clitkInvertVFGenericFilter_txx
