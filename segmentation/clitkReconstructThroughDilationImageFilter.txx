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
#ifndef clitkReconstructThroughDilationImageFilter_txx
#define clitkReconstructThroughDilationImageFilter_txx

namespace clitk
{

  //-------------------------------------------------------------------
  // Update with the number of dimensions
  //-------------------------------------------------------------------
  template<class InputImageType, class OutputImageType>
  ReconstructThroughDilationImageFilter<InputImageType, OutputImageType>::ReconstructThroughDilationImageFilter()
  {
   m_Verbose=false;
   m_BackgroundValue=0;
   m_ForegroundValue=1;
   m_ErosionPaddingValue=static_cast<InputPixelType>(-1);
   for (unsigned int i=0; i<InputImageDimension; i++)
     m_Radius[i]=1;
   m_MaximumNumberOfLabels=10;
  }


  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template<class InputImageType, class  OutputImageType> 
  void 
  ReconstructThroughDilationImageFilter<InputImageType, OutputImageType>::GenerateData()
  {

    //---------------------------------
    // Typedefs 
    //--------------------------------- 
    
    // Internal type
    typedef itk::Image<InternalPixelType, InputImageDimension> InternalImageType;

    // Filters used
    typedef itk::CastImageFilter<InputImageType, InternalImageType> InputCastImageFilterType;   
    typedef itk::ThresholdImageFilter<InternalImageType> InputThresholdImageFilterType;
    typedef itk::StatisticsImageFilter<InternalImageType> StatisticsImageFilterType;
    typedef itk::BinaryBallStructuringElement<InternalPixelType,InputImageDimension > KernelType;
    typedef clitk::ConditionalBinaryDilateImageFilter<InternalImageType, InternalImageType , KernelType> ConditionalBinaryDilateImageFilterType;
    typedef itk::DifferenceImageFilter<InternalImageType, InternalImageType> DifferenceImageFilterType;
    typedef itk::CastImageFilter<InternalImageType, OutputImageType> OutputCastImageFilterType;
    typedef clitk::SetBackgroundImageFilter<InternalImageType, InternalImageType, InternalImageType> SetBackgroundImageFilterType;

    //---------------------------------
    // Cast
    //---------------------------------
    typename InputCastImageFilterType::Pointer castImageFilter=InputCastImageFilterType::New();
    castImageFilter->SetInput(this->GetInput());
    castImageFilter->Update();

    //---------------------------------
    // Threshold
    //---------------------------------
    typename InputThresholdImageFilterType::Pointer thresholdImageFilter=InputThresholdImageFilterType::New();
    thresholdImageFilter->SetInput(castImageFilter->GetOutput());
    thresholdImageFilter->ThresholdAbove(m_MaximumNumberOfLabels);
    thresholdImageFilter->SetOutsideValue(m_ForegroundValue);
    if(m_Verbose) std::cout<<"Thresholding the input to "<<m_MaximumNumberOfLabels<<" labels ..."<<std::endl;
    thresholdImageFilter->Update();

    //---------------------------------
    // Set -1 to padding value
    //---------------------------------
    typename SetBackgroundImageFilterType::Pointer setBackgroundFilter =SetBackgroundImageFilterType::New();
    setBackgroundFilter->SetInput(thresholdImageFilter->GetOutput());
    setBackgroundFilter->SetInput2(castImageFilter->GetOutput());
    setBackgroundFilter->SetMaskValue(m_ErosionPaddingValue);
    setBackgroundFilter->SetOutsideValue(-1);
    if(m_Verbose) std::cout<<"Setting the eroded region from "<<m_ErosionPaddingValue<<" to -1..."<<std::endl;

    
    //---------------------------------
    // Count the initial labels
    //---------------------------------
    typename StatisticsImageFilterType::Pointer inputStatisticsImageFilter=StatisticsImageFilterType::New();
    inputStatisticsImageFilter->SetInput(setBackgroundFilter->GetOutput());
    if(m_Verbose) std::cout<<"Counting the initial labels..."<<std::endl;
    inputStatisticsImageFilter->Update();
    unsigned int initialNumberOfLabels= inputStatisticsImageFilter->GetMaximum();
    if(m_Verbose) std::cout<<"The input contained "<<initialNumberOfLabels<<" disctictive label(s)..."<<std::endl;
    unsigned int numberOfConsideredLabels=std::min(initialNumberOfLabels, m_MaximumNumberOfLabels);
    if(m_Verbose) std::cout<<"Performing dilation the first "<<numberOfConsideredLabels<<" disctictive labels..."<<std::endl;

    //---------------------------------
    // Dilate while change
    //---------------------------------
    typename itk::NumericTraits<InputPixelType>::AccumulateType difference=1;
    typename InternalImageType::Pointer labelImage=inputStatisticsImageFilter->GetOutput();
    typename InternalImageType::Pointer oldLabelImage=inputStatisticsImageFilter->GetOutput();

    // element
    KernelType structuringElement;
    structuringElement.SetRadius(m_Radius);
    structuringElement.CreateStructuringElement();

    while( difference)
      {
	// Dilate all labels once
	for ( int label=0; label<(int)numberOfConsideredLabels+1;label++)  
	  if ( m_BackgroundValue != label)
	    {
	      typename ConditionalBinaryDilateImageFilterType::Pointer dilateFilter=ConditionalBinaryDilateImageFilterType::New();
	      dilateFilter->SetBoundaryToForeground(false);
	      dilateFilter->SetKernel(structuringElement);
	      dilateFilter->SetBackgroundValue (-1);
	      dilateFilter->SetInput (labelImage);
	      dilateFilter->SetForegroundValue (label);
	      if(m_Verbose) std::cout<<"Dilating the label "<<label<<"..."<<std::endl;
	      dilateFilter->Update();
	      labelImage=dilateFilter->GetOutput();
	    }
  
	// Difference with previous labelImage
	typename DifferenceImageFilterType::Pointer differenceFilter=DifferenceImageFilterType::New();
	differenceFilter->SetValidInput(oldLabelImage);
	differenceFilter->SetTestInput(labelImage);
	differenceFilter->Update();
	difference =differenceFilter->GetTotalDifference();
	if(m_Verbose) std::cout<<"The change in this iteration was "<<difference<<"..."<<std::endl;     
	oldLabelImage=labelImage;
      }
      
    //---------------------------------
    // Set -1 to padding value
    //---------------------------------
    typename SetBackgroundImageFilterType::Pointer setBackgroundFilter2 =SetBackgroundImageFilterType::New();
    setBackgroundFilter2->SetInput(labelImage);
    setBackgroundFilter2->SetInput2(labelImage);
    setBackgroundFilter2->SetMaskValue(-1);
    setBackgroundFilter2->SetOutsideValue(m_ErosionPaddingValue);
    if(m_Verbose) std::cout<<"Setting the eroded region to "<<m_ErosionPaddingValue<<"..."<<std::endl;

    //---------------------------------
    // Cast
    //---------------------------------
    typename OutputCastImageFilterType::Pointer outputCastImageFilter=OutputCastImageFilterType::New();
    outputCastImageFilter->SetInput(setBackgroundFilter2->GetOutput());
    if(m_Verbose) std::cout<<"Casting the output..."<<std::endl;
    outputCastImageFilter->Update();

    //---------------------------------
    // SetOutput
    //---------------------------------
    this->SetNthOutput(0, outputCastImageFilter->GetOutput());


  }


}//end clitk
 
#endif //#define clitkReconstructThroughDilationImageFilter_txx
