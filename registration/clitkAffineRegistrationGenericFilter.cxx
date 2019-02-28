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

#ifndef CLITKAFFINEREGISTRATIONGENERICFILTER_CXX
#define CLITKAFFINEREGISTRATIONGENERICFILTER_CXX

#include "clitkAffineRegistrationGenericFilter.h"
// clitk include
#include "clitkIO.h"
#include "clitkCommon.h"
#include "clitkImageCommon.h"
#include "clitkAffineRegistration_ggo.h"
#include "clitkImageArithm_ggo.h"
#include "clitkCorrelationRatioImageToImageMetric.h"
#include "clitkTransformUtilities.h"
#include "clitkGenericMetric.h"
#include "clitkGenericOptimizer.h"
#include "clitkGenericInterpolator.h"
#include "clitkGenericAffineTransform.h"
#include "clitkImageToImageGenericFilter.h"


//itk include
#include <itkMultiResolutionImageRegistrationMethod.h>
#include <itkMultiResolutionPyramidImageFilter.h>
#include <itkImageToImageMetric.h>
#include <itkEuler2DTransform.h>
#include <itkCenteredEuler3DTransform.h>
#include <itkImage.h>
#include <itkResampleImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkNormalizeImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>
#include <itkImageMaskSpatialObject.h>
#include <itkCommand.h>
#include <itkCheckerBoardImageFilter.h>
#include <itkSubtractImageFilter.h>
#include <itkLightObject.h>
#include <itkImageMomentsCalculator.h>
#include <itkThresholdImageFilter.h>

// other includes
#include <time.h>
#include <iostream>
#include <iomanip>


namespace clitk
{
//==============================================================================
//Creating an observer class that allows us to monitor the registration
//================================================================================
class CommandIterationUpdate : public itk::Command
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef  itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};
public:
  typedef   clitk::GenericOptimizer<args_info_clitkAffineRegistration>     OptimizerType;
  typedef   const OptimizerType   *           OptimizerPointer;

  // Set the generic optimizer
  void SetOptimizer(OptimizerPointer o) {
    m_Optimizer=o;
  }

  // Execute
  void Execute(itk::Object *caller, const itk::EventObject & event) ITK_OVERRIDE {
    Execute( (const itk::Object *)caller, event);
  }

  void Execute(const itk::Object * object, const itk::EventObject & event) ITK_OVERRIDE {
    if ( !(itk::IterationEvent().CheckEvent( &event )) ) {
      return;
    }

    m_Optimizer->OutputIterationInfo();
  }

  OptimizerPointer m_Optimizer;
};
//==================================================================================================================================//
//Constructor
//===================================================================================================================================//

AffineRegistrationGenericFilter::AffineRegistrationGenericFilter():
    ImageToImageGenericFilter<Self>("Register")

{
  InitializeImageType<2>();
  InitializeImageType<3>();
  m_Verbose=true;
}
//==========================================================================================================//
//============================================================================================================//
//--------------------------------------------------------------------
template<unsigned int Dim>
void AffineRegistrationGenericFilter::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//==============================================================================
//Creating an observer class that allows us to change parameters at subsequent levels
//==============================================================================
template <typename TRegistration>
class RegistrationInterfaceCommand : public itk::Command
{
public:
  typedef  RegistrationInterfaceCommand   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  RegistrationInterfaceCommand() {};
public:

  // Registration
  typedef   TRegistration                              RegistrationType;
  typedef   RegistrationType *                         RegistrationPointer;

  // Metric
  typedef typename RegistrationType::FixedImageType    InternalImageType;
  typedef clitk::GenericMetric<args_info_clitkAffineRegistration, InternalImageType, InternalImageType> GenericMetricType;

  // Two arguments are passed to the Execute() method: the first
  // is the pointer to the object which invoked the event and the
  // second is the event that was invoked.
  void Execute(itk::Object * object, const itk::EventObject & event) ITK_OVERRIDE {
    if ( !(itk::IterationEvent().CheckEvent( &event )) ) {
      return;
    }

    // Get the levels
    RegistrationPointer registration = dynamic_cast<RegistrationPointer>( object );
    unsigned int numberOfLevels=registration->GetNumberOfLevels();
    unsigned int currentLevel=registration->GetCurrentLevel()+1;

    // Output the levels
    std::cout<<std::endl;
    std::cout<<"========================================"<<std::endl;
    std::cout<<"Starting resolution level "<<currentLevel<<" of "<<numberOfLevels<<"..."<<std::endl;
    std::cout<<"========================================"<<std::endl;
    std::cout<<std::endl;

    // Higher level?
    if (currentLevel>1) {
      // Reinitialize the metric (!= number of samples)
      typename GenericMetricType::Pointer genericMetric= GenericMetricType::New();
      genericMetric->SetArgsInfo(m_ArgsInfo);
      genericMetric->SetFixedImage(registration->GetFixedImagePyramid()->GetOutput(registration->GetCurrentLevel()));
      if (m_ArgsInfo.referenceMask_given)  genericMetric->SetFixedImageMask(registration->GetMetric()->GetFixedImageMask());
      typedef itk::ImageToImageMetric< InternalImageType, InternalImageType >  MetricType;
      typename  MetricType::Pointer metric=genericMetric->GetMetricPointer();
      registration->SetMetric(metric);
    }
  }

  void Execute(const itk::Object * , const itk::EventObject & ) ITK_OVERRIDE {
    return;
  }

 void SetArgsInfo(args_info_clitkAffineRegistration a) {
    m_ArgsInfo=a;
  }
  args_info_clitkAffineRegistration m_ArgsInfo;
};

//==============================================================================================//
// ArgsInfo
//==============================================================================================//
void AffineRegistrationGenericFilter::SetArgsInfo(const args_info_clitkAffineRegistration & a)
{
  m_ArgsInfo=a;
  if (m_ArgsInfo.reference_given) AddInputFilename(m_ArgsInfo.reference_arg);

  if (m_ArgsInfo.target_given) {
    AddInputFilename(m_ArgsInfo.target_arg);
  }

  if (m_ArgsInfo.output_given) SetOutputFilename(m_ArgsInfo.output_arg);
}
//==============================================================================
// Update with the number of dimensions and pixeltype
//==============================================================================
template<class InputImageType>
void AffineRegistrationGenericFilter::UpdateWithInputImageType()
{
  //=============================================================================
  //Input
  //=============================================================================

  typedef typename  InputImageType::PixelType PixelType;
//typedef typename InputImageType::ImageDimension Dimension;

  bool threadsGiven=m_ArgsInfo.threads_given;
  int threads=m_ArgsInfo.threads_arg;

  //Coordinate Representation
  typedef double TCoordRep;


  typename InputImageType::Pointer fixedImage = this->template GetInput<InputImageType>(0);

  typename InputImageType::Pointer inputFixedImage = this->template GetInput<InputImageType>(0);

  // typedef input2
  typename InputImageType::Pointer movingImage = this->template GetInput<InputImageType>(1);

  typename InputImageType::Pointer inputMovingImage = this->template GetInput<InputImageType>(1);



  //The pixeltype of the fixed image will be used for output
  typedef itk::Image< PixelType, InputImageType::ImageDimension > FixedImageType;

  //Whatever the pixel type, internally we work with an image represented in float
  typedef typename  InputImageType::PixelType  InternalPixelType;
  typedef itk::Image< PixelType, InputImageType::ImageDimension > InternalImageType;


  //Read in the reference/fixed image
//  typedef itk::ImageFileReader< InternalImageType > ReaderType;
//  typename ReaderType::Pointer  fixedImageReader  = ReaderType::New();
//  fixedImageReader->SetFileName( m_ArgsInfo.reference_arg);


  //Read in the object/moving image
//  typename ReaderType::Pointer movingImageReader = ReaderType::New();
//  movingImageReader->SetFileName( m_ArgsInfo.target_arg );
  if (m_Verbose) std::cout<<"Reading images..."<<std::endl;
//  fixedImageReader->Update();
//  movingImageReader->Update();

  if (m_Verbose) std::cout  << "Reading images... " << std::endl;

  //we connect pointers to these internal images
 // typedef typename  fixedImageReader fixedImage;
 // typedef typename  movingImageReader movingImage;

  //We keep the images used for input for possible output
// typedef typename   fixedImageReader inputFixedImage;
// typedef typename  movingImageReader inputMovingImage;


  //============================================================================
  // Preprocessing
  //============================================================================

  //If given, the intensities of both images are first normalized to a zero mean and SD of 1
  // (usefull for MI, not necessary for Mattes' MI but performed anyway for the ouput)
  if ( m_ArgsInfo.normalize_flag ) {
    typedef itk::NormalizeImageFilter< InternalImageType,InternalImageType >  NormalizeFilterType;

    typename  NormalizeFilterType::Pointer  fixedNormalizeFilter = NormalizeFilterType::New();
    typename  NormalizeFilterType::Pointer  movingNormalizeFilter = NormalizeFilterType::New();

    fixedNormalizeFilter->SetInput( fixedImage );
    movingNormalizeFilter->SetInput( movingImage );

    fixedNormalizeFilter->Update();
    movingNormalizeFilter->Update();

    //We keep the images used for input for possible output
    inputFixedImage= fixedNormalizeFilter->GetOutput();
    inputMovingImage= movingNormalizeFilter->GetOutput();

    //the pointers are reconnected for further output
    fixedImage=fixedNormalizeFilter->GetOutput();
    movingImage=movingNormalizeFilter->GetOutput();

    if (m_Verbose)  std::cout <<  "Normalizing image intensities to zero mean and SD of 1..." << std::endl;
  }


  //If given, the images are blurred before processing
  if ( m_ArgsInfo.blur_arg!= 0) {
    typedef itk::DiscreteGaussianImageFilter<InternalImageType,InternalImageType> GaussianFilterType;
    typename GaussianFilterType::Pointer fixedSmoother = GaussianFilterType::New();
    typename GaussianFilterType::Pointer movingSmoother = GaussianFilterType::New();
    fixedSmoother->SetVariance( m_ArgsInfo.blur_arg );
    movingSmoother->SetVariance(m_ArgsInfo.blur_arg );

    fixedSmoother->SetInput( fixedImage );
    movingSmoother->SetInput( movingImage );

    fixedSmoother->Update();
    movingSmoother->Update();

    fixedImage=fixedSmoother->GetOutput();
    movingImage=movingSmoother->GetOutput();

    if (m_Verbose)  std::cout <<  "Blurring images with a Gaussian with standard deviation of " << m_ArgsInfo.blur_arg <<"..." << std::endl;
  }


  //============================================================================
  // Setting up the moving image in a reference system
  //============================================================================
  const itk::Vector<double, InputImageType::ImageDimension> movingResolution = movingImage->GetSpacing();
  typename InternalImageType::RegionType movingRegion = movingImage->GetLargestPossibleRegion();
  typename InternalImageType::RegionType::SizeType  movingSize = movingRegion.GetSize();

  // Print the parameters of the moving image
  if (m_Verbose) {
    std::cout << "Object or Moving image:"<<std::endl;
    std::cout << "Size: " << movingSize[0] << ", " << movingSize[1];
    if (InputImageType::ImageDimension==3) std::cout<<", " << movingSize[2];
    std::cout << std::endl;

    std::cout<< "Resolution: "<< movingResolution[0] << ", " << movingResolution[1];
    if (InputImageType::ImageDimension==3) std::cout<< ", " << movingResolution[2];
    std::cout << std::endl;
  }


  //============================================================================
  // Setting up the fixed image in a reference system
  //============================================================================
  const itk::Vector<double, InputImageType::ImageDimension> fixedResolution = fixedImage->GetSpacing();
  typename InternalImageType::RegionType fixedRegion = fixedImage->GetLargestPossibleRegion();
  typename InternalImageType::RegionType::SizeType fixedSize = fixedRegion.GetSize();

  // Print the parameters of the moving image and the transform
  if (m_Verbose) {
    std::cout << "Target or Moving image:"<<std::endl;
    std::cout << "Size: " << fixedSize[0] << ", " << fixedSize[1];
    if (InputImageType::ImageDimension==3) std::cout<<", " << fixedSize[2];
    std::cout << std::endl;

    std::cout<< "Resolution: "<< fixedResolution[0] << ", " << fixedResolution[1];
    if (InputImageType::ImageDimension==3) std::cout<< ", " << fixedResolution[2];
    std::cout << std::endl;
  }



  //===========================================================================
  // If given, we connect a mask to reference or target
  //============================================================================
  typedef itk::ImageMaskSpatialObject<  InputImageType::ImageDimension >   MaskType;
  typename MaskType::Pointer  fixedMask=ITK_NULLPTR;
  if (m_ArgsInfo.referenceMask_given) {
    fixedMask= MaskType::New();
    typedef itk::Image< unsigned char, InputImageType::ImageDimension >   ImageMaskType;
    typedef itk::ImageFileReader< ImageMaskType >    MaskReaderType;
    typename MaskReaderType::Pointer  maskReader = MaskReaderType::New();
    maskReader->SetFileName(m_ArgsInfo.referenceMask_arg);
    try {
      maskReader->Update();
    } catch ( itk::ExceptionObject & err ) {
      std::cerr << "ExceptionObject caught while reading mask !" << std::endl;
      std::cerr << err << std::endl;
      return;
    }
    if (m_Verbose)std::cout <<"Reference image mask was read..." <<std::endl;

    // Set the image to the spatialObject
    fixedMask->SetImage( maskReader->GetOutput() );
  }

  typedef itk::ImageMaskSpatialObject<  InputImageType::ImageDimension >   MaskType;
  typename MaskType::Pointer  movingMask=ITK_NULLPTR;
  if (m_ArgsInfo.targetMask_given) {
    movingMask= MaskType::New();
    typedef itk::Image< unsigned char, InputImageType::ImageDimension >   ImageMaskType;
    typedef itk::ImageFileReader< ImageMaskType >    MaskReaderType;
    typename MaskReaderType::Pointer  maskReader = MaskReaderType::New();
    maskReader->SetFileName(m_ArgsInfo.targetMask_arg);
    try {
      maskReader->Update();
    } catch ( itk::ExceptionObject & err ) {
      std::cerr << "ExceptionObject caught !" << std::endl;
      std::cerr << err << std::endl;
    }
    if (m_Verbose)std::cout <<"Target image mask was read..." <<std::endl;

    movingMask->SetImage( maskReader->GetOutput() );
  }


  //============================================================================
  // The image pyramids
  //============================================================================
  typedef itk::RecursiveMultiResolutionPyramidImageFilter<InternalImageType,InternalImageType >  FixedImagePyramidType;
  typedef itk::RecursiveMultiResolutionPyramidImageFilter<InternalImageType,InternalImageType >  MovingImagePyramidType;
  typename FixedImagePyramidType::Pointer fixedImagePyramid = FixedImagePyramidType::New();
  typename MovingImagePyramidType::Pointer movingImagePyramid = MovingImagePyramidType::New();
  fixedImagePyramid->SetUseShrinkImageFilter(false);
  fixedImagePyramid->SetInput(fixedImage);
  fixedImagePyramid->SetNumberOfLevels(m_ArgsInfo.levels_arg);
  movingImagePyramid->SetUseShrinkImageFilter(false);
  movingImagePyramid->SetInput(movingImage);
  movingImagePyramid->SetNumberOfLevels(m_ArgsInfo.levels_arg);
  if (m_Verbose) std::cout<<"Creating the image pyramid..."<<std::endl;

  fixedImagePyramid->Update();
  movingImagePyramid->Update();



  //============================================================================
  // We retrieve the type of metric from the command line
  //============================================================================
  typedef clitk::GenericMetric<args_info_clitkAffineRegistration, InternalImageType, InternalImageType> GenericMetricType;
  typename GenericMetricType::Pointer genericMetric=GenericMetricType::New();
  genericMetric->SetArgsInfo(m_ArgsInfo);
  genericMetric->SetFixedImage(fixedImagePyramid->GetOutput(0));
  if (fixedMask) genericMetric->SetFixedImageMask(fixedMask);
  typedef itk::ImageToImageMetric< InternalImageType, InternalImageType >  MetricType;
  typename  MetricType::Pointer metric=genericMetric->GetMetricPointer();
  if (movingMask) metric->SetMovingImageMask(movingMask);

  if (threadsGiven) {
#if ITK_VERSION_MAJOR <= 4
    metric->SetNumberOfThreads( threads );
#else
    metric->SetNumberOfWorkUnits( threads );
#endif
  }

  //============================================================================
  // Initialize using image moments.
  //============================================================================
  if (m_ArgsInfo.moment_flag) {
    typedef itk::ImageMomentsCalculator< InternalImageType > CalculatorType;
    typename CalculatorType::Pointer fixedCalculator= CalculatorType::New();

    typename InternalImageType::Pointer fixedThresh;
    if (m_ArgsInfo.intThreshold_given) {
      typedef itk::ThresholdImageFilter<InternalImageType> ThresholdImageFilterType;
      typename ThresholdImageFilterType::Pointer thresholder = ThresholdImageFilterType::New();
      thresholder->SetInput(fixedImage);
      thresholder->SetLower(m_ArgsInfo.intThreshold_arg);
      thresholder->Update();
      fixedThresh=thresholder->GetOutput();
    } else fixedThresh=fixedImage;

    fixedCalculator->SetImage(fixedThresh);
    fixedCalculator->Compute();
    Vector<double, InputImageType::ImageDimension> fixedCenter=fixedCalculator->GetCenterOfGravity();
    if (m_Verbose)std::cout<<"The fixed center of gravity is "<<fixedCenter<<"..."<<std::endl;

    typedef itk::ImageMomentsCalculator< InternalImageType > CalculatorType;
    typename CalculatorType::Pointer movingCalculator= CalculatorType::New();

    typename InternalImageType::Pointer movingThresh;
    if (m_ArgsInfo.intThreshold_given) {
      typedef itk::ThresholdImageFilter<InternalImageType> ThresholdImageFilterType;
      typename ThresholdImageFilterType::Pointer thresholder = ThresholdImageFilterType::New();
      thresholder->SetInput(movingImage);
      thresholder->SetLower(m_ArgsInfo.intThreshold_arg);
      thresholder->Update();
      movingThresh=thresholder->GetOutput();
    } else movingThresh=movingImage;

    movingCalculator->SetImage(movingThresh);
    movingCalculator->Compute();
    Vector<double, InputImageType::ImageDimension> movingCenter=movingCalculator->GetCenterOfGravity();
    if (m_Verbose)std::cout<<"The moving center of gravity is "<<movingCenter<<"..."<<std::endl;

    Vector<double, InputImageType::ImageDimension> shift= movingCenter-fixedCenter;
    if (m_Verbose)std::cout<<"The initial shift applied is "<<shift<<"..."<<std::endl;

    m_ArgsInfo.transX_arg= shift [0];
    m_ArgsInfo.transY_arg= shift [1];
    if (InputImageType::ImageDimension==3) m_ArgsInfo.transZ_arg=shift [2];
  }

  //============================================================================
  // Transform
  //============================================================================
  typedef clitk::GenericAffineTransform<args_info_clitkAffineRegistration, TCoordRep, InputImageType::ImageDimension > GenericAffineTransformType;
  typename GenericAffineTransformType::Pointer genericAffineTransform = GenericAffineTransformType::New();
  genericAffineTransform->SetArgsInfo(m_ArgsInfo);
  typedef itk::Transform< double, InputImageType::ImageDimension, InputImageType::ImageDimension > TransformType;
  typename TransformType::Pointer transform = genericAffineTransform->GetTransform();
   std::cout<<m_ArgsInfo.transform_arg<<std::endl;


  //=======================================================
  // Interpolator
  //=======================================================
  std::cout<<"setting Interpolator..."<<std::endl;
  typedef clitk::GenericInterpolator<args_info_clitkAffineRegistration, InternalImageType,TCoordRep > GenericInterpolatorType;
  typename GenericInterpolatorType::Pointer genericInterpolator=GenericInterpolatorType::New();
  genericInterpolator->SetArgsInfo(m_ArgsInfo);
  typedef itk::InterpolateImageFunction< InternalImageType, TCoordRep >  InterpolatorType;
  typename  InterpolatorType::Pointer interpolator=genericInterpolator->GetInterpolatorPointer();
  std::cout<<"end of interpolator"<<std::endl;

  //============================================================================
  // Optimizer
  //============================================================================
  typedef clitk::GenericOptimizer<args_info_clitkAffineRegistration> GenericOptimizerType;
  unsigned int nParam = transform->GetNumberOfParameters();
  typename GenericOptimizerType::Pointer genericOptimizer=GenericOptimizerType::New();
  genericOptimizer->SetArgsInfo(m_ArgsInfo);
  genericOptimizer->SetOutputIteration(m_Verbose);
  genericOptimizer->SetOutputPosition(m_Verbose);
  genericOptimizer->SetOutputValue(m_Verbose);
  genericOptimizer->SetOutputGradient(m_ArgsInfo.gradient_flag);
  genericOptimizer->SetMaximize(genericMetric->GetMaximize());
  genericOptimizer->SetNumberOfParameters(nParam);
  typedef itk::SingleValuedNonLinearOptimizer OptimizerType;
  OptimizerType::Pointer optimizer=genericOptimizer->GetOptimizerPointer();

  // Scales
  itk::Optimizer::ScalesType scales( nParam );
  for (unsigned int i=nParam-InputImageType::ImageDimension; i<nParam; i++) //Translations
    scales[i] = m_ArgsInfo.tWeight_arg;
  for (unsigned int i=0; i<nParam-InputImageType::ImageDimension; i++)      //Rest
    scales[i] = m_ArgsInfo.rWeight_arg*180./M_PI;
  optimizer->SetScales(scales);
  //============================================================================
  // Multiresolution registration
  //============================================================================
  std::cout<<"start MultiResolution..."<<std::endl;
  typedef itk::MultiResolutionImageRegistrationMethod< InternalImageType,InternalImageType >  RegistrationType;
  typename  RegistrationType::Pointer registration = RegistrationType::New();
  registration->SetFixedImage( fixedImage  );
  registration->SetFixedImageRegion(fixedImage->GetLargestPossibleRegion());
  registration->SetMovingImage(  movingImage );
  registration->SetFixedImagePyramid( fixedImagePyramid );
  registration->SetMovingImagePyramid( movingImagePyramid );
  registration->SetTransform( transform );
  registration->SetInitialTransformParameters( transform->GetParameters() );
  registration->SetInterpolator( interpolator );
  registration->SetMetric(metric);
  registration->SetOptimizer(optimizer);
  registration->SetNumberOfLevels( m_ArgsInfo.levels_arg );
  if (m_Verbose) std::cout << "Setting "<< m_ArgsInfo.levels_arg <<" resolution levels..." << std::endl;
  if (m_Verbose) std::cout << "Initial Transform: "<< registration->GetInitialTransformParameters()<<std::endl;

  //============================================================================
  // Connecting the commander to the registration to monitor it
  //============================================================================
  if (m_Verbose) {

    // Output iteration info
    CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
    observer->SetOptimizer(genericOptimizer);
    optimizer->AddObserver( itk::IterationEvent(), observer );


    // Output level info
    typedef RegistrationInterfaceCommand<RegistrationType> CommandType;
    typename CommandType::Pointer command = CommandType::New();
    command->SetArgsInfo(m_ArgsInfo);
    registration->AddObserver( itk::IterationEvent(), command );

  }


  //============================================================================
  // Finally we can start the registration with the given amount of multiresolution levels
  //============================================================================
  if (m_Verbose) std::cout << "Starting the registration now..." << std::endl;

  try {
    registration->Update();
  } catch ( itk::ExceptionObject & err ) {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
  }


  //============================================================================
  // Processing the result of the registration
  //============================================================================
  OptimizerType::ParametersType finalParameters = registration->GetLastTransformParameters();
  std::cout<< "Result : " <<std::setprecision(12)<<std::endl;

  for (unsigned int i=nParam-InputImageType::ImageDimension; i<nParam; i++) //Translations
    std::cout << " Translation " << i << " = " << finalParameters[i];
  for (unsigned int i=0; i<nParam-InputImageType::ImageDimension; i++)      //Rest
    std::cout << " Other parameter " << i << " = " << finalParameters[i];

  itk::Matrix<double,InputImageType::ImageDimension+1,InputImageType::ImageDimension+1> matrix;
  if (m_ArgsInfo.transform_arg == 3) {
    for (unsigned int i=0; i<InputImageType::ImageDimension; i++) {
      matrix[i][3] =  finalParameters[nParam-InputImageType::ImageDimension+i];
      for (unsigned int j=0; j<InputImageType::ImageDimension; j++) {
        matrix[i][j] = finalParameters[i*3+j];
      }
      matrix[3][3] = 1.0;
    }
  } else {
    matrix = clitk::GetBackwardAffineMatrix<InputImageType::ImageDimension>(finalParameters);
   std::cout<<"outside GetBackWardAffineMatrix...."<<std::endl;
}

  std::cout << " Affine transform matrix =" << std::endl;
  std::cout << matrix <<std::setprecision(6)<< std::endl;
  std::cout << " End of Registration" << std::endl;
  // Write matrix to a file
  if (m_ArgsInfo.matrix_given) {
    std::ofstream mFile;
    mFile.open(m_ArgsInfo.matrix_arg);
    mFile<<std::setprecision(12)<<matrix<< std::setprecision(6)<<std::endl;
    mFile.close();
  }

  //============================================================================
  // Prepare the resampling filter in order to transform the moving image.
  //============================================================================
 // if (m_ArgsInfo.output_given || m_ArgsInfo.checker_after_given || m_ArgsInfo.after_given ) {
    transform->SetParameters( finalParameters );
    typedef itk::ResampleImageFilter< InternalImageType,InternalImageType >    ResampleFilterType;
    typename    ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    resampler->SetTransform( transform );
    resampler->SetInput( movingImage );
    resampler->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
    resampler->SetOutputOrigin(  fixedImage->GetOrigin() );
    resampler->SetOutputSpacing( fixedImage->GetSpacing() );
    resampler->SetDefaultPixelValue( 0 );
    resampler->Update();
    //Output?
   // if (m_ArgsInfo.output_given) {
      //We write an output in the same pixeltype then the input
      /*typedef itk::ImageFileWriter< FixedImageType >  WriterType;
      typename WriterType::Pointer outputWriter =  WriterType::New();
      outputWriter->SetFileName(m_ArgsInfo.output_arg );
      outputWriter->SetInput( resampler->GetOutput()   );
      outputWriter->Update();*/
     typedef InternalImageType OutputImageType;
     typename OutputImageType::Pointer outputImage = resampler->GetOutput();
     std::cout<<"Writing Output....."<<std::endl;
     this->template SetNextOutput<OutputImageType>(outputImage);
  //  }


    //============================================================================
    // Checker after?
    //============================================================================
    if (m_ArgsInfo.checker_after_given) {
      //To display correctly the checkerboard image, the intensities must lie in the same range (normalized)
      //We write the image in the internal image type
      typedef itk::ResampleImageFilter< InternalImageType,InternalImageType >    ResampleFilterType;
      typename    ResampleFilterType::Pointer internalResampler = ResampleFilterType::New();
      internalResampler->SetTransform( transform );
      internalResampler->SetInput( inputMovingImage );
      internalResampler->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
      internalResampler->SetOutputOrigin(  fixedImage->GetOrigin() );
      internalResampler->SetOutputSpacing( fixedImage->GetSpacing() );
      internalResampler->SetDefaultPixelValue( 0 );

      //We pass the normalized images to the checker filter
      typedef itk::CheckerBoardImageFilter< InternalImageType > CheckerBoardFilterType;
      typename CheckerBoardFilterType::Pointer checkerFilter= CheckerBoardFilterType::New();

      checkerFilter->SetInput1(inputFixedImage);
      checkerFilter->SetInput2(internalResampler->GetOutput());
      typedef itk::ImageFileWriter< InternalImageType >  InternalWriterType;
      typename  InternalWriterType::Pointer checkerWriter =  InternalWriterType::New();
      checkerWriter->SetFileName(m_ArgsInfo.checker_after_arg);
      checkerWriter->SetInput( checkerFilter->GetOutput() );
      checkerWriter->Update();
    }


    //============================================================================
    // Checker before?
    //============================================================================
    if (m_ArgsInfo.checker_before_given) {
      //To display correctly the checkerboard image, the intensities must lie in the same range (normalized)
      //We write the image in the internal image type
      //We pass the normalized images to the checker filter
      typedef itk::CheckerBoardImageFilter< InternalImageType > CheckerBoardFilterType;
      typename CheckerBoardFilterType::Pointer checkerFilter= CheckerBoardFilterType::New();

      checkerFilter->SetInput1(inputFixedImage);
      checkerFilter->SetInput2(inputMovingImage);
      typedef itk::ImageFileWriter< InternalImageType >  InternalWriterType;
      typename  InternalWriterType::Pointer checkerWriter =  InternalWriterType::New();
      checkerWriter->SetFileName(m_ArgsInfo.checker_before_arg);
      checkerWriter->SetInput( checkerFilter->GetOutput() );
      checkerWriter->Update();
    }


    //============================================================================
    // Difference After?
    //============================================================================
    if (m_ArgsInfo.after_given) {
      typedef itk::SubtractImageFilter< InternalImageType, FixedImageType,FixedImageType > DifferenceImageFilterType;
      typename DifferenceImageFilterType::Pointer differenceAfterFilter= DifferenceImageFilterType::New();

      differenceAfterFilter->SetInput1(fixedImage);
      differenceAfterFilter->SetInput2(resampler->GetOutput());

      // Prepare a writer to write the difference image
      typedef itk::ImageFileWriter< FixedImageType >  WriterType;
      typename   WriterType::Pointer     differenceAfterWriter =  WriterType::New();
      differenceAfterWriter->SetFileName(m_ArgsInfo.after_arg );
      differenceAfterWriter->SetInput( differenceAfterFilter->GetOutput()   );
      differenceAfterWriter->Update();
    }
//  }

  //============================================================================
  // Difference Before?
  //============================================================================
  if (m_ArgsInfo.before_given) {
    typedef itk::CastImageFilter< InternalImageType,FixedImageType > CastFilterType;
    typename    CastFilterType::Pointer  caster =  CastFilterType::New();
    caster->SetInput( movingImage );

    typedef itk::SubtractImageFilter< InternalImageType, FixedImageType, FixedImageType > DifferenceImageFilterType;
    typename DifferenceImageFilterType::Pointer differenceBeforeFilter= DifferenceImageFilterType::New();


    differenceBeforeFilter->SetInput1(fixedImage);
    differenceBeforeFilter->SetInput2(caster->GetOutput());

    // Prepare a writer to write the difference image
    typedef itk::ImageFileWriter< FixedImageType >  WriterType;
    typename WriterType::Pointer     differenceBeforeWriter =  WriterType::New();
    differenceBeforeWriter->SetFileName(m_ArgsInfo.before_arg);
    differenceBeforeWriter->SetInput( differenceBeforeFilter->GetOutput()   );
    differenceBeforeWriter->Update();
  }

}

}
//====================================================================

#endif  //#define CLITKAFFINEREGISTRATIONCGENERICFILTER_CXX
