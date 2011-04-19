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
#ifndef clitkFillImageRegionGenericFilter_txx
#define clitkFillImageRegionGenericFilter_txx

/* =================================================
 * @file   clitkFillImageRegionGenericFilter.txx
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
  FillImageRegionGenericFilter::UpdateWithDim(std::string PixelType)
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
  FillImageRegionGenericFilter::UpdateWithDimAndPixelType()
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

    // Processing
    switch(m_ArgsInfo.shape_arg)
      {
	//rectangular
      case 0:
	{
	  // Get Size  and index of the region
	  typename InputImageType::SizeType size;
	  if (m_ArgsInfo.size_given)
	    for(unsigned int i=0; i<Dimension; i++)
	      size[i]=m_ArgsInfo.size_arg[i];
	  else 
	    size.Fill(m_ArgsInfo.size_arg[0]);

	  // Get Index of the region
	  typename InputImageType::IndexType index;
	  if (m_ArgsInfo.index_given)
	    for(unsigned int i=0; i<Dimension; i++)
	      index[i]=m_ArgsInfo.index_arg[i];
	  else 
	    index.Fill(m_ArgsInfo.index_arg[0]);

	  // Set the region
	  typename InputImageType::RegionType region;
	  region.SetIndex(index);
	  region.SetSize(size);

	  // Iterator
	  typedef itk::ImageRegionIterator<InputImageType> IteratorType;
	  IteratorType it(input, region);
	  it.GoToBegin();
	  while (!it.IsAtEnd()) {
	    it.Set(m_ArgsInfo.value_arg);
	    ++it;
	  }
	  
	  break;
	}
	
	//ellipsoide
      case 1:
	{

	  //Get the center
	  typename InputImageType::PointType center;
	  if (m_ArgsInfo.center_given)
	    for(unsigned int i=0; i<Dimension; i++)
	      center[i]=m_ArgsInfo.center_arg[i];
	  else
	    {
	      typename InputImageType::SizeType size= input->GetLargestPossibleRegion().GetSize();
	      typename InputImageType::SpacingType spacing= input->GetSpacing();
	      typename InputImageType::PointType origin= input->GetOrigin();
	      for (unsigned int i=0; i<Dimension; i++)
		center[i]=origin[i]+(double)size[i]/2*spacing[i];
	    }
	  if (m_ArgsInfo.offset_given)
	    {
	      typename itk::Vector<double, Dimension> offset;
	      for (unsigned int i=0; i<Dimension; i++)
		  center[i]+=m_ArgsInfo.offset_arg[i];
	    }


	  // Get the half axes size
	  typename itk::Vector<double, Dimension> axes;
	  if (m_ArgsInfo.axes_given)
	    for(unsigned int i=0; i<Dimension; i++)
	      axes[i]=m_ArgsInfo.axes_arg[i];
	  else
	    axes.Fill(m_ArgsInfo.axes_arg[0]);
	  
	  
	  // Build iterator
	  typedef itk::ImageRegionIteratorWithIndex<InputImageType> IteratorType;
	  IteratorType it(input, input->GetLargestPossibleRegion());
	  it.GoToBegin();

	  typename InputImageType::PointType point; 
	  typename InputImageType::IndexType index;
	  double distance;
  
	  while (!it.IsAtEnd())
	    {    
	      index=it.GetIndex();
	      input->TransformIndexToPhysicalPoint(index, point);
	      distance=0.0;      
	      for(unsigned int i=0; i<Dimension; i++)
		distance+=powf( ( (center[i]-point[i])/axes[i] ), 2);
	      
	      if (distance<1)
		it.Set(m_ArgsInfo.value_arg);
	      ++it;
	    }
	  break;
	}
      }
    
    // Output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(input);
    writer->Update();
    
  }
  
  
}//end clitk
 
#endif //#define clitkFillImageRegionGenericFilter_txx
