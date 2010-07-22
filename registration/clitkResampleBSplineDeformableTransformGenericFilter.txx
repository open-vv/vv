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
#ifndef clitkResampleBSplineDeformableTransformGenericFilter_txx
#define clitkResampleBSplineDeformableTransformGenericFilter_txx

/* =================================================
 * @file   clitkResampleBSplineDeformableTransformGenericFilter.txx
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
  ResampleBSplineDeformableTransformGenericFilter::UpdateWithDim(std::string PixelType, int Components)
  {
    if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<<Components<<" component(s) of "<<  PixelType<<"..."<<std::endl;

    if (Components==2)
      {
	if(PixelType == "double"){  
	  if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and 2D double..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, itk::Vector<double,2 > >(); 
	}
	else {
	  if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and 2D float..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, itk::Vector<float,2 > >();
	}
      }
    else if (Components==3)
      {
	if(PixelType == "double"){  
	  if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and 3D double..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, itk::Vector<double,3 > >(); 
	}
	else {
	  if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and 3D float..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, itk::Vector<float,3 > >();
	}
      }
    else std::cerr<<"Number of components is "<<Components<<", not supported!"<<std::endl;
  }


  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template <unsigned int Dimension, class  PixelType> 
  void 
  ResampleBSplineDeformableTransformGenericFilter::UpdateWithDimAndPixelType()
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

    // Filter
    typedef clitk::ResampleBSplineDeformableTransformImageFilter<InputImageType,OutputImageType> ResampleFilterType;
    typename   ResampleFilterType::Pointer filter=ResampleFilterType::New();
    filter->SetInput(input);
    
    // Output image info
    if (m_ArgsInfo.like_given)
      {
	typename InputReaderType::Pointer likeReader=InputReaderType::New();
	likeReader->SetFileName(m_ArgsInfo.like_arg);
	likeReader->Update();
	filter->SetOutputParametersFromImage(likeReader->GetOutput());
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
	if (m_Verbose) std::cout<<"Setting the size to "<<outputSize<<"..."<<std::endl;
	
	// Spacing
	typename OutputImageType::SpacingType outputSpacing;
	if (m_ArgsInfo.spacing_given) 
	  {
	    for(unsigned int i=0; i< Dimension; i++)
	      outputSpacing[i]=m_ArgsInfo.spacing_arg[i];
	  }
	else outputSpacing=input->GetSpacing();
	if (m_Verbose) std::cout<<"Setting the spacing to "<<outputSpacing<<"..."<<std::endl;
    
	// Origin
	typename OutputImageType::PointType outputOrigin;
	if (m_ArgsInfo.origin_given) 
	  {
	    for(unsigned int i=0; i< Dimension; i++)
	      outputOrigin[i]=m_ArgsInfo.origin_arg[i];
	  }
	else outputOrigin=input->GetOrigin();
	if (m_Verbose) std::cout<<"Setting the origin to "<<outputOrigin<<"..."<<std::endl;

	// Set
	filter->SetSize( outputSize );
	filter->SetOutputSpacing( outputSpacing );
	filter->SetOutputOrigin(  outputOrigin );
	
      }

    // Go
    filter->Update();
    
    // Get the output
    typename OutputImageType::Pointer output=filter->GetOutput();
    
    // Output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(output);
    writer->Update();

  }


}//end clitk
 
#endif //#define clitkResampleBSplineDeformableTransformGenericFilter_txx
