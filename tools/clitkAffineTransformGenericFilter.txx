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
#ifndef clitkAffineTransformGenericFilter_txx
#define clitkAffineTransformGenericFilter_txx

/* =================================================
 * @file   clitkAffineTransformGenericFilter.txx
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
AffineTransformGenericFilter<args_info_type>::AffineTransformGenericFilter()
{
  m_Verbose=false;
  m_InputFileName="";
}


//-----------------------------------------------------------
// Update
//-----------------------------------------------------------
template<class args_info_type>
void AffineTransformGenericFilter<args_info_type>::Update()
{
  // Read the Dimension and PixelType
  int Dimension, Components;
  std::string PixelType;
  ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType, Components);


  // Call UpdateWithDim
  if(Dimension==2) UpdateWithDim<2>(PixelType, Components);
  else if(Dimension==3) UpdateWithDim<3>(PixelType, Components);
  else if (Dimension==4)UpdateWithDim<4>(PixelType, Components);
  else {
    std::cout<<"Error, Only for 2, 3 or 4  Dimensions!!!"<<std::endl ;
    return;
  }
}

//-------------------------------------------------------------------
// Update with the number of dimensions
//-------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dimension>
void
AffineTransformGenericFilter<args_info_type>::UpdateWithDim(std::string PixelType, int Components)
{
  if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<<Components<<" component(s) of "<<  PixelType<<"..."<<std::endl;

  if (Components==1) {
    if(PixelType == "short") {
      if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
      UpdateWithDimAndPixelType<Dimension, signed short>();
    }
    //    else if(PixelType == "unsigned_short"){
    //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
    //       UpdateWithDimAndPixelType<Dimension, unsigned short>();
    //     }

    else if (PixelType == "unsigned_char") {
      if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
      UpdateWithDimAndPixelType<Dimension, unsigned char>();
    }

    //     else if (PixelType == "char"){
    //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
    //       UpdateWithDimAndPixelType<Dimension, signed char>();
    //     }
    else {
      if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
      UpdateWithDimAndPixelType<Dimension, float>();
    }
  }

  else if (Components==3) {
    if (m_Verbose) std::cout  << "Launching transform in "<< Dimension <<"D and 3D float (DVF)" << std::endl;
    UpdateWithDimAndVectorType<Dimension, itk::Vector<float, Dimension> >();
  }

  else std::cerr<<"Number of components is "<<Components<<", not supported!"<<std::endl;

}


//-------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//-------------------------------------------------------------------
template<class args_info_type>
template <unsigned int Dimension, class  PixelType>
void
AffineTransformGenericFilter<args_info_type>::UpdateWithDimAndPixelType()
{

  // ImageTypes
  typedef itk::Image<PixelType, Dimension> InputImageType;
  typedef itk::Image<PixelType, Dimension> OutputImageType;

  // Read the input
  typedef itk::ImageFileReader<InputImageType> InputReaderType;
  typename InputReaderType::Pointer reader = InputReaderType::New();
  reader->SetFileName( m_InputFileName);
  reader->Update();
  typename InputImageType::Pointer input= reader->GetOutput();

  //Filter
  typedef  itk::ResampleImageFilter< InputImageType,OutputImageType >  ResampleFilterType;
  typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();

  // Matrix
  typename itk::Matrix<double, Dimension+1, Dimension+1> matrix;
  if (m_ArgsInfo.matrix_given) {
    matrix= clitk::ReadMatrix<Dimension>(m_ArgsInfo.matrix_arg);
    if (m_Verbose) std::cout<<"Reading the matrix..."<<std::endl;
  } else {
    matrix.SetIdentity();
  }
  if (m_Verbose) std::cout<<"Using the following matrix:"<<std::endl;
  if (m_Verbose) std::cout<<matrix<<std::endl;
  typename itk::Matrix<double, Dimension, Dimension> rotationMatrix=clitk::GetRotationalPartMatrix(matrix);
  typename itk::Vector<double,Dimension> translationPart= clitk::GetTranslationPartMatrix(matrix);

  // Transform
  typedef itk::AffineTransform<double, Dimension> AffineTransformType;
  typename AffineTransformType::Pointer affineTransform=AffineTransformType::New();
  affineTransform->SetMatrix(rotationMatrix);
  affineTransform->SetTranslation(translationPart);

  // Interp
  typedef clitk::GenericInterpolator<args_info_type, InputImageType, double> GenericInterpolatorType;
  typename GenericInterpolatorType::Pointer genericInterpolator=GenericInterpolatorType::New();
  genericInterpolator->SetArgsInfo(m_ArgsInfo);

  // Properties
  if (m_ArgsInfo.like_given) {
    typename InputReaderType::Pointer likeReader=InputReaderType::New();
    likeReader->SetFileName(m_ArgsInfo.like_arg);
    likeReader->Update();
    resampler->SetOutputParametersFromImage(likeReader->GetOutput());
  } else {
    //Size
    typename OutputImageType::SizeType outputSize;
    if (m_ArgsInfo.size_given) {
      for(unsigned int i=0; i< Dimension; i++)
        outputSize[i]=m_ArgsInfo.size_arg[i];
    } else outputSize=input->GetLargestPossibleRegion().GetSize();
    std::cout<<"Setting the size to "<<outputSize<<"..."<<std::endl;

    //Spacing
    typename OutputImageType::SpacingType outputSpacing;
    if (m_ArgsInfo.spacing_given) {
      for(unsigned int i=0; i< Dimension; i++)
        outputSpacing[i]=m_ArgsInfo.spacing_arg[i];
    } else outputSpacing=input->GetSpacing();
    std::cout<<"Setting the spacing to "<<outputSpacing<<"..."<<std::endl;

    //Origin
    typename OutputImageType::PointType outputOrigin;
    if (m_ArgsInfo.origin_given) {
      for(unsigned int i=0; i< Dimension; i++)
        outputOrigin[i]=m_ArgsInfo.origin_arg[i];
    } else outputOrigin=input->GetOrigin();
    std::cout<<"Setting the origin to "<<outputOrigin<<"..."<<std::endl;

    // Set
    resampler->SetSize( outputSize );
    resampler->SetOutputSpacing( outputSpacing );
    resampler->SetOutputOrigin(  outputOrigin );

  }

  resampler->SetInput( input );
  resampler->SetTransform( affineTransform );
  resampler->SetInterpolator( genericInterpolator->GetInterpolatorPointer());
  resampler->SetDefaultPixelValue( static_cast<PixelType>(m_ArgsInfo.pad_arg) );

  try {
    resampler->Update();
  } catch(itk::ExceptionObject) {
    std::cerr<<"Error resampling the image"<<std::endl;
  }

  typename OutputImageType::Pointer output = resampler->GetOutput();

  // Output
  typedef itk::ImageFileWriter<OutputImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(m_ArgsInfo.output_arg);
  writer->SetInput(output);
  writer->Update();

}

//-------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype (components)
//-------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dimension, class PixelType>
void AffineTransformGenericFilter<args_info_type>::UpdateWithDimAndVectorType()
{
  // ImageTypes
  typedef itk::Image<PixelType, Dimension> InputImageType;
  typedef itk::Image<PixelType, Dimension> OutputImageType;

  // Read the input
  typedef itk::ImageFileReader<InputImageType> InputReaderType;
  typename InputReaderType::Pointer reader = InputReaderType::New();
  reader->SetFileName( m_InputFileName);
  reader->Update();
  typename InputImageType::Pointer input= reader->GetOutput();

  //Filter
  typedef  itk::VectorResampleImageFilter< InputImageType,OutputImageType, double >  ResampleFilterType;
  typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();

  // Matrix
  typename itk::Matrix<double, Dimension+1, Dimension+1> matrix;
  if (m_ArgsInfo.matrix_given)
    matrix= clitk::ReadMatrix<Dimension>(m_ArgsInfo.matrix_arg);
  else
    matrix.SetIdentity();
  if (m_Verbose) std::cout<<"Using the following matrix:"<<std::endl;
  if (m_Verbose) std::cout<<matrix<<std::endl;
  typename itk::Matrix<double, Dimension, Dimension> rotationMatrix=clitk::GetRotationalPartMatrix(matrix);
  typename itk::Vector<double, Dimension> translationPart= clitk::GetTranslationPartMatrix(matrix);

  // Transform
  typedef itk::AffineTransform<double, Dimension> AffineTransformType;
  typename AffineTransformType::Pointer affineTransform=AffineTransformType::New();
  affineTransform->SetMatrix(rotationMatrix);
  affineTransform->SetTranslation(translationPart);

  // Interp
  typedef clitk::GenericVectorInterpolator<args_info_type, InputImageType, double> GenericInterpolatorType;
  typename GenericInterpolatorType::Pointer genericInterpolator=GenericInterpolatorType::New();
  genericInterpolator->SetArgsInfo(m_ArgsInfo);

  // Properties
  if (m_ArgsInfo.like_given) {
    typename InputReaderType::Pointer likeReader=InputReaderType::New();
    likeReader->SetFileName(m_ArgsInfo.like_arg);
    likeReader->Update();
    resampler->SetSize( likeReader->GetOutput()->GetLargestPossibleRegion().GetSize() );
    resampler->SetOutputSpacing( likeReader->GetOutput()->GetSpacing() );
    resampler->SetOutputOrigin(  likeReader->GetOutput()->GetOrigin() );
  } else {
    //Size
    typename OutputImageType::SizeType outputSize;
    if (m_ArgsInfo.size_given) {
      for(unsigned int i=0; i< Dimension; i++)
        outputSize[i]=m_ArgsInfo.size_arg[i];
    } else outputSize=input->GetLargestPossibleRegion().GetSize();
    std::cout<<"Setting the size to "<<outputSize<<"..."<<std::endl;

    //Spacing
    typename OutputImageType::SpacingType outputSpacing;
    if (m_ArgsInfo.spacing_given) {
      for(unsigned int i=0; i< Dimension; i++)
        outputSpacing[i]=m_ArgsInfo.spacing_arg[i];
    } else outputSpacing=input->GetSpacing();
    std::cout<<"Setting the spacing to "<<outputSpacing<<"..."<<std::endl;

    //Origin
    typename OutputImageType::PointType outputOrigin;
    if (m_ArgsInfo.origin_given) {
      for(unsigned int i=0; i< Dimension; i++)
        outputOrigin[i]=m_ArgsInfo.origin_arg[i];
    } else outputOrigin=input->GetOrigin();
    std::cout<<"Setting the origin to "<<outputOrigin<<"..."<<std::endl;

    // Set
    resampler->SetSize( outputSize );
    resampler->SetOutputSpacing( outputSpacing );
    resampler->SetOutputOrigin(  outputOrigin );

  }

  resampler->SetInput( input );
  resampler->SetTransform( affineTransform );
  resampler->SetInterpolator( genericInterpolator->GetInterpolatorPointer());
  resampler->SetDefaultPixelValue( static_cast<PixelType>(m_ArgsInfo.pad_arg) );

  try {
    resampler->Update();
  } catch(itk::ExceptionObject) {
    std::cerr<<"Error resampling the image"<<std::endl;
  }

  typename OutputImageType::Pointer output = resampler->GetOutput();

  // Output
  typedef itk::ImageFileWriter<OutputImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(m_ArgsInfo.output_arg);
  writer->SetInput(output);
  writer->Update();

}


} //end clitk

#endif //#define clitkAffineTransformGenericFilter_txx
