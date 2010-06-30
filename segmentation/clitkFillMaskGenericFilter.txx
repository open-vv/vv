#ifndef clitkFillMaskGenericFilter_txx
#define clitkFillMaskGenericFilter_txx

/* =================================================
 * @file   clitkFillMaskGenericFilter.txx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


namespace clitk
{

  //-------------------------------------------------------------------
  // Update with the pixeltype
  //-------------------------------------------------------------------
  template <class  PixelType> 
  void 
  FillMaskGenericFilter::UpdateWithPixelType()
  {
    // Dim & Pix
    const unsigned int Dimension=3;
    typedef int InternalPixelType;

    // ImageTypes
    typedef itk::Image<PixelType, Dimension> InputImageType;
    typedef itk::Image<InternalPixelType, Dimension> InternalImageType;
    typedef itk::Image<PixelType, Dimension> OutputImageType;
    
    // Read the input
    typedef itk::ImageFileReader<InputImageType> InputReaderType;
    typename InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName( m_InputFileName);
    reader->Update();
    typename InputImageType::Pointer input= reader->GetOutput();
  
    // Read the directions over which to fill holes
    std::vector<unsigned int> direction;
    if (m_ArgsInfo.dir_given)
      for ( unsigned int i=0;i<m_ArgsInfo.dir_given;i++)
	direction.push_back(m_ArgsInfo.dir_arg[i]);
    else
      for ( unsigned int i=0;i<Dimension;i++)
	direction.push_back(Dimension-i-1);

    //---------------------------------------- 
    // Cast to internal type    
    //----------------------------------------  
    typedef itk::CastImageFilter<InputImageType,InternalImageType> InputCastImageFilterType;
    typename InputCastImageFilterType::Pointer inputCaster= InputCastImageFilterType::New();
    inputCaster->SetInput(input);
    inputCaster->Update();

    //---------------------------------------- 
    // Loop over directions    
    //---------------------------------------- 
    typename InternalImageType::Pointer output=inputCaster->GetOutput();
    for (unsigned int i=0; i<direction.size();i++)
      {
	
	//---------------------------------------- 
	// Fill the holes of a mask in 2D
	//----------------------------------------
	if(m_Verbose) std::cout<<"Fill holes in the mask slice by slice in direction "<<direction[i]<<"..."<<std::endl;

	// We define the region to be extracted.
	typedef  itk::Image<InternalPixelType, Dimension-1> ImageSliceType;
	typedef  itk::Image<InternalPixelType, Dimension-1> MaskSliceType;
	typename InternalImageType::RegionType region3D= input->GetLargestPossibleRegion();
	typename InternalImageType::RegionType::SizeType size3D= region3D.GetSize();
	typename InternalImageType::RegionType::SizeType size2D=size3D;
	size2D[direction[i]]=0;
	typename InternalImageType::IndexType start2D; 
	start2D.Fill(0);
	typename InternalImageType::RegionType desiredRegion;
	desiredRegion.SetSize( size2D );
	desiredRegion.SetIndex( start2D );
      
	// Extract and Join 
	typedef itk::ExtractImageFilter<InternalImageType, ImageSliceType> ExtractImageFilterType;
	typedef itk::JoinSeriesImageFilter<ImageSliceType, InternalImageType> JoinSeriesFilterType;
	typename JoinSeriesFilterType::Pointer joinFilter=JoinSeriesFilterType::New();
	joinFilter->SetSpacing(input->GetSpacing()[direction[i]]);
      
	//---------------------------------------- 
	// Run over the sliceIndexs
	// ----------------------------------------
	for(unsigned int sliceIndex=0; sliceIndex <size3D[direction[i]]; sliceIndex++)
	  {
	    //---------------------------------------- 
	    // Extract mask sliceIndex
	    //----------------------------------------
	    typename ExtractImageFilterType::Pointer extractFilter=ExtractImageFilterType::New();
	    extractFilter->SetInput(output);
	    start2D[direction[i]]=sliceIndex;
	    desiredRegion.SetIndex( start2D );
	    extractFilter->SetExtractionRegion( desiredRegion );
	    extractFilter->Update( );
	    typename ImageSliceType::Pointer slice= extractFilter->GetOutput();

	    // Binarize the image (Before: OBJECT!=0, rest=0, After: object=1, rest=0 )
	    typedef itk::BinaryThresholdImageFilter<ImageSliceType,ImageSliceType> BinarizeFilterType;
	    typename BinarizeFilterType::Pointer binarizeFilter=BinarizeFilterType::New();
	    binarizeFilter->SetInput(slice);
	    binarizeFilter->SetUpperThreshold(0);
	    binarizeFilter->SetOutsideValue(0);
	    binarizeFilter->SetInsideValue(1);
	    // writeImage<ImageSliceType>(binarizeFilter->GetOutput(),"/home/jef/tmp/input.mhd");

	    // Perform connected labelling on the slice (body+air=0 )
	    typedef itk::ConnectedComponentImageFilter<ImageSliceType, ImageSliceType> ConnectFilterType;
	    typename ConnectFilterType::Pointer connectFilter=ConnectFilterType::New();
	    connectFilter->SetInput(binarizeFilter->GetOutput());
	    connectFilter->SetBackgroundValue(0);
	    connectFilter->SetFullyConnected(false);
	    //connectFilter->Update();
	    //writeImage<ImageSliceType>(connectFilter->GetOutput(),"/home/jef/tmp/connect.mhd");
	    	  
	    // Sort the labels
	    typedef itk::RelabelComponentImageFilter<ImageSliceType, ImageSliceType> RelabelFilterType;
	    typename RelabelFilterType::Pointer relabelFilter=RelabelFilterType::New();
	    relabelFilter->SetInput(connectFilter->GetOutput());
	    //relabelFilter->Update();
	    //writeImage<ImageSliceType>(relabelFilter->GetOutput(),"/home/jef/tmp/label.mhd");	
	    
	    // Keep the first
	    typedef itk::ThresholdImageFilter<ImageSliceType> ThresholdFilterType;
	    typename ThresholdFilterType::Pointer thresholdFilter=ThresholdFilterType::New();
	    thresholdFilter->SetInput(relabelFilter->GetOutput());
	    thresholdFilter->SetUpper(1);
	    thresholdFilter->SetOutsideValue(0);
	    // thresholdFilter->Update();
	    // writeImage<ImageSliceType>(thresholdFilter->GetOutput(),"/home/jef/tmp/bin.mhd");	

	    // Invert the labels (lung 1, rest 0)
	    typename BinarizeFilterType::Pointer switchFilter=BinarizeFilterType::New();
	    switchFilter->SetInput(thresholdFilter->GetOutput());
	    switchFilter->SetUpperThreshold(0);
	    switchFilter->SetOutsideValue(0);
	    switchFilter->SetInsideValue(1);
	    switchFilter->Update();
	    //writeImage<ImageSliceType>(switchFilter->GetOutput(),"/home/jef/tmp/inv_bin.mhd");	
	    
	    //Join
	    joinFilter->SetInput( sliceIndex, switchFilter->GetOutput());
	  }
	
	// Join to a 3D image	
	if (m_Verbose) std::cout<<"Joining the slices..."<<std::endl;
	joinFilter->Update();
	
	// Permute the axes to reset to orientation
	typedef itk::PermuteAxesImageFilter<InternalImageType> PermuteFilterType;
	typename PermuteFilterType::Pointer permuteFilter=PermuteFilterType::New();
	permuteFilter->SetInput(joinFilter->GetOutput());
	typename PermuteFilterType::PermuteOrderArrayType order;
	order[direction[i]]=2;
	if( direction[i]==2)
	  {
	    order[0]=0;
	    order[1]=1;
	  }
	else if ( direction[i]==1)
	  {
	    order[0]=0;
	    order[2]=1;
	  }
	else if (direction[i]==0)
	  {
	    order[1]=0;
	    order[2]=1;
	  }
	permuteFilter->SetOrder(order);
	permuteFilter->Update();
	output =permuteFilter->GetOutput();
	
	// Set the image direction to the input one
	output->SetDirection(input->GetDirection());
	output->SetOrigin(input->GetOrigin());
      }


    // Cast
    typedef itk::CastImageFilter<InternalImageType,OutputImageType> OutputCastImageFilterType;
    typename OutputCastImageFilterType::Pointer outputCaster =OutputCastImageFilterType::New();
    outputCaster->SetInput(output);
            
    // Output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(outputCaster->GetOutput());
    writer->Update();
  }

}//end clitk

#endif //#define clitkFillMaskGenericFilter_txx
