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
===========================================================================*/
#ifndef clitkDecomposeThroughErosionImageFilter_txx
#define clitkDecomposeThroughErosionImageFilter_txx

/* =================================================
 * @file   clitkDecomposeThroughErosionImageFilter.txx
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
  template<class InputImageType, class OutputImageType>
  DecomposeThroughErosionImageFilter<InputImageType, OutputImageType>::DecomposeThroughErosionImageFilter()
  {
   m_Verbose=false;
   m_Lower =1;
   m_Upper=1;
   m_Inside=1;
   m_Outside=0;
   m_ErosionPaddingValue=static_cast<OutputPixelType>(-1);
   for (unsigned int i=0; i<InputImageDimension; i++)
     m_Radius[i]=1;
   m_NumberOfNewLabels=1;
   m_FullyConnected=true;
   m_MinimumObjectSize=10;
   m_MinimumNumberOfIterations=1;
  }


  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template<class InputImageType, class  OutputImageType> 
  void 
  DecomposeThroughErosionImageFilter<InputImageType, OutputImageType>::GenerateData()
  {

    //---------------------------------
    // Typedefs 
    //--------------------------------- 

    // Internal type
    typedef itk::Image<InternalPixelType, InputImageDimension> InternalImageType;

    // Filters used
    typedef itk::BinaryThresholdImageFilter<InputImageType, InternalImageType> InputBinaryThresholdImageFilter;
    typedef itk::BinaryBallStructuringElement<InputPixelType,InputImageDimension > KernelType;
    typedef itk::BinaryErodeImageFilter<InternalImageType, InternalImageType , KernelType> BinaryErodeImageFilterType;
    typedef itk::BinaryThresholdImageFilter<InternalImageType, InternalImageType> BinaryThresholdImageFilterType;
    typedef itk::StatisticsImageFilter<InternalImageType> StatisticsImageFilterType;
    typedef itk::ConnectedComponentImageFilter<InternalImageType, InternalImageType> ConnectFilterType;
    typedef itk::RelabelComponentImageFilter<InternalImageType, InternalImageType> RelabelImageFilterType;
    typedef clitk::SetBackgroundImageFilter<InternalImageType, InternalImageType, InternalImageType> SetBackgroundImageFilterType;
  
    //---------------------------------
    // Binarize input
    //--------------------------------- 
    typename  InputBinaryThresholdImageFilter::Pointer inputBinarizer=InputBinaryThresholdImageFilter::New();
    inputBinarizer->SetInput(this->GetInput());
    inputBinarizer->SetLowerThreshold(m_Lower);
    inputBinarizer->SetUpperThreshold(m_Upper);
    inputBinarizer ->SetInsideValue(m_Inside);
    inputBinarizer ->SetOutsideValue(m_Outside);
    if(m_Verbose) std::cout<<"Binarizing the input..."<<std::endl;
    inputBinarizer->Update();
 
    //---------------------------------
    // Label the input
    //---------------------------------
    typename ConnectFilterType::Pointer inputConnectFilter=ConnectFilterType::New();
    inputConnectFilter->SetInput(inputBinarizer->GetOutput());
    inputConnectFilter->SetBackgroundValue(0);
    inputConnectFilter->SetFullyConnected(m_FullyConnected);
    if(m_Verbose) std::cout<<"Labelling the connected components..."<<std::endl;
    //   inputConnectFilter->Update();
 
    //---------------------------------
    // Count the initial labels
    //---------------------------------
    typename StatisticsImageFilterType::Pointer inputStatisticsImageFilter=StatisticsImageFilterType::New();
    inputStatisticsImageFilter->SetInput(inputConnectFilter->GetOutput());
    if(m_Verbose) std::cout<<"Counting the initial labels..."<<std::endl;
    inputStatisticsImageFilter->Update();
    unsigned int initialNumberOfLabels= inputStatisticsImageFilter->GetMaximum();
    if(m_Verbose) std::cout<<"The input contained "<<initialNumberOfLabels<<" disctictive label(s)..."<<std::endl;
    if(m_Verbose) std::cout<<"Performing erosions till at least "<<initialNumberOfLabels+m_NumberOfNewLabels<<" distinctive labels are counted..."<<std::endl;
 
    //---------------------------------
    // Structuring element
    //---------------------------------
    KernelType structuringElement;
    structuringElement.SetRadius(m_Radius);
    structuringElement.CreateStructuringElement();

    //---------------------------------
    // Repeat while not decomposed
    //---------------------------------
    typename InternalImageType::Pointer current=inputBinarizer->GetOutput();
    typename InternalImageType::Pointer output=inputBinarizer->GetOutput();
    unsigned int iteration=0;
    unsigned int max =initialNumberOfLabels;

    while (  (iteration < m_MinimumNumberOfIterations) || ( (max< initialNumberOfLabels + m_NumberOfNewLabels ) && (iteration<100 ) ) )
      {


	if(m_Verbose) std::cout<<"Eroding image (iteration "<<iteration<<")..."<<std::endl;
	
	//---------------------------------
	// Erode
	//---------------------------------
	typename BinaryErodeImageFilterType::Pointer erosionFilter=BinaryErodeImageFilterType::New();
	erosionFilter->SetInput (current);
	erosionFilter->SetForegroundValue (1);
	erosionFilter->SetBackgroundValue (-1);
	erosionFilter->SetBoundaryToForeground(false);
	erosionFilter->SetKernel(structuringElement);
	erosionFilter->Update();
	current=erosionFilter->GetOutput();
	
	//---------------------------------
	// Binarize (remove -1)
	//--------------------------------- 
	typename  BinaryThresholdImageFilterType::Pointer binarizer=BinaryThresholdImageFilterType::New();
	binarizer->SetInput(erosionFilter->GetOutput());
	binarizer->SetLowerThreshold(1);
	binarizer->SetUpperThreshold(1);
	binarizer ->SetInsideValue(1);
	binarizer ->SetOutsideValue(0);
	if(m_Verbose) std::cout<<"Binarizing the eroded image..."<<std::endl;
	//binarizer->Update();

	
	//---------------------------------
	// ReLabel the connected components
	//---------------------------------
	typename ConnectFilterType::Pointer connectFilter=ConnectFilterType::New();
	connectFilter->SetInput(binarizer->GetOutput());
	connectFilter->SetBackgroundValue(0);
	connectFilter->SetFullyConnected(m_FullyConnected);
	if(m_Verbose) std::cout<<"Labelling the connected components..."<<std::endl;
	//connectFilter->Update();
		
	//---------------------------------
	// Sort
	//---------------------------------
	typename RelabelImageFilterType::Pointer relabelFilter=RelabelImageFilterType::New();
	relabelFilter->SetInput(connectFilter->GetOutput());
	relabelFilter->SetMinimumObjectSize(m_MinimumObjectSize);
	//relabelFilter->Update();   
	
	
	//---------------------------------
	// Count the labels
	//---------------------------------
	typename StatisticsImageFilterType::Pointer statisticsImageFilter=StatisticsImageFilterType::New();
	statisticsImageFilter->SetInput(relabelFilter->GetOutput());
	statisticsImageFilter->Update();
	max= statisticsImageFilter->GetMaximum();
	if(m_Verbose) std::cout<<"Counted "<<max<<" label (s) larger then "<<m_MinimumObjectSize<<" voxels..."<<std::endl;
	output=statisticsImageFilter->GetOutput();
	
	// Next iteration
	iteration++;
      }


    //---------------------------------
    // Binarize current (remove -1)
    //--------------------------------- 
    typename  BinaryThresholdImageFilterType::Pointer binarizer=BinaryThresholdImageFilterType::New();
    binarizer->SetInput(current);
    binarizer->SetLowerThreshold(1);
    binarizer->SetUpperThreshold(1);
    binarizer ->SetInsideValue(1);
    binarizer ->SetOutsideValue(0);
    if(m_Verbose) std::cout<<"Binarizing the eroded image..."<<std::endl;
    //binarizer->Update();
    
    //---------------------------------
    // ReLabel the connected components
    //---------------------------------
    typename ConnectFilterType::Pointer connectFilter=ConnectFilterType::New();
    connectFilter->SetInput(binarizer->GetOutput());
    connectFilter->SetBackgroundValue(0);
    connectFilter->SetFullyConnected(m_FullyConnected);
    if(m_Verbose) std::cout<<"Labelling the connected components..."<<std::endl;
    connectFilter->Update();
    
    //---------------------------------
    // Sort
    //---------------------------------
    typename RelabelImageFilterType::Pointer relabelFilter=RelabelImageFilterType::New();
    relabelFilter->SetInput(connectFilter->GetOutput());
    //relabelFilter->SetMinimumObjectSize(m_MinimumObjectSize); // Preserve all intensities
    //relabelFilter->Update();   

    //---------------------------------
    // Set -1 to padding value
    //---------------------------------
    typename SetBackgroundImageFilterType::Pointer setBackgroundFilter =SetBackgroundImageFilterType::New();
    setBackgroundFilter->SetInput(relabelFilter->GetOutput());
    setBackgroundFilter->SetInput2(current);
    setBackgroundFilter->SetMaskValue(-1);
    setBackgroundFilter->SetOutsideValue(m_ErosionPaddingValue);
    if(m_Verbose) std::cout<<"Setting the eroded region to "<<m_ErosionPaddingValue<<"..."<<std::endl;
        
    //---------------------------------
    // Cast
    //---------------------------------
    typedef itk::CastImageFilter<InternalImageType, OutputImageType> CastImageFilterType;
    typename CastImageFilterType::Pointer caster= CastImageFilterType::New();
    caster->SetInput(setBackgroundFilter->GetOutput());
    caster->Update();
    
    //---------------------------------
    // SetOutput
    //---------------------------------
    this->SetNthOutput(0, caster->GetOutput());
  }


}//end clitk
 
#endif //#define clitkDecomposeThroughErosionImageFilter_txx
