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
#ifndef clitkMultiResolutionPyramidGenericFilter_txx
#define clitkMultiResolutionPyramidGenericFilter_txx

/* =================================================
 * @file   clitkMultiResolutionPyramidGenericFilter.txx
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
  MultiResolutionPyramidGenericFilter::UpdateWithDim(std::string PixelType)
  {
    if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<< PixelType<<"..."<<std::endl;

    if(PixelType == "short"){  
      if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
      UpdateWithDimAndPixelType<Dimension, signed short>(); 
    }
    //    else if(PixelType == "unsigned_short"){  
    //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
    //       UpdateWithDimAndPixelType<Dimension, unsigned short>(); 
    //     }
    
    else if (PixelType == "unsigned_char"){ 
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


  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template <unsigned int Dimension, class  PixelType> 
  void 
  MultiResolutionPyramidGenericFilter::UpdateWithDimAndPixelType()
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
    typedef itk::ImageToImageFilter<InputImageType, OutputImageType> FilterType;
    typename FilterType::Pointer filter;
    switch(m_ArgsInfo.type_arg)
      {
      case 0: 
	{
	  typedef itk::MultiResolutionPyramidImageFilter<InputImageType, OutputImageType> MRFilterType;
	  typename MRFilterType::Pointer rmFilter =MRFilterType::New();
	  if (m_Verbose) std::cout<<"Using the multi-resolution filter..."<<std::endl;
	  rmFilter->SetNumberOfLevels(m_ArgsInfo.levels_arg); 
	  filter=rmFilter;
	  break;
	}
      case 1: 
	{
	  typedef itk::RecursiveMultiResolutionPyramidImageFilter<InputImageType, OutputImageType> RecursiveFilterType;
	  typename RecursiveFilterType::Pointer rFilter=RecursiveFilterType::New();
	  if (m_Verbose) std::cout<<"Using the recursive multi-resolution filter..."<<std::endl;
	  rFilter->SetNumberOfLevels(m_ArgsInfo.levels_arg); 
	  rFilter->SetUseShrinkImageFilter(false);
	  filter=rFilter;
	  break;
	}
      case 2: 
	{
	  typedef clitk::SpatioTemporalMultiResolutionPyramidImageFilter<InputImageType, OutputImageType> SpatioTemporalFilterType;
	  typename SpatioTemporalFilterType::Pointer spFilter=SpatioTemporalFilterType::New();
	  if (m_Verbose) std::cout<<"Using the spatio-temporal multi-resolution filter..."<<std::endl;
	  spFilter->SetNumberOfLevels(m_ArgsInfo.levels_arg); 
	  filter=spFilter;
	  break;
	}
    case 3: 
	{
	  typedef clitk::RecursiveSpatioTemporalMultiResolutionPyramidImageFilter<InputImageType, OutputImageType> RecursiveSpatioTemporalFilterType;
	  typename RecursiveSpatioTemporalFilterType::Pointer rspFilter=RecursiveSpatioTemporalFilterType::New();
	  if (m_Verbose) std::cout<<"Using the recursive spatio-temporal multi-resolution filter..."<<std::endl;
	  rspFilter->SetNumberOfLevels(m_ArgsInfo.levels_arg); 
	  rspFilter->SetUseShrinkImageFilter(false);
	  filter=rspFilter;
	  break;
	}
      }

    // Common
    filter->SetInput(input);  
    try
      {
	filter->Update();
      }
    catch (itk::ExceptionObject)
      {
	std::cerr<<"Exception thrown during update() of the multi-resolution pyramid filter!"<<std::endl; 
      }    

    // Output
    for(unsigned int i=0; i< m_ArgsInfo.output_given;i++)
      {
	// Get image at level i
	typename OutputImageType::Pointer output=filter->GetOutput(i);
		
	// Write 
	writeImage<OutputImageType>(output,m_ArgsInfo.output_arg[i], m_Verbose);
// 	typedef itk::ImageFileWriter<OutputImageType> WriterType;
// 	typename WriterType::Pointer writer = WriterType::New();
// 	writer->SetFileName(m_ArgsInfo.output_arg[i]);
// 	writer->SetInput(output);
// 	writer->Update();
      }
  }


}//end clitk
 
#endif //#define clitkMultiResolutionPyramidGenericFilter_txx
