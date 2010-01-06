#ifndef CLITKIMAGERESAMPLEGENERICFILTER_TXX
#define CLITKIMAGERESAMPLEGENERICFILTER_TXX

/**
   ------------------------------------------------=
   * @file   clitkImageResampleGenericFilter.txx
   * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
   * @date   23 Feb 2008 08:40:11
   * 
   * @brief  
   * 
   * 
   ------------------------------------------------=*/

//--------------------------------------------------------------------
template<unsigned int Dim>
void ImageResampleGenericFilter::Update_WithDim() { 

#define TRY_TYPE(TYPE)							\
  if (IsSameType<TYPE>(mPixelTypeName)) { Update_WithDimAndPixelType<Dim, TYPE>(); return; } 
  TRY_TYPE(signed char);
  TRY_TYPE(uchar);
  TRY_TYPE(short);
  TRY_TYPE(ushort);
  TRY_TYPE(int);
  TRY_TYPE(unsigned int); 
  TRY_TYPE(float);
  TRY_TYPE(double);
#undef TRY_TYPE

  std::string list = CreateListOfTypes<char, uchar, short, ushort, int, float, double>();
  std::cerr << "Error, I don't know the type '" << mPixelTypeName << "' for the input image '"
	    << mInputFilenames[0] << "'." << std::endl << "Known types are " << list << std::endl;
  exit(0);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<unsigned int Dim, class PixelType>
void ImageResampleGenericFilter::Update_WithDimAndPixelType() {
  // Reading input
  typedef itk::Image<PixelType,Dim> ImageType;
  //typename ImageType::Pointer input = clitk::readImage<ImageType>(mInputFilenames, mIOVerbose);
  typename ImageType::Pointer input = this->GetInput<ImageType>(0);

  // Main filter
  typename ImageType::Pointer outputImage = ComputeImage<ImageType>(input);

  // Write results
  this->SetNextOutput<ImageType>(outputImage);
  //clitk::writeImage<ImageType>(outputImage, mOutputFilename, mIOVerbose);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer 
ImageResampleGenericFilter::ComputeImage(typename ImageType::Pointer inputImage) {

  // Warning
  if (!std::numeric_limits<typename ImageType::PixelType>::is_signed) {
    if ((mInterpolatorName == "bspline") || (mInterpolatorName == "blut")) {
      std::cerr << "Warning : input pixel type is not signed, use bspline interpolation at your own risk ..." << std::endl;
    }
  }

  // Check options
  static unsigned int dim = ImageType::ImageDimension;
  if (mOutputSize.size() != dim) {
    std::cerr << "Please set size with " << dim << " dimensions." << std::endl;
    return NULL;
  }
  if (mOutputSpacing.size() != dim) {
    std::cerr << "Please set spacing with " << dim << " dimensions." << std::endl;
    return NULL;
  }
  mOutputOrigin.resize(dim);

  if (mApplyGaussianFilterBefore && mSigma.size() != dim) {
    std::cerr << "Please set sigma with " << dim << " dimensions." << std::endl;
    return NULL;
  }

  // Some typedefs
  typedef typename ImageType::SizeType SizeType;
  typedef typename ImageType::SpacingType SpacingType;
  typedef typename ImageType::PointType PointType;


  // Create Image Filter
  typedef itk::ResampleImageFilter<ImageType,ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
    
  // Instance of the transform object to be passed to the resample
  // filter. By default, identity transform is applied
  typedef itk::AffineTransform<double, ImageType::ImageDimension> TransformType;
  typename TransformType::Pointer transform =  TransformType::New();
  filter->SetTransform(transform);

  // Set filter's parameters
  SizeType outputSize;
  SpacingType outputSpacing;
  PointType outputOrigin;
  for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
    outputSize[i] = mOutputSize[i];
    outputSpacing[i] = mOutputSpacing[i];
    outputOrigin[i] = inputImage->GetOrigin()[i];
  }

  filter->SetSize(outputSize);
  filter->SetOutputSpacing(outputSpacing);
  filter->SetOutputOrigin(outputOrigin);
  filter->SetDefaultPixelValue(static_cast<typename ImageType::PixelType>(mDefaultPixelValue));//DS TODO//JV comme ça?

  // Select interpolator
  if (mInterpolatorName == "nn") {
    typedef itk::NearestNeighborInterpolateImageFunction<ImageType, double> InterpolatorType;     
    typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
    filter->SetInterpolator(interpolator);
  }
  else { 
    if (mInterpolatorName == "linear") {
      typedef itk::LinearInterpolateImageFunction<ImageType, double> InterpolatorType;     
      typename InterpolatorType::Pointer interpolator =  InterpolatorType::New();
      filter->SetInterpolator(interpolator);
    }
    else {
      if (mInterpolatorName == "bspline") {
	typedef itk::BSplineInterpolateImageFunction<ImageType, double> InterpolatorType; 
	typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
	interpolator->SetSplineOrder(mBSplineOrder);
	filter->SetInterpolator(interpolator);
      }
      else {
	if (mInterpolatorName == "blut") {
	  typedef itk::BSplineInterpolateImageFunctionWithLUT<ImageType, double> InterpolatorType; 
	  typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
	  interpolator->SetSplineOrder(mBSplineOrder);
	  interpolator->SetLUTSamplingFactor(mSamplingFactors[0]);
	  filter->SetInterpolator(interpolator);
	}
	else {
	  std::cerr << "Sorry, I do not know the interpolator '" << mInterpolatorName 
		    << "'. Known interpolators are :  nn, linear, bspline, blut" << std::endl;
	  exit(0);
	}
      }
    }
  }

  // Build initial Gaussian bluring (if needed)
  typedef itk::RecursiveGaussianImageFilter<ImageType, ImageType> GaussianFilterType;
  std::vector<typename GaussianFilterType::Pointer> gaussianFilters;
  if (mApplyGaussianFilterBefore) {
    for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
      // Create filter
      gaussianFilters.push_back(GaussianFilterType::New());
      // Set options
      gaussianFilters[i]->SetDirection(i);
      gaussianFilters[i]->SetOrder(GaussianFilterType::ZeroOrder);
      gaussianFilters[i]->SetNormalizeAcrossScale(false);
      gaussianFilters[i]->SetSigma(mSigma[i]); // in millimeter !
      // Set input
      if (i==0) gaussianFilters[i]->SetInput(inputImage);
      else gaussianFilters[i]->SetInput(gaussianFilters[i-1]->GetOutput());
    }
    filter->SetInput(gaussianFilters[ImageType::ImageDimension-1]->GetOutput());
  }
  else {
    filter->SetInput(inputImage);
  }

  // Go !
  try { 
    filter->Update();
  }
  catch( itk::ExceptionObject & err ) {
    std::cerr << "Error while filtering " << mInputFilenames[0].c_str() 
	      << " " << err << std::endl;
    exit(0);
  }

  // Return result
  return filter->GetOutput();

}
//--------------------------------------------------------------------



#endif /* end #define CLITKIMAGERESAMPLEGENERICFILTER_TXX */

