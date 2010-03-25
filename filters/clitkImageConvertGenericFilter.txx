#ifndef CLITKIMAGECONVERTGENERICFILTER_TXX
#define CLITKIMAGECONVERTGENERICFILTER_TXX
/**
   =================================================
   * @file   clitkImageConvertGenericFilter.txx
   * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
   * @date   05 May 2008 11:14:20
   * 
   * @brief  
   * 
   * 
   =================================================*/

#include <limits>

//====================================================================
template<unsigned int Dim>
void ImageConvertGenericFilter::Update_WithDim() {  
#define TRY_TYPE(TYPE) \
  if (IsSameType<TYPE>(mPixelTypeName)) { Update_WithDimAndPixelType<Dim, TYPE>(); return; } 
  // TRY_TYPE(signed char);
  TRY_TYPE(char);
  TRY_TYPE(uchar);
  TRY_TYPE(short);
  TRY_TYPE(ushort);
  TRY_TYPE(int); 
  TRY_TYPE(unsigned int); 
  TRY_TYPE(float);
  TRY_TYPE(double);
#undef TRY_TYPE

  std::string list = CreateListOfTypes<uchar, short, ushort, int, uint, float, double>();
  std::cerr << "Error, I don't know the type '" << mPixelTypeName << "' for the input image '"
	    << mInputFilenames[0] << "'." << std::endl << "Known types are " << list << std::endl;
  exit(0);
}
//====================================================================

//====================================================================
template<unsigned int Dim, class PixelType>
void ImageConvertGenericFilter::Update_WithDimAndPixelType() {
  if ((mPixelTypeName == mOutputPixelTypeName) || (mOutputPixelTypeName == "NotSpecified")) {
    typedef itk::Image<PixelType,Dim> InputImageType;
    typename InputImageType::Pointer input = clitk::readImage<InputImageType>(mInputFilenames);
    //clitk::writeImage<InputImageType>(input, mOutputFilename, mIOVerbose);
    this->SetNextOutput<InputImageType>(input);
  }
  else {
#define TRY_TYPE(TYPE) \
    if (IsSameType<TYPE>(mOutputPixelTypeName)) { Update_WithDimAndPixelTypeAndOutputType<Dim, PixelType, TYPE>(); return; }
    TRY_TYPE(char);
    // TRY_TYPE(signed char);
    TRY_TYPE(uchar);
    TRY_TYPE(short);
    TRY_TYPE(ushort);
    TRY_TYPE(int); // no uint ...
    TRY_TYPE(float);
    TRY_TYPE(double);
#undef TRY_TYPE

    std::string list = CreateListOfTypes<char, uchar, short, ushort, int, float, double>();
    std::cerr << "Error, I don't know the output type '" << mOutputPixelTypeName 
	      << "'. " << std::endl << "Known types are " << list << "." << std::endl;
    exit(0);
  }  
}
//====================================================================

//====================================================================
template<unsigned int Dim, class PixelType, class OutputPixelType>
void ImageConvertGenericFilter::Update_WithDimAndPixelTypeAndOutputType() {
  // Read
  typedef itk::Image<PixelType,Dim> InputImageType;
  typename InputImageType::Pointer input = clitk::readImage<InputImageType>(mInputFilenames);

  // Warning
  if (std::numeric_limits<PixelType>::is_signed) {
    if (!std::numeric_limits<OutputPixelType>::is_signed) {
      std::cerr << "Warning, input type is signed (" << mPixelTypeName << ") while output type is not (" 
		<< mOutputPixelTypeName << "), use at your own responsability." << std::endl;
    }
  }
  if (!std::numeric_limits<PixelType>::is_integer) {
    if (std::numeric_limits<OutputPixelType>::is_integer) {
      std::cerr << "Warning, input type is not integer (" << mPixelTypeName << ") while output type is (" 
		<< mOutputPixelTypeName << "), use at your own responsability." << std::endl;
    }
  }
  //  DD(std::numeric_limits<PixelType>::digits10);
  // DD(std::numeric_limits<OutputPixelType>::digits10);
  if (!std::numeric_limits<PixelType>::is_integer) {
    if (std::numeric_limits<OutputPixelType>::is_integer) {
      std::cerr << "Warning, input type is not integer (" << mPixelTypeName << ") while output type is (" 
		<< mOutputPixelTypeName << "), use at your own responsability." << std::endl;
    }
  }
  if (std::numeric_limits<PixelType>::digits10 > std::numeric_limits<OutputPixelType>::digits10) {
    std::cerr << "Warning, possible loss of precision : input type is (" << mPixelTypeName << ") while output type is (" 
		<< mOutputPixelTypeName << "), use at your own responsability." << std::endl;
  }

  // Cast
  typedef itk::Image<OutputPixelType,Dim> OutputImageType;
  typedef itk::CastImageFilter<InputImageType, OutputImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(input);
  filter->Update();

  // Write
  SetNextOutput<OutputImageType>(filter->GetOutput());
  //clitk::writeImage<OutputImageType>(filter->GetOutput(), mOutputFilename, mIOVerbose);
}
//====================================================================

#endif /* end #define CLITKIMAGECONVERTGENERICFILTER_TXX */

