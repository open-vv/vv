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
#ifndef clitkImageStatisticsGenericFilter_txx
#define clitkImageStatisticsGenericFilter_txx

/* =================================================
 * @file   clitkImageStatisticsGenericFilter.txx
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
  ImageStatisticsGenericFilter::UpdateWithDim(std::string PixelType)
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
  ImageStatisticsGenericFilter::UpdateWithDimAndPixelType()
  {

    // ImageTypes
    typedef itk::Image<PixelType, Dimension> InputImageType;
    typedef itk::Image<unsigned int, Dimension> LabelImageType;
    typedef itk::Image<PixelType, Dimension> OutputImageType;
    
    // Read the input
    typedef itk::ImageFileReader<InputImageType> InputReaderType;
    typename InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName( m_InputFileName);
    reader->Update();
    typename InputImageType::Pointer input= reader->GetOutput();

    // Filter
    typedef itk::LabelStatisticsImageFilter<InputImageType, LabelImageType> StatisticsImageFilterType;
    typename StatisticsImageFilterType::Pointer statisticsFilter=StatisticsImageFilterType::New();
    statisticsFilter->SetInput(input);

    // Label image
    typename LabelImageType::Pointer labelImage;
    if (m_ArgsInfo.mask_given)
      {
	typedef itk::ImageFileReader<LabelImageType> LabelImageReaderType;
	typename LabelImageReaderType::Pointer labelImageReader=LabelImageReaderType::New();
	labelImageReader->SetFileName(m_ArgsInfo.mask_arg);
	labelImageReader->Update();
	labelImage= labelImageReader->GetOutput();
      }
    else
      { 
	labelImage=LabelImageType::New();
	labelImage->SetRegions(input->GetLargestPossibleRegion());
	labelImage->SetOrigin(input->GetOrigin());
	labelImage->SetSpacing(input->GetSpacing());
	labelImage->Allocate();
	labelImage->FillBuffer(m_ArgsInfo.label_arg[0]);
      }
    statisticsFilter->SetLabelInput(labelImage);

    // For each Label
    typename LabelImageType::PixelType label;
    unsigned int numberOfLabels;
    if (m_ArgsInfo.label_given)
      numberOfLabels=m_ArgsInfo.label_given;
    else
      numberOfLabels=1;

    for (unsigned int k=0; k< numberOfLabels; k++)
      {
	label=m_ArgsInfo.label_arg[k];

	std::cout<<std::endl;
	if (m_Verbose) std::cout<<"-------------"<<std::endl;
	if (m_Verbose) std::cout<<"| Label: "<<label<<"  |"<<std::endl;
	if (m_Verbose) std::cout<<"-------------"<<std::endl;
	
	// Histograms
	if (m_ArgsInfo.histogram_given)
	  {
	    statisticsFilter->SetUseHistograms(true);
	    statisticsFilter->SetHistogramParameters(m_ArgsInfo.bins_arg, m_ArgsInfo.lower_arg, m_ArgsInfo.upper_arg);
	  }
	statisticsFilter->Update();


	// Output
	if (m_Verbose) std::cout<<"N° of pixels: ";
	std::cout<<statisticsFilter->GetCount(label)<<std::endl;

	if (m_Verbose) std::cout<<"Mean: ";
	std::cout<<statisticsFilter->GetMean(label)<<std::endl;
    
	if (m_Verbose) std::cout<<"SD: ";
	std::cout<<statisticsFilter->GetSigma(label)<<std::endl;

	if (m_Verbose) std::cout<<"Variance: ";
	std::cout<<statisticsFilter->GetVariance(label)<<std::endl;

	if (m_Verbose) std::cout<<"Min: ";
	std::cout<<statisticsFilter->GetMinimum(label)<<std::endl;

	if (m_Verbose) std::cout<<"Max: ";
	std::cout<<statisticsFilter->GetMaximum(label)<<std::endl;

	if (m_Verbose) std::cout<<"Sum: ";
	std::cout<<statisticsFilter->GetSum(label)<<std::endl;

	if (m_Verbose) std::cout<<"Bounding box: ";
	for(unsigned int i =0; i <statisticsFilter->GetBoundingBox(label).size(); i++)
	  std::cout<<statisticsFilter->GetBoundingBox(label)[i]<<" ";
	std::cout<<std::endl;


	// Histogram
	if (m_ArgsInfo.histogram_given)
	  {
	    if (m_Verbose) std::cout<<"Median: ";
	    std::cout<<statisticsFilter->GetMedian(label)<<std::endl;

	    typename StatisticsImageFilterType::HistogramPointer histogram =statisticsFilter->GetHistogram(label);
	    
	    // Screen
	    if (m_Verbose) std::cout<<"Histogram: "<<std::endl;
	    std::cout<<"# MinBin\tMidBin\tMaxBin\tFrequency"<<std::endl;
	    for( int i =0; i <m_ArgsInfo.bins_arg; i++)
	      std::cout<<histogram->GetBinMin(0,i)<<"\t"<<histogram->GetMeasurement(i,0)<<"\t"<<histogram->GetBinMax(0,i)<<"\t"<<histogram->GetFrequency(i)<<std::endl;
	  
	    // Add to the file
	    std::ofstream histogramFile(m_ArgsInfo.histogram_arg);
	    histogramFile<<"#Histogram: "<<std::endl;
	    histogramFile<<"#MinBin\tMidBin\tMaxBin\tFrequency"<<std::endl;
	    for( int i =0; i <m_ArgsInfo.bins_arg; i++)
	      histogramFile<<histogram->GetBinMin(0,i)<<"\t"<<histogram->GetMeasurement(i,0)<<"\t"<<histogram->GetBinMax(0,i)<<"\t"<<histogram->GetFrequency(i)<<std::endl;
	  }
      }
    
    return;
    
  }
  
  
}//end clitk

#endif //#define clitkImageStatisticsGenericFilter_txx
