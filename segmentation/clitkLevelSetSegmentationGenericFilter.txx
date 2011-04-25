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
#ifndef clitkLevelSetSegmentationGenericFilter_txx
#define clitkLevelSetSegmentationGenericFilter_txx

/* =================================================
 * @file   clitkLevelSetSegmentationGenericFilter.txx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
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
  LevelSetSegmentationGenericFilter::UpdateWithDim(std::string PixelType)
  {
    if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<< PixelType<<"..."<<std::endl;

    //     if(PixelType == "short"){  
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
    //else {
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, float>();
    // }
  }


  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template <unsigned int Dimension, class  PixelType> 
  void 
  LevelSetSegmentationGenericFilter::UpdateWithDimAndPixelType()
  {

    // ImageTypes
    typedef itk::Image<float, Dimension> InputImageType;
    typedef itk::Image<float, Dimension> FeatureImageType;
    typedef itk::Image<float, Dimension> OutputImageType;
    
    // Read the input (initial level set)
    typedef itk::ImageFileReader<InputImageType> InputReaderType;
    typename InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName( m_ArgsInfo.input_arg);
    typename InputImageType::Pointer input= reader->GetOutput();
    
    // Feature image
    typename FeatureImageType::Pointer featureImage;
    if( m_ArgsInfo.feature_given)
      {
	// Read it
	typedef itk::ImageFileReader<FeatureImageType> FeatureReaderType;
	typename FeatureReaderType::Pointer featureReader = FeatureReaderType::New();
	featureReader->SetFileName(m_ArgsInfo.feature_arg);
	featureReader->Update();
	featureImage=featureReader->GetOutput();
	
	// Edge preserving smoothing
	if (m_ArgsInfo.smooth_flag)
	  {
	    typedef   itk::CurvatureAnisotropicDiffusionImageFilter<FeatureImageType, FeatureImageType >  SmoothingFilterType;
	    typename SmoothingFilterType::Pointer smoothingFilter = SmoothingFilterType::New();
	    smoothingFilter->SetInput(featureImage);
	    smoothingFilter->SetTimeStep( m_ArgsInfo.timeStep_arg );
	    smoothingFilter->SetNumberOfIterations( m_ArgsInfo.iterSmooth_arg );
	    smoothingFilter->SetConductanceParameter( m_ArgsInfo.cond_arg );
	    smoothingFilter->Update();
	    featureImage=smoothingFilter->GetOutput();
	    
	  }

	// Recursive gaussian gradient magnitude
	if (m_ArgsInfo.gradMag_flag)
	  {
	    typedef   itk::GradientMagnitudeImageFilter< FeatureImageType,FeatureImageType>  GradientFilterType;
	    typename GradientFilterType::Pointer  gradientMagnitude = GradientFilterType::New();
	    gradientMagnitude->SetInput(featureImage);
	    gradientMagnitude->Update();
	    featureImage=gradientMagnitude->GetOutput();
	    
	  }
	
	// Recursive gaussian gradient magnitude
	if (m_ArgsInfo.gradMagGauss_flag)
	  {
	    typedef   itk::GradientMagnitudeRecursiveGaussianImageFilter< FeatureImageType,FeatureImageType>  GradientFilterType;
	    typename GradientFilterType::Pointer  gradientMagnitude = GradientFilterType::New();
	    gradientMagnitude->SetInput(featureImage);
	    gradientMagnitude->SetSigma( m_ArgsInfo.sigma_arg  );
	    gradientMagnitude->Update();
	    featureImage=gradientMagnitude->GetOutput();
	  }
	
	// Sigmoid 
	if (m_ArgsInfo.sigmoid_flag)
	  {
	    typedef   itk::SigmoidImageFilter<FeatureImageType, FeatureImageType >  SigmoidFilterType;
	    typename SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();
	    sigmoid->SetInput(featureImage);
	    sigmoid->SetAlpha( m_ArgsInfo.alpha_arg );
	    sigmoid->SetBeta(  m_ArgsInfo.beta_arg  );
	    sigmoid->Update();
	    featureImage=sigmoid->GetOutput();
	  }

      }

    // Filter
    typename FeatureImageType::Pointer output;
    if (m_ArgsInfo.GAC_flag)
      {

	// Create the filter
	typedef  itk::GeodesicActiveContourLevelSetImageFilter< InputImageType, FeatureImageType >    GeodesicActiveContourFilterType;
	typename GeodesicActiveContourFilterType::Pointer geodesicActiveContour = GeodesicActiveContourFilterType::New();
	
	geodesicActiveContour->SetPropagationScaling( m_ArgsInfo.propScale_arg );
	geodesicActiveContour->SetCurvatureScaling( m_ArgsInfo.curveScale_arg );
	geodesicActiveContour->SetAdvectionScaling( m_ArgsInfo.advectionScale_arg );
	geodesicActiveContour->SetMaximumRMSError( m_ArgsInfo.maxRMS_arg );
	geodesicActiveContour->SetInput(  input );
	geodesicActiveContour->SetFeatureImage( featureImage );
	geodesicActiveContour->SetUseImageSpacing(true);

	// Monitor
	unsigned int totalNumberOfIterations=0;
	if(m_ArgsInfo.monitorIm_given)
	  {
	    geodesicActiveContour->SetNumberOfIterations( m_ArgsInfo.monitorIt_arg );
	    while (true)
	      {
		geodesicActiveContour->Update();
		totalNumberOfIterations+=geodesicActiveContour->GetElapsedIterations();
		if(m_Verbose) std::cout <<"Writing image after "<< totalNumberOfIterations<<"..."<<std::endl;
		writeImage<InputImageType>(geodesicActiveContour->GetOutput(), m_ArgsInfo.monitorIm_arg);
		geodesicActiveContour->SetInput(geodesicActiveContour->GetOutput());
		geodesicActiveContour->SetNumberOfIterations(  std::min( (m_ArgsInfo.iter_arg-totalNumberOfIterations) ,(unsigned int) m_ArgsInfo.monitorIt_arg ) ); 
		if (totalNumberOfIterations> (unsigned int) m_ArgsInfo.iter_arg) break;
	      }
	  }
	else
	  {
	    geodesicActiveContour->SetNumberOfIterations( m_ArgsInfo.iter_arg );
	    geodesicActiveContour->Update();
	    totalNumberOfIterations=geodesicActiveContour->GetElapsedIterations();
	  }


	// Print
	std::cout << std::endl;
	std::cout << "Max. no. iterations: " << m_ArgsInfo.iter_arg << std::endl;
	std::cout << "Max. RMS error: " << geodesicActiveContour->GetMaximumRMSError() << std::endl;
	std::cout << std::endl;
	std::cout << "No. elpased iterations: " << totalNumberOfIterations << std::endl;
	std::cout << "RMS change: " << geodesicActiveContour->GetRMSChange() << std::endl;
	
	output = geodesicActiveContour->GetOutput();
      }
	
    // Write levelset
    if (m_ArgsInfo.levelSet_given)
      {
	typedef itk::ImageFileWriter<FeatureImageType> WriterType;
	typename WriterType::Pointer writer = WriterType::New();
	writer->SetFileName(m_ArgsInfo.levelSet_arg);
	writer->SetInput(output);
	writer->Update();
      }

    // Threshold
    typedef itk::BinaryThresholdImageFilter< FeatureImageType,FeatureImageType    >    ThresholdingFilterType;
    typename ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
    thresholder->SetLowerThreshold( -1000.0 );
    thresholder->SetUpperThreshold(     0.0 );
    thresholder->SetOutsideValue(  0  );
    thresholder->SetInsideValue(  1 );
    thresholder->SetInput( output );
    thresholder->Update();
    output=thresholder->GetOutput();
     
    // Output
    typedef itk::ImageFileWriter<FeatureImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(output);
    writer->Update();

  }


}//end clitk
 
#endif //#define clitkLevelSetSegmentationGenericFilter_txx
