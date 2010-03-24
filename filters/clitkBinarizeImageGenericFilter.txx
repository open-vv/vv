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

// itk include
#include "itkBinaryThresholdImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkMaskNegatedImageFilter.h"

namespace clitk
{
  
  //--------------------------------------------------------------------
  template<class args_info_type>
  BinarizeImageGenericFilter<args_info_type>::BinarizeImageGenericFilter():
    ImageToImageGenericFilter<Self>("Binarize") {
    InitializeImageType<2>();
    InitializeImageType<3>();
    InitializeImageType<4>();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class args_info_type>
  template<unsigned int Dim>
  void BinarizeImageGenericFilter<args_info_type>::InitializeImageType() {      
    ADD_IMAGE_TYPE(Dim, char);
    ADD_IMAGE_TYPE(Dim, uchar);
    ADD_IMAGE_TYPE(Dim, short);
    ADD_IMAGE_TYPE(Dim, int);
    ADD_IMAGE_TYPE(Dim, float);
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
    typedef itk::Image<char, InputImageType::ImageDimension> OutputImageType;

    // Filter
    typedef itk::BinaryThresholdImageFilter<InputImageType, OutputImageType> BinaryThresholdImageFilterType;
    typename BinaryThresholdImageFilterType::Pointer thresholdFilter=BinaryThresholdImageFilterType::New();
    thresholdFilter->SetInput(input);
    thresholdFilter->SetInsideValue(mArgsInfo.fg_arg);
 
    if (mArgsInfo.lower_given) thresholdFilter->SetLowerThreshold(static_cast<PixelType>(mArgsInfo.lower_arg));
    if (mArgsInfo.upper_given) thresholdFilter->SetUpperThreshold(static_cast<PixelType>(mArgsInfo.upper_arg));

    // DD(mArgsInfo.lower_given);
    // DD(mArgsInfo.upper_given);
    // DD(mArgsInfo.lower_arg);
    // DD(mArgsInfo.upper_arg);
    // DD(mArgsInfo.fg_arg);
    // DD(mArgsInfo.bg_arg);
    // DD(mArgsInfo.fg_given);
    // DD(mArgsInfo.bg_given);
    // DD(mArgsInfo.mode_arg);

// <<<<<<< clitkBinarizeImageGenericFilter.txx
//     DD(mArgsInfo.useFG_flag);
//     DD(mArgsInfo.useBG_flag);

//     thresholdFilter->SetInsideValue(mArgsInfo.fg_arg);

//     // Keep BG value to 0 if maskFilterType is used after
//     if (mArgsInfo.useBG_flag && mArgsInfo.useFG_flag) {
//       thresholdFilter->SetOutsideValue(mArgsInfo.bg_arg);
//     }
//     else {
//       DD("0");
//       thresholdFilter->SetOutsideValue(0);
//     }

//     // thresholdFilter->Update();

//     // If no BG or no FG : new image, copy input with MaskImageFilter
//     // If setFG -> FG BG have been changed
//     if (mArgsInfo.useBG_flag && mArgsInfo.useFG_flag) {
// =======
    /* Three modes : 
       - FG -> only use FG value for pixel in the Foreground (or Inside), keep input values for outside
       - BG -> only use BG value for pixel in the Background (or Outside), keep input values for inside
       - both -> use FG and BG (real binary image)
    */
      if (mArgsInfo.mode_arg == std::string("both")) {
      thresholdFilter->SetOutsideValue(mArgsInfo.bg_arg);
      thresholdFilter->Update();
      //>>>>>>> 1.5
      typename OutputImageType::Pointer outputImage = thresholdFilter->GetOutput();
      this->template SetNextOutput<OutputImageType>(outputImage);
    }
    else {
      typename InputImageType::Pointer outputImage;
      thresholdFilter->SetOutsideValue(0);
      if (mArgsInfo.mode_arg == std::string("BG")) {
	typedef itk::MaskImageFilter<InputImageType,OutputImageType> maskFilterType;
	typename maskFilterType::Pointer maskFilter = maskFilterType::New();
	maskFilter->SetInput1(input);
	maskFilter->SetInput2(thresholdFilter->GetOutput());
	maskFilter->SetOutsideValue(mArgsInfo.bg_arg);
	maskFilter->Update();
	outputImage = maskFilter->GetOutput();
      }
      else {
	typedef itk::MaskNegatedImageFilter<InputImageType,OutputImageType> maskFilterType;
	typename maskFilterType::Pointer maskFilter = maskFilterType::New();
	maskFilter->SetInput1(input);
	maskFilter->SetInput2(thresholdFilter->GetOutput());
	maskFilter->SetOutsideValue(mArgsInfo.fg_arg);
	maskFilter->Update();
	outputImage = maskFilter->GetOutput();
      }
      // Write/Save results
      this->template SetNextOutput<InputImageType>(outputImage);
    }
  }
  //--------------------------------------------------------------------


}//end clitk
 
#endif //#define clitkBinarizeImageGenericFilter_txx
