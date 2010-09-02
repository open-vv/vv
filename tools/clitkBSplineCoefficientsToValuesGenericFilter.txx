/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/
#ifndef clitkBSplineCoefficientsToValuesGenericFilter_txx
#define clitkBSplineCoefficientsToValuesGenericFilter_txx

/* =================================================
 * @file   clitkBSplineCoefficientsToValuesGenericFilter.txx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


namespace clitk
{

  //-------------------------------------------------------------------
  // Update with the number of dimensions
  //-------------------------------------------------------------------
  template<unsigned int Dimension>
  void 
  BSplineCoefficientsToValuesGenericFilter::UpdateWithDim(std::string PixelType, unsigned int Components)
  {
    if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<<Components<<" component(s) of "<<  PixelType<<"..."<<std::endl;

    if (Components==1)
      {
	// 	if(PixelType == "short"){  
	// 	  if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
	// 	  UpdateWithDimAndPixelType<Dimension, signed short>(); 
	// 	}
	//    else if(PixelType == "unsigned_short"){  
	//       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
	//       UpdateWithDimAndPixelType<Dimension, unsigned short>(); 
	//     }
	
	// 	else if (PixelType == "unsigned_char"){ 
	// 	  if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
	// 	  UpdateWithDimAndPixelType<Dimension, unsigned char>();
	// 	}
	
	//     else if (PixelType == "char"){ 
	//       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
	//       UpdateWithDimAndPixelType<Dimension, signed char>();
	//     }
	//	else {
	  if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, float>();
	  //	}
      }

   else 
    if (Components==3)
      {
	if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and 3D float (DVF)" << std::endl;
	UpdateWithDimAndVectorType<Dimension, itk::Vector<float, 3> >();
      }

    else std::cerr<<"Number of components is "<<Components<<", not supported!"<<std::endl;

  }


  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template <unsigned int Dimension, class  PixelType> 
  void 
  BSplineCoefficientsToValuesGenericFilter::UpdateWithDimAndPixelType()
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

    // Properties
    if (m_ArgsInfo.like_given)
      {
	typename InputReaderType::Pointer likeReader=InputReaderType::New();
	likeReader->SetFileName(m_ArgsInfo.like_arg);
	likeReader->Update();
	resampler->SetOutputParametersFromImage(likeReader->GetOutput());
	if (m_Verbose)std::cout<<"Resampling output like "<<m_ArgsInfo.like_arg<<"..."<<std::endl;
      }
    else
      {
	// Size
	typename OutputImageType::SizeType outputSize;
	if (m_ArgsInfo.size_given) 
	  {
	    for(unsigned int i=0; i< Dimension; i++)
	      outputSize[i]=m_ArgsInfo.size_arg[i];
	  }
	else outputSize=input->GetLargestPossibleRegion().GetSize();
	if (m_Verbose)std::cout<<"Setting the size to "<<outputSize<<"..."<<std::endl;
	
	// Spacing
	typename OutputImageType::SpacingType outputSpacing;
	if (m_ArgsInfo.spacing_given) 
	  {
	    for(unsigned int i=0; i< Dimension; i++)
	      outputSpacing[i]=m_ArgsInfo.spacing_arg[i];
	  }
	else outputSpacing=input->GetSpacing();
	if (m_Verbose)std::cout<<"Setting the spacing to "<<outputSpacing<<"..."<<std::endl;
    
	// Origin
	typename OutputImageType::PointType outputOrigin;
	if (m_ArgsInfo.origin_given) 
	  {
	    for(unsigned int i=0; i< Dimension; i++)
	      outputOrigin[i]=m_ArgsInfo.origin_arg[i];
	  }
	else outputOrigin=input->GetOrigin();
	if (m_Verbose)std::cout<<"Setting the origin to "<<outputOrigin<<"..."<<std::endl;
    
	// Set
	resampler->SetSize( outputSize );
	resampler->SetOutputSpacing( outputSpacing );
	resampler->SetOutputOrigin(  outputOrigin );

      }

    // Interp : coeff   
    typedef itk::BSplineResampleImageFunction<InputImageType, double> InterpolatorType;
    typename InterpolatorType::Pointer interpolator=InterpolatorType::New();
    interpolator->SetSplineOrder(m_ArgsInfo.order_arg);

    // Set
    resampler->SetInterpolator( interpolator);
    resampler->SetDefaultPixelValue( static_cast<PixelType>(m_ArgsInfo.pad_arg) );
    resampler->SetInput( input );

    // Update
    try
      {
	resampler->Update();
      }
    catch(itk::ExceptionObject)
      {
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
  // Update with the number of dimensions and the Vectortype
  //-------------------------------------------------------------------
  template <unsigned int Dimension, class  PixelType> 
  void 
  BSplineCoefficientsToValuesGenericFilter::UpdateWithDimAndVectorType()
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
    typedef  itk::VectorResampleImageFilter< InputImageType,OutputImageType >  ResampleFilterType;
    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    // Properties
    if (m_ArgsInfo.like_given)
      {
	typename InputReaderType::Pointer likeReader=InputReaderType::New();
	likeReader->SetFileName(m_ArgsInfo.like_arg);
	likeReader->Update();

	// Set
	resampler->SetSize( likeReader->GetOutput()->GetLargestPossibleRegion().GetSize() );
	resampler->SetOutputSpacing(likeReader->GetOutput()->GetSpacing() );
	resampler->SetOutputOrigin(  likeReader->GetOutput()->GetOrigin() );
	if (m_Verbose)std::cout<<"Resampling output like "<<m_ArgsInfo.like_arg<<"..."<<std::endl;
      }
    else
      {
	// Size
	typename OutputImageType::SizeType outputSize;
	if (m_ArgsInfo.size_given) 
	  {
	    for(unsigned int i=0; i< Dimension; i++)
	      outputSize[i]=m_ArgsInfo.size_arg[i];
	  }
	else outputSize=input->GetLargestPossibleRegion().GetSize();
	if (m_Verbose)std::cout<<"Setting the size to "<<outputSize<<"..."<<std::endl;
	
	// Spacing
	typename OutputImageType::SpacingType outputSpacing;
	if (m_ArgsInfo.spacing_given) 
	  {
	    for(unsigned int i=0; i< Dimension; i++)
	      outputSpacing[i]=m_ArgsInfo.spacing_arg[i];
	  }
	else outputSpacing=input->GetSpacing();
	if (m_Verbose)std::cout<<"Setting the spacing to "<<outputSpacing<<"..."<<std::endl;
    
	// Origin
	typename OutputImageType::PointType outputOrigin;
	if (m_ArgsInfo.origin_given) 
	  {
	    for(unsigned int i=0; i< Dimension; i++)
	      outputOrigin[i]=m_ArgsInfo.origin_arg[i];
	  }
	else outputOrigin=input->GetOrigin();
	if (m_Verbose)std::cout<<"Setting the origin to "<<outputOrigin<<"..."<<std::endl;
    
	// Set
	resampler->SetSize( outputSize );
	resampler->SetOutputSpacing( outputSpacing );
	resampler->SetOutputOrigin(  outputOrigin );

      }

    // Interp : coeff   
    typedef clitk::VectorBSplineResampleImageFunction<InputImageType, double> InterpolatorType;
    typename InterpolatorType::Pointer interpolator=InterpolatorType::New();
    interpolator->SetSplineOrder(m_ArgsInfo.order_arg);


    // Set
    resampler->SetInterpolator( interpolator);
    resampler->SetDefaultPixelValue( static_cast<PixelType>(m_ArgsInfo.pad_arg) );
    resampler->SetInput( input );

    // Update
    try
      {
	resampler->Update();
      }
    catch(itk::ExceptionObject)
      {
	std::cerr<<"Error resampling the image"<<std::endl;
      }
    
    typename OutputImageType::Pointer output = resampler->GetOutput();
    //writeImage<OutputImageType>(output, "/home/jef/tmp/output1.mhd", true);


    // Matrix Transform
    if(m_ArgsInfo.matrix_given)
      {
	typedef itk::TransformToDeformationFieldSource<OutputImageType, double> ConvertorType;
	typename   ConvertorType::Pointer filter= ConvertorType::New();
	filter->SetOutputParametersFromImage(output);
	
	typedef itk::AffineTransform<double, Dimension> TransformType;
	typename TransformType::Pointer transform =TransformType::New();
	itk::Matrix<double, Dimension+1, Dimension+1> homMatrix= ReadMatrix<Dimension>( m_ArgsInfo.matrix_arg);
	itk::Matrix<double, Dimension, Dimension> matrix =GetRotationalPartMatrix( homMatrix);
	itk::Vector<double, Dimension> offset= GetTranslationPartMatrix( homMatrix);
	transform->SetMatrix(matrix);
	transform->SetOffset(offset);
	filter->SetTransform(transform);
	filter->Update();
	typename OutputImageType::Pointer output2=filter->GetOutput();
	//writeImage<OutputImageType>(output2, "/home/jef/tmp/output2.mhd", true);

	// Add 
	typedef itk::AddImageFilter< OutputImageType, OutputImageType, OutputImageType > AddType;
	typename AddType::Pointer adder= AddType::New();
	adder->SetInput(output);
	adder->SetInput(1,output2);
	adder->Update();
	output=adder->GetOutput();
      }


    // Output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(output);
    writer->Update();
  }

}//end clitk
 
#endif //#define clitkBSplineCoefficientsToValuesGenericFilter_txx
