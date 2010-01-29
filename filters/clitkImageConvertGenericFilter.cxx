#ifndef CLITKIMAGECONVERTGENERICFILTER_CXX
#define CLITKIMAGECONVERTGENERICFILTER_CXX

/**
 -------------------------------------------------
 * @file   clitkImageConvertGenericFilter.cxx
 * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
 * @date   05 May 2008 10:57:19
 * 
 * @brief  
 * 
 * 
 -------------------------------------------------*/

#include "clitkImageConvertGenericFilter.h"

//--------------------------------------------------------------------
clitk::ImageConvertGenericFilter::ImageConvertGenericFilter():
  clitk::ImageToImageGenericFilter<Self>("ImageConvert") {
  mOutputPixelTypeName = "NotSpecified";
  InitializeImageType<2>();
  InitializeImageType<3>();  
  InitializeImageType<4>();  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<unsigned int Dim>
void clitk::ImageConvertGenericFilter::InitializeImageType() {      
  ADD_IMAGE_TYPE(Dim, char);
  ADD_IMAGE_TYPE(Dim, short);
  ADD_IMAGE_TYPE(Dim, unsigned short);
  ADD_IMAGE_TYPE(Dim, int);
  ADD_IMAGE_TYPE(Dim, float);
  ADD_IMAGE_TYPE(Dim, double);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class InputImageType>
void clitk::ImageConvertGenericFilter::UpdateWithInputImageType() {

  // Verbose stuff
  if (mIOVerbose) {
    if (mInputFilenames.size() == 1) {
      std::cout << "Input image <" << mInputFilenames[0] << "> is ";
      itk::ImageIOBase::Pointer header = clitk::readImageHeader(mInputFilenames[0]);
      printImageHeader(header, std::cout);
      std::cout << std::endl;
    }
    else {
      for(unsigned int i=0; i<mInputFilenames.size(); i++) {
	std::cout << "Input image " << i << " <" << mInputFilenames[i] << "> is ";
	itk::ImageIOBase::Pointer h = clitk::readImageHeader(mInputFilenames[i]);	
	printImageHeader(h, std::cout);
	std::cout << std::endl;
      }
    }
  }


  if ((mPixelTypeName == mOutputPixelTypeName) || (mOutputPixelTypeName == "NotSpecified")) {
    //    typename InputImageType::Pointer input = clitk::readImage<InputImageType>(mInputFilenames);
    typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);
    //clitk::writeImage<InputImageType>(input, mOutputFilename, mIOVerbose);
    this->SetNextOutput<InputImageType>(input);
  }
  else {
#define TRY_TYPE(TYPE) \
    if (IsSameType<TYPE>(mOutputPixelTypeName)) { UpdateWithOutputType<InputImageType, TYPE>(); return; }
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
template<class InputImageType, class OutputPixelType>
void clitk::ImageConvertGenericFilter::UpdateWithOutputType() {
  // Read
  typename InputImageType::Pointer input =this->template GetInput<InputImageType>(0);

  // Typedef
  typedef typename InputImageType::PixelType PixelType;

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
  typedef itk::Image<OutputPixelType,InputImageType::ImageDimension> OutputImageType;
  typedef itk::CastImageFilter<InputImageType, OutputImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(input);
  filter->Update();

  // Write
  SetNextOutput<OutputImageType>(filter->GetOutput());
  //clitk::writeImage<OutputImageType>(filter->GetOutput(), mOutputFilename, mIOVerbose);
}
//====================================================================


#endif /* end #define CLITKIMAGECONVERTGENERICFILTER_CXX */

