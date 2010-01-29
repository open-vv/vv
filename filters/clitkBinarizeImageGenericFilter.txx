#ifndef clitkBinarizeImageGenericFilter_txx
#define clitkBinarizeImageGenericFilter_txx

/* =================================================
 * @file   clitkBinarizeImageGenericFilter.txx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   29 june 2009
 * 
 * @brief 
 * 
 ===================================================*/

namespace clitk
{
  
  //--------------------------------------------------------------------
  template<class args_info_type>
  BinarizeImageGenericFilter<args_info_type>::BinarizeImageGenericFilter():
    ImageToImageGenericFilter<Self>("Binarize") {
    InitializeImageType<2>();
    InitializeImageType<3>();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class args_info_type>
  template<unsigned int Dim>
  void BinarizeImageGenericFilter<args_info_type>::InitializeImageType() {      
    ADD_IMAGE_TYPE(Dim, char);
    ADD_IMAGE_TYPE(Dim, short);
  }
  //--------------------------------------------------------------------
  

  //--------------------------------------------------------------------
  template<class args_info_type>
  void BinarizeImageGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a) {
    mArgsInfo=a;
    SetIOVerbose(mArgsInfo.verbose_flag);

    if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();

    if (mArgsInfo.input_given) {
      SetInputFilename(mArgsInfo.input_arg);
    }
    if (mArgsInfo.output_given) {
      SetOutputFilename(mArgsInfo.output_arg);
    }
  }
  //--------------------------------------------------------------------

  //--------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //--------------------------------------------------------------------
  template<class args_info_type>
  template<class InputImageType>
  void 
  BinarizeImageGenericFilter<args_info_type>::UpdateWithInputImageType()
  {

    // Reading input
    typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);
    
    // Main filter
    typedef typename InputImageType::PixelType PixelType;
    typedef itk::Image<int, InputImageType::ImageDimension> OutputImageType;

    // Filter
    typedef itk::BinaryThresholdImageFilter<InputImageType, OutputImageType> BinaryThresholdImageFilterType;
    typename BinaryThresholdImageFilterType::Pointer thresholdFilter=BinaryThresholdImageFilterType::New();
    thresholdFilter->SetInput(input);
    if(mArgsInfo.lower_given) thresholdFilter->SetLowerThreshold(static_cast<PixelType>(mArgsInfo.lower_arg));
    if(mArgsInfo.upper_given) thresholdFilter->SetUpperThreshold(static_cast<PixelType>(mArgsInfo.upper_arg));

    DD(mArgsInfo.lower_given);
    DD(mArgsInfo.upper_given);
    DD(mArgsInfo.lower_arg);
    DD(mArgsInfo.upper_arg);

    DD(mArgsInfo.inside_arg);
    DD(mArgsInfo.outside_arg);
    DD(mArgsInfo.inside_given);
    DD(mArgsInfo.outside_given);

    thresholdFilter->SetInsideValue(mArgsInfo.inside_arg);
    thresholdFilter->SetOutsideValue(mArgsInfo.outside_arg);

    thresholdFilter->Update();
  
    // Write/Save results
    typename OutputImageType::Pointer outputImage = thresholdFilter->GetOutput();
    this->template SetNextOutput<OutputImageType>(outputImage);
  }
  //--------------------------------------------------------------------


}//end clitk
 
#endif //#define clitkBinarizeImageGenericFilter_txx
