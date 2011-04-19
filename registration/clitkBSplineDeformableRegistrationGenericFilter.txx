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
#ifndef __clitkBSplineDeformableRegistrationGenericFilter_txx
#define __clitkBSplineDeformableRegistrationGenericFilter_txx
#include "clitkBSplineDeformableRegistrationGenericFilter.h"


namespace clitk
{

  //==============================================================================
  //Creating an observer class that allows us to change parameters at subsequent levels
  //==============================================================================
  template <typename TRegistration>
  class RegistrationInterfaceCommand : public itk::Command 
  {
  public:
    typedef RegistrationInterfaceCommand   Self;
  

    typedef itk::Command             Superclass;
    typedef itk::SmartPointer<Self>  Pointer;
    itkNewMacro( Self );
  protected:
    RegistrationInterfaceCommand() {};
  public:
    typedef   TRegistration                              RegistrationType;
    typedef   RegistrationType *                         RegistrationPointer;
  
    // Two arguments are passed to the Execute() method: the first
    // is the pointer to the object which invoked the event and the 
    // second is the event that was invoked.
    void Execute(itk::Object * object, const itk::EventObject & event)
    {
      if( !(itk::IterationEvent().CheckEvent( &event )) )
	{
	  return;
	}
      RegistrationPointer registration = dynamic_cast<RegistrationPointer>( object );
      unsigned int numberOfLevels=registration->GetNumberOfLevels();
      unsigned int currentLevel=registration->GetCurrentLevel()+1;
      std::cout<<std::endl;
      std::cout<<"========================================"<<std::endl;
      std::cout<<"Starting resolution level "<<currentLevel<<" of "<<numberOfLevels<<"..."<<std::endl;
      std::cout<<"========================================"<<std::endl;	
      std::cout<<std::endl;
    }
  
    void Execute(const itk::Object * , const itk::EventObject & )
    { return; }
  
  };


  //==============================================================================
  // Creating an observer class that allows output at each iteration
  //==============================================================================
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
    typedef   clitk::GenericOptimizer<args_info_clitkBSplineDeformableRegistration>     OptimizerType;
    typedef   const OptimizerType   *           OptimizerPointer;
  
    // We set the generic optimizer
    void SetOptimizer(OptimizerPointer o){m_Optimizer=o;}
    
    // Execute
    void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }
  
    void Execute(const itk::Object * object, const itk::EventObject & event)
    {  
      if( !(itk::IterationEvent().CheckEvent( &event )) )
	{
	  return;
	}
      
      m_Optimizer->OutputIterationInfo();
    }

    OptimizerPointer m_Optimizer;
  };


  //==============================================================================
  // Update with the number of dimensions
  //==============================================================================
  template<unsigned int Dimension>
  void BSplineDeformableRegistrationGenericFilter::UpdateWithDim(std::string PixelType)
  {

    if (m_Verbose) std::cout  << "Images were detected to be "<< Dimension << "D and " << PixelType << "..." << std::endl;
    
    if(PixelType == "short"){  
      if (m_Verbose) std::cout  << "Launching warp in "<< Dimension <<"D and signed short..." << std::endl;
      UpdateWithDimAndPixelType<Dimension, signed short>(); 
    }
    //    else if(PixelType == "unsigned_short"){  
    //       if (m_Verbose) std::cout  << "Launching warp in "<< Dimension <<"D and unsigned_short..." << std::endl;
    //       UpdateWithDimAndPixelType<Dimension, unsigned short>(); 
    //     }
    
    //     else if (PixelType == "unsigned_char"){ 
    //       if (m_Verbose) std::cout  << "Launching warp in "<< Dimension <<"D and unsigned_char..." << std::endl;
    //       UpdateWithDimAndPixelType<Dimension, unsigned char>();
    //     }
    
    //     else if (PixelType == "char"){ 
    //       if (m_Verbose) std::cout  << "Launching warp in "<< Dimension <<"D and signed_char..." << std::endl;
    //       UpdateWithDimAndPixelType<Dimension, signed char>();
    //    }
    else {
      if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
      UpdateWithDimAndPixelType<Dimension, float>();
    }
  }

  

  //==============================================================================
  // Update with the number of dimensions and pixeltype
  //==============================================================================
  template<unsigned int ImageDimension, class PixelType>
  void BSplineDeformableRegistrationGenericFilter::UpdateWithDimAndPixelType()
  {
      
    //=======================================================
    // Run-time
    //=======================================================
    bool threadsGiven=m_ArgsInfo.threads_given;
    int threads=m_ArgsInfo.threads_arg;
    
    typedef itk::Image< PixelType, ImageDimension >  FixedImageType;
    typedef itk::Image< PixelType, ImageDimension >  MovingImageType;
    const unsigned int SpaceDimension = ImageDimension;
    typedef double TCoordRep;


    //=======================================================
    //Input
    //=======================================================
    typedef itk::ImageFileReader< FixedImageType  > FixedImageReaderType;
    typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;

    typename FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
    typename MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

    fixedImageReader->SetFileName(  m_ArgsInfo.reference_arg );
    movingImageReader->SetFileName( m_ArgsInfo.target_arg );
    if (m_Verbose) std::cout<<"Reading images..."<<std::endl;
    fixedImageReader->Update();
    movingImageReader->Update();

    typename FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();
    typename MovingImageType::Pointer movingImage =movingImageReader->GetOutput();
    typename FixedImageType::RegionType fixedImageRegion = fixedImage->GetLargestPossibleRegion();

    // The metric region: where should the metric be CALCULATED (depends on mask)
    typename FixedImageType::RegionType metricRegion = fixedImage->GetLargestPossibleRegion();
    typename FixedImageType::RegionType::SizeType metricRegionSize=metricRegion.GetSize();
    typename FixedImageType::RegionType::IndexType metricRegionIndex=metricRegion.GetIndex();
    typename FixedImageType::PointType metricRegionOrigin=fixedImage->GetOrigin();
    
    // The transform region: where should the transform be DEFINED (depends on mask)
    typename FixedImageType::RegionType transformRegion = fixedImage->GetLargestPossibleRegion();
    typename FixedImageType::RegionType::SizeType transformRegionSize=transformRegion.GetSize();
    typename FixedImageType::RegionType::IndexType transformRegionIndex=transformRegion.GetIndex();
    typename FixedImageType::PointType transformRegionOrigin=fixedImage->GetOrigin();


    //=======================================================
    // If given, we connect a mask to the fixed image
    //======================================================
    typedef itk::ImageMaskSpatialObject<  ImageDimension >   MaskType;
    typename MaskType::Pointer  spatialObjectMask=NULL;
   
    if (m_ArgsInfo.mask_given)
      {
	typedef itk::Image< unsigned char, ImageDimension >   ImageMaskType;
	typedef itk::ImageFileReader< ImageMaskType >    MaskReaderType;
	typename MaskReaderType::Pointer  maskReader = MaskReaderType::New();
	maskReader->SetFileName(m_ArgsInfo.mask_arg);
	
	try 
	  { 
	    maskReader->Update(); 
	  } 
	catch( itk::ExceptionObject & err ) 
	  { 
	    std::cerr << "ExceptionObject caught while reading mask !" << std::endl; 
	    std::cerr << err << std::endl; 
	    return;
	  } 
	if (m_Verbose)std::cout <<"Reference image mask was read..." <<std::endl;


	// Set the image to the spatialObject
	spatialObjectMask = MaskType::New();
	spatialObjectMask->SetImage( maskReader->GetOutput() );

	// Find the bounding box of the "inside" label
	typedef itk::LabelStatisticsImageFilter<ImageMaskType, ImageMaskType> StatisticsImageFilterType;
	typename StatisticsImageFilterType::Pointer statisticsImageFilter=StatisticsImageFilterType::New();
	statisticsImageFilter->SetInput(maskReader->GetOutput());
	statisticsImageFilter->SetLabelInput(maskReader->GetOutput());
	statisticsImageFilter->Update();
	typename StatisticsImageFilterType::BoundingBoxType boundingBox = statisticsImageFilter->GetBoundingBox(1);

	// Limit the transform region to the mask
	for (unsigned int i=0; i<ImageDimension; i++)
	  {
	    transformRegionIndex[i]=boundingBox[2*i];
	    transformRegionSize[i]=boundingBox[2*i+1]-boundingBox[2*i]+1;
	  }
	transformRegion.SetSize(transformRegionSize);
	transformRegion.SetIndex(transformRegionIndex);
	fixedImage->TransformIndexToPhysicalPoint(transformRegion.GetIndex(), transformRegionOrigin);
	
	// Limit the metric region to the mask
	metricRegion=transformRegion;
	fixedImage->TransformIndexToPhysicalPoint(metricRegion.GetIndex(), metricRegionOrigin);
 
     }



    //=======================================================
    // Regions
    //=====================================================
    if (m_Verbose)
      {
	// Fixed image region
	std::cout<<"The fixed image has its origin at "<<fixedImage->GetOrigin()<<std::endl 
		 <<"The fixed image region starts at index "<<fixedImageRegion.GetIndex()<<std::endl
		 <<"The fixed image region has size "<< fixedImageRegion.GetSize()<<std::endl;

	// Transform region
	std::cout<<"The transform has its origin at "<<transformRegionOrigin<<std::endl 
		 <<"The transform region will start at index "<<transformRegion.GetIndex()<<std::endl
		 <<"The transform region has size "<< transformRegion.GetSize()<<std::endl;

	// Metric region
	std::cout<<"The metric region has its origin at "<<metricRegionOrigin<<std::endl 
		 <<"The metric region will start at index "<<metricRegion.GetIndex()<<std::endl
		 <<"The metric region has size "<< metricRegion.GetSize()<<std::endl;
	
      }  

    
    //=======================================================
    //Pyramids
    //=======================================================
    typedef itk::RecursiveMultiResolutionPyramidImageFilter< FixedImageType, FixedImageType>    FixedImagePyramidType;
    typedef itk::RecursiveMultiResolutionPyramidImageFilter< MovingImageType, MovingImageType>    MovingImagePyramidType;
    typename FixedImagePyramidType::Pointer fixedImagePyramid = FixedImagePyramidType::New();
    typename MovingImagePyramidType::Pointer movingImagePyramid = MovingImagePyramidType::New();
    

    //     //=======================================================
    //     // Rigid Transform
    //     //=======================================================
    //     typedef itk::Euler3DTransform <double> RigidTransformType;
    //     RigidTransformType::Pointer rigidTransform=RigidTransformType::New();
    
    //     if (m_ArgsInfo.rigid_given)
    //       {
    //     	itk::Matrix<double,4,4> rigidTransformMatrix=clitk::ReadMatrix3D(m_ArgsInfo.rigid_arg);
    
    //     	//Set the rotation
    //     	itk::Matrix<double,3,3> finalRotation = clitk::GetRotationalPartMatrix3D(rigidTransformMatrix);
    //     	rigidTransform->SetMatrix(finalRotation);
    
    //     	//Set the translation
    //     	itk::Vector<double,3> finalTranslation = clitk::GetTranslationPartMatrix3D(rigidTransformMatrix);
    //     	rigidTransform->SetTranslation(finalTranslation);
    
    //       }


    //=======================================================
    // BSpline Transform
    //=======================================================
    typename FixedImageType::RegionType::SizeType splineOrders ;
   
    //Default is cubic splines
    splineOrders.Fill(3);
    if (m_ArgsInfo.order_given)
      for(unsigned int i=0; i<ImageDimension;i++) 
	splineOrders[i]=m_ArgsInfo.order_arg[i];

    // BLUT or ITK FFD
    typedef itk::Transform<TCoordRep, ImageDimension, SpaceDimension> TransformType;
    typename TransformType::Pointer transform;
    typedef  itk::BSplineDeformableTransform<TCoordRep,SpaceDimension, 3> BSplineTransformType;
    typedef  BSplineTransformType* BSplineTransformPointer;
    typedef  clitk::BSplineDeformableTransform<TCoordRep,ImageDimension, SpaceDimension > BLUTTransformType;
    typedef  BLUTTransformType* BLUTTransformPointer;

    // JV parameter array is passed by reference, create outside context so it exists afterwards!!!!!
    typedef typename TransformType::ParametersType     ParametersType;
    ParametersType parameters;


    // CLITK BLUT transform
    if(m_ArgsInfo.wlut_flag)
      {
	typename BLUTTransformType::Pointer  bsplineTransform = BLUTTransformType::New();
	if (m_Verbose) std::cout<<"Setting the spline orders  to "<<splineOrders<<"..."<<std::endl;
	bsplineTransform->SetSplineOrders(splineOrders);
      
	//-------------------------------------------------------------------------
	// Define the region: Either the spacing or the number of CP should be given 
	//-------------------------------------------------------------------------
	
	// Region
	typedef typename BSplineTransformType::RegionType RegionType;
	RegionType bsplineRegion;
	typename RegionType::SizeType   gridSizeOnImage;
	typename RegionType::SizeType   gridBorderSize;
	typename RegionType::SizeType   totalGridSize;
      
	// Spacing
	typedef typename BSplineTransformType::SpacingType SpacingType;
	SpacingType fixedImageSpacing, chosenSpacing, adaptedSpacing;
	fixedImageSpacing = fixedImage->GetSpacing();

	// Only spacing given: adjust if necessary
	if (m_ArgsInfo.spacing_given && !m_ArgsInfo.control_given)
	  {
	    for(unsigned int r=0; r<ImageDimension; r++) 
	      {
		chosenSpacing[r]= m_ArgsInfo.spacing_arg[r];
		gridSizeOnImage[r] = ceil( (double) transformRegion.GetSize()[r] / ( round(chosenSpacing[r]/fixedImageSpacing[r]) ) );
		adaptedSpacing[r]= ( round(chosenSpacing[r]/fixedImageSpacing[r]) *fixedImageSpacing[r] ) ;
	      }
	    if (m_Verbose) std::cout<<"The chosen control point spacing "<<chosenSpacing<<"..."<<std::endl;
	    if (m_Verbose) std::cout<<"The control points spacing was adapted to "<<adaptedSpacing<<"..."<<std::endl; 
	    if (m_Verbose) std::cout<<"The number of (internal) control points is "<<gridSizeOnImage<<"..."<<std::endl; 
	  }

	// Only number of CP given: adjust if necessary
	else if (m_ArgsInfo.control_given && !m_ArgsInfo.spacing_given)
	  {
	    for(unsigned int r=0; r<ImageDimension; r++) 
	      {
		gridSizeOnImage[r]= m_ArgsInfo.control_arg[r];
		chosenSpacing[r]=fixedImageSpacing[r]*( (double)(transformRegion.GetSize()[r])  / 
							(double)(gridSizeOnImage[r]) );
		adaptedSpacing[r]= fixedImageSpacing[r]* ceil( (double)(transformRegion.GetSize()[r] - 1)  / 
							       (double)(gridSizeOnImage[r] - 1) );
	      }
	    if (m_Verbose) std::cout<<"The chosen control point spacing "<<chosenSpacing<<"..."<<std::endl;
	    if (m_Verbose) std::cout<<"The control points spacing was adapted to "<<adaptedSpacing<<"..."<<std::endl; 
	    if (m_Verbose) std::cout<<"The number of (internal) control points is "<<gridSizeOnImage<<"..."<<std::endl; 
	  }
	
	// Spacing and number of CP given: no adjustment adjust, just warnings
	else if (m_ArgsInfo.control_given && m_ArgsInfo.spacing_given)
	  {
	    for(unsigned int r=0; r<ImageDimension; r++) 
	      {
		adaptedSpacing[r]= m_ArgsInfo.spacing_arg[r];
		gridSizeOnImage[r] =  m_ArgsInfo.control_arg[r];
		if (gridSizeOnImage[r]*adaptedSpacing[r]< transformRegion.GetSize()[r]*fixedImageSpacing[r]) 
		  {
		    std::cout<<"WARNING: Specified control point region ("<<gridSizeOnImage[r]*adaptedSpacing[r]
			     <<"mm) does not cover the transform region ("<< transformRegion.GetSize()[r]*fixedImageSpacing[r]
			     <<"mm) for dimension "<<r<<"!" <<std::endl
			     <<"Specify only --spacing or --control for automatic adjustment..."<<std::endl;
		  }
		if (  fmod(adaptedSpacing[r], fixedImageSpacing[r]) ) 
		  {
		    std::cout<<"WARNING: Specified control point spacing for dimension "<<r
			     <<" does not allow exact representation of BLUT FFD!"<<std::endl
			     <<"Spacing ratio is non-integer: "<<adaptedSpacing[r]/ fixedImageSpacing[r]<<std::endl
			     <<"Specify only --spacing or --control for automatic adjustment..."<<std::endl;  
		  }
	      }
	    if (m_Verbose) std::cout<<"The control points spacing was set to "<<adaptedSpacing<<"..."<<std::endl; 
	    if (m_Verbose) std::cout<<"The number of (internal) control points spacing is "<<gridSizeOnImage<<"..."<<std::endl; 
	  }

	//JV  border size should depend on spline order
	for(unsigned int r=0; r<ImageDimension; r++) gridBorderSize[r]=splineOrders[r]; // Border for spline order = 3 ( 1 lower, 2 upper )
	totalGridSize = gridSizeOnImage + gridBorderSize;
	bsplineRegion.SetSize( totalGridSize );
	if (m_Verbose) std::cout<<"The total control point grid size was set to "<<totalGridSize<<"..."<<std::endl;

	// Direction
	typename FixedImageType::DirectionType gridDirection = fixedImage->GetDirection();
	SpacingType gridOriginOffset = gridDirection * adaptedSpacing;

	// Origin: 1 CP border for spatial dimensions 
	typedef typename BSplineTransformType::OriginType OriginType;
	OriginType gridOrigin = transformRegionOrigin - gridOriginOffset;
	if (m_Verbose) std::cout<<"The control point grid origin was set to "<<gridOrigin<<"..."<<std::endl;

	// Set 
	bsplineTransform->SetGridSpacing( adaptedSpacing );
	bsplineTransform->SetGridOrigin( gridOrigin );
	bsplineTransform->SetGridRegion( bsplineRegion );
	bsplineTransform->SetGridDirection( gridDirection );

	//Bulk transform
	//if (m_Verbose) std::cout<<"Setting rigid transform..."<<std::endl;
	//bsplineTransform->SetBulkTransform( rigidTransform );
  
	//Vector BSpline interpolator
	//bsplineTransform->SetOutputSpacing(fixedImage->GetSpacing());
	typename RegionType::SizeType samplingFactors;
	for (unsigned int i=0; i< ImageDimension; i++)
	  {
	    if (m_Verbose) std::cout<<"For dimension "<<i<<", the ideal sampling factor (if integer) is a multitude of "
				    << (double)adaptedSpacing[i]/ (double) fixedImageSpacing[i]<<"..."<<std::endl;
	    if (m_ArgsInfo.samplingFactor_given) samplingFactors[i]=m_ArgsInfo.samplingFactor_arg[i];
	    else samplingFactors[i]=(int) ((double)adaptedSpacing[i]/ (double) movingImage->GetSpacing()[i]);
	    if (m_Verbose) std::cout<<"Setting sampling factor "<<i<<" to "<<samplingFactors[i]<<"..."<<std::endl;
	  }
	bsplineTransform->SetLUTSamplingFactors(samplingFactors);

	//initial parameters
	if (m_ArgsInfo.init_given)
	  {
	    typedef itk::ImageFileReader<typename BLUTTransformType::CoefficientImageType> CoefficientReaderType;
	    typename CoefficientReaderType::Pointer coeffReader=CoefficientReaderType::New();
	    coeffReader->SetFileName(m_ArgsInfo.init_arg[0]);
	    coeffReader->Update();
	    bsplineTransform->SetCoefficientImage(coeffReader->GetOutput());
	  }
	else
	  {
	    //typedef typename TransformType::ParametersType     ParametersType;
	    const unsigned int numberOfParameters =    bsplineTransform->GetNumberOfParameters();
	    parameters=ParametersType( numberOfParameters );
	    parameters.Fill( 0.0 );
	    bsplineTransform->SetParameters( parameters );
	  }

	// Mask
	if (spatialObjectMask) bsplineTransform->SetMask( spatialObjectMask );

	// Pass
	transform=bsplineTransform;
      }

    //ITK BSpline transform
    else
      {
	typename BSplineTransformType::Pointer  bsplineTransform = BSplineTransformType::New();

	// Define the region
	typedef typename BSplineTransformType::RegionType RegionType;
	RegionType bsplineRegion;
	typename RegionType::SizeType   gridSizeOnImage;
	typename RegionType::SizeType   gridBorderSize;
	typename RegionType::SizeType   totalGridSize;

	//Set the number of control points
	for(unsigned int r=0; r<ImageDimension; r++)  gridSizeOnImage[r]=m_ArgsInfo.control_arg[r];
	if (m_Verbose) std::cout<<"Setting the number of internal control points "<<gridSizeOnImage<<"..."<<std::endl;
	gridBorderSize.Fill( 3 );    // Border for spline order = 3 ( 1 lower, 2 upper )
	totalGridSize = gridSizeOnImage + gridBorderSize;
	bsplineRegion.SetSize( totalGridSize );

	// Spacing
	typedef typename BSplineTransformType::SpacingType SpacingType;
	SpacingType spacing = fixedImage->GetSpacing();
	typename FixedImageType::SizeType fixedImageSize = fixedImageRegion.GetSize();
	if (m_ArgsInfo.spacing_given)
	  {
	    
	    for(unsigned int r=0; r<ImageDimension; r++)
	      {
		spacing[r] =m_ArgsInfo.spacing_arg[r];
	      }
	  }
	else
	  {
	    for(unsigned int r=0; r<ImageDimension; r++)
	      {
		spacing[r] *= static_cast<double>(fixedImageSize[r] - 1)  / 
		  static_cast<double>(gridSizeOnImage[r] - 1);
	      }
	  }
	if (m_Verbose) std::cout<<"The control points spacing was set to "<<spacing<<"..."<<std::endl;

	// Direction
	typename FixedImageType::DirectionType gridDirection = fixedImage->GetDirection();
	SpacingType gridOriginOffset = gridDirection * spacing;

	// Origin
	typedef typename BSplineTransformType::OriginType OriginType;
	OriginType origin = fixedImage->GetOrigin();
	OriginType gridOrigin = origin - gridOriginOffset; 

	// Set
	bsplineTransform->SetGridSpacing( spacing );
	bsplineTransform->SetGridOrigin( gridOrigin );
	bsplineTransform->SetGridRegion( bsplineRegion );
	bsplineTransform->SetGridDirection( gridDirection );

	// Bulk transform
	// if (m_Verbose) std::cout<<"Setting rigid transform..."<<std::endl;
	// bsplineTransform->SetBulkTransform( rigidTransform );

	// Initial parameters
	if (m_ArgsInfo.init_given)
	  {
	    typedef itk::ImageFileReader<typename BSplineTransformType::ImageType> CoefficientReaderType;
	    typename BSplineTransformType::ImageType::Pointer coeffImages[SpaceDimension];
	    for(unsigned int i=0; i<SpaceDimension; i++)
	      {
		typename CoefficientReaderType::Pointer coeffReader=CoefficientReaderType::New();
		coeffReader->SetFileName(m_ArgsInfo.init_arg[i]);
		coeffReader->Update();
		coeffImages[i]=coeffReader->GetOutput();
	      }
	    bsplineTransform->SetCoefficientImage(coeffImages);
	  }
	else
	  {
	    const unsigned int numberOfParameters =    bsplineTransform->GetNumberOfParameters();
	    parameters=ParametersType( numberOfParameters );
	    parameters.Fill( 0.0 );
	    bsplineTransform->SetParameters( parameters );
	  }

	// Pass
	transform=bsplineTransform;

      }
  

    //=======================================================
    // Interpolator
    //=======================================================
    typedef clitk::GenericInterpolator<args_info_clitkBSplineDeformableRegistration, FixedImageType, TCoordRep > GenericInterpolatorType;
    typename   GenericInterpolatorType::Pointer genericInterpolator=GenericInterpolatorType::New();
    genericInterpolator->SetArgsInfo(m_ArgsInfo);
    typedef itk::InterpolateImageFunction< FixedImageType, TCoordRep >  InterpolatorType;
    typename  InterpolatorType::Pointer interpolator=genericInterpolator->GetInterpolatorPointer();


    //=======================================================
    // Metric
    //=======================================================
    typedef clitk::GenericMetric<args_info_clitkBSplineDeformableRegistration, FixedImageType,MovingImageType > GenericMetricType;
    typename GenericMetricType::Pointer genericMetric=GenericMetricType::New();
    genericMetric->SetArgsInfo(m_ArgsInfo);
    genericMetric->SetFixedImageRegion(metricRegion);
    typedef itk::ImageToImageMetric< FixedImageType, MovingImageType >  MetricType;
    typename  MetricType::Pointer metric=genericMetric->GetMetricPointer();
    if (spatialObjectMask) metric->SetFixedImageMask( spatialObjectMask );

#ifdef ITK_USE_OPTIMIZED_REGISTRATION_METHODS
    if (threadsGiven) metric->SetNumberOfThreads( threads );
#else
    if (m_Verbose) std::cout<<"Not setting the number of threads (not compiled with USE_OPTIMIZED_REGISTRATION_METHODS)..."<<std::endl;
#endif


    //=======================================================
    // Optimizer
    //=======================================================
    typedef clitk::GenericOptimizer<args_info_clitkBSplineDeformableRegistration> GenericOptimizerType;
    GenericOptimizerType::Pointer genericOptimizer = GenericOptimizerType::New();
    genericOptimizer->SetArgsInfo(m_ArgsInfo);
    genericOptimizer->SetMaximize(genericMetric->GetMaximize());
    genericOptimizer->SetNumberOfParameters(transform->GetNumberOfParameters());
    typedef itk::SingleValuedNonLinearOptimizer OptimizerType;
    OptimizerType::Pointer optimizer = genericOptimizer->GetOptimizerPointer();
    
    
    //=======================================================
    // Registration
    //=======================================================
    typedef itk::MultiResolutionImageRegistrationMethod<  FixedImageType, MovingImageType >    RegistrationType;
    typename RegistrationType::Pointer   registration  = RegistrationType::New();
    registration->SetMetric(        metric        );
    registration->SetOptimizer(     optimizer     );
    registration->SetInterpolator(  interpolator  );
    registration->SetTransform (transform);
    if(threadsGiven) registration->SetNumberOfThreads(threads);
    registration->SetFixedImage(  fixedImage   );
    registration->SetMovingImage(   movingImage   );
    registration->SetFixedImageRegion( metricRegion );
    registration->SetFixedImagePyramid( fixedImagePyramid );
    registration->SetMovingImagePyramid( movingImagePyramid );
    registration->SetInitialTransformParameters( transform->GetParameters() );
    registration->SetNumberOfLevels(m_ArgsInfo.levels_arg);
    if (m_Verbose) std::cout<<"Setting the number of resolution levels to "<<m_ArgsInfo.levels_arg<<"..."<<std::endl;
       
  
    //================================================================================================
    // Observers
    //================================================================================================
    if (m_Verbose)
      {
	// Output iteration info
	CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
	observer->SetOptimizer(genericOptimizer);
	optimizer->AddObserver( itk::IterationEvent(), observer );
	
	
	// Output level info
	typedef RegistrationInterfaceCommand<RegistrationType> CommandType;
	typename CommandType::Pointer command = CommandType::New();
	registration->AddObserver( itk::IterationEvent(), command );
      }


    //=======================================================
    // Let's go
    //=======================================================
    if (m_Verbose) std::cout << std::endl << "Starting Registration" << std::endl;

    try 
      { 
	registration->StartRegistration(); 
      } 
    catch( itk::ExceptionObject & err ) 
      { 
	std::cerr << "ExceptionObject caught while registering!" << std::endl; 
	std::cerr << err << std::endl; 
	return;
      } 
  

    //=======================================================
    // Get the result
    //=======================================================
    OptimizerType::ParametersType finalParameters =  registration->GetLastTransformParameters();
    transform->SetParameters( finalParameters );

 
    //=======================================================
    // Get the BSpline coefficient images and write them
    //=======================================================
    if (m_ArgsInfo.coeff_given)
      { 
	if(m_ArgsInfo.wlut_flag)
	  {
	    BLUTTransformPointer bsplineTransform=dynamic_cast<BLUTTransformPointer>(registration->GetTransform());
	    typedef  itk::Image<itk::Vector<TCoordRep, SpaceDimension>, ImageDimension> CoefficientImageType;
	    typename CoefficientImageType::Pointer coefficientImage =bsplineTransform->GetCoefficientImage();
	    typedef itk::ImageFileWriter<CoefficientImageType> CoeffWriterType;
	    typename CoeffWriterType::Pointer coeffWriter=CoeffWriterType::New();
	    coeffWriter->SetInput(coefficientImage);
	    coeffWriter->SetFileName(m_ArgsInfo.coeff_arg[0]);
	    coeffWriter->Update();
	  }
	else
	  {
	    BSplineTransformPointer bsplineTransform=dynamic_cast<BSplineTransformPointer>(registration->GetTransform());
	    typedef  itk::Image<TCoordRep, ImageDimension> CoefficientImageType;
#if ITK_VERSION_MAJOR > 3
            typename BSplineTransformType::CoefficientImageArray coefficientImages = bsplineTransform->GetCoefficientImage();
#else
	    typename CoefficientImageType::Pointer *coefficientImages =bsplineTransform->GetCoefficientImage();
#endif
	    typedef itk::ImageFileWriter<CoefficientImageType> CoeffWriterType;
	    for (unsigned int i=0;i<SpaceDimension; i ++)
	      {
		typename CoeffWriterType::Pointer coeffWriter=CoeffWriterType::New();
		coeffWriter->SetInput(coefficientImages[i]);
		coeffWriter->SetFileName(m_ArgsInfo.coeff_arg[i]);
		coeffWriter->Update();
	      }
	  }
      }
  
  
    //=======================================================
    // Generate the DVF
    //=======================================================
    typedef itk::Vector< float, SpaceDimension >  DisplacementType;
    typedef itk::Image< DisplacementType, ImageDimension >  DeformationFieldType;
  
    typename DeformationFieldType::Pointer field = DeformationFieldType::New();
    field->SetRegions( fixedImageRegion );
    field->SetOrigin( fixedImage->GetOrigin() );
    field->SetSpacing( fixedImage->GetSpacing() );
    field->SetDirection( fixedImage->GetDirection() );
    field->Allocate();
  
    typedef itk::ImageRegionIteratorWithIndex< DeformationFieldType > FieldIterator;
    FieldIterator fi( field, fixedImageRegion );
    fi.GoToBegin();
  
    typename TransformType::InputPointType  fixedPoint;
    typename TransformType::OutputPointType movingPoint;
    typename DeformationFieldType::IndexType index;
  
    DisplacementType displacement;
    while( ! fi.IsAtEnd() )
      {
	index = fi.GetIndex();
	field->TransformIndexToPhysicalPoint( index, fixedPoint );
	movingPoint = transform->TransformPoint( fixedPoint );
	displacement = movingPoint - fixedPoint;
	fi.Set( displacement );
	++fi;
      }

 
    //=======================================================
    // Write the DVF
    //=======================================================
    typedef itk::ImageFileWriter< DeformationFieldType >  FieldWriterType;
    typename FieldWriterType::Pointer fieldWriter = FieldWriterType::New();
    fieldWriter->SetFileName( m_ArgsInfo.vf_arg );
    fieldWriter->SetInput( field );
    try
      {
	fieldWriter->Update();
      }
    catch( itk::ExceptionObject & excp )
      {
	std::cerr << "Exception thrown writing the DVF" << std::endl;
	std::cerr << excp << std::endl;
	return;
      }
  
  
    //=======================================================
    // Resample the moving image
    //=======================================================
    typedef itk::WarpImageFilter< MovingImageType, FixedImageType, DeformationFieldType >    WarpFilterType;
    typename WarpFilterType::Pointer warp = WarpFilterType::New();

    warp->SetDeformationField( field );
    warp->SetInput( movingImageReader->GetOutput() );
    warp->SetOutputOrigin(  fixedImage->GetOrigin() );
    warp->SetOutputSpacing( fixedImage->GetSpacing() );
    warp->SetOutputDirection( fixedImage->GetDirection() );
    warp->SetEdgePaddingValue( 0.0 );
    warp->Update();
 

    //=======================================================
    // Write the warped image
    //=======================================================
    typedef itk::ImageFileWriter< FixedImageType >  WriterType;
    typename WriterType::Pointer      writer =  WriterType::New();
    writer->SetFileName( m_ArgsInfo.output_arg );
    writer->SetInput( warp->GetOutput()    );

    try
      {
	writer->Update();
      }
    catch( itk::ExceptionObject & err ) 
      { 
	std::cerr << "ExceptionObject caught !" << std::endl; 
	std::cerr << err << std::endl; 
	return;
      } 
 

    //=======================================================
    // Calculate the difference after the deformable transform
    //=======================================================
    typedef clitk::DifferenceImageFilter<  FixedImageType, FixedImageType> DifferenceFilterType;
    if (m_ArgsInfo.after_given)
      {
	typename DifferenceFilterType::Pointer difference = DifferenceFilterType::New();
	difference->SetValidInput( fixedImage );
	difference->SetTestInput( warp->GetOutput() );
      
	try
	  {
	    difference->Update();
	  }
	catch( itk::ExceptionObject & err ) 
	  { 
	    std::cerr << "ExceptionObject caught calculating the difference !" << std::endl; 
	    std::cerr << err << std::endl; 
	    return;
	  }
      
	typename WriterType::Pointer differenceWriter=WriterType::New();
	differenceWriter->SetInput(difference->GetOutput());
	differenceWriter->SetFileName(m_ArgsInfo.after_arg);
	differenceWriter->Update(); 
      
      }


    //=======================================================
    // Calculate the difference before the deformable transform
    //=======================================================
    if( m_ArgsInfo.before_given )
      {

	typename FixedImageType::Pointer moving=FixedImageType::New();
	if (m_ArgsInfo.rigid_given)
	  {
	    typedef itk::ResampleImageFilter<MovingImageType, FixedImageType> ResamplerType;
	    typename ResamplerType::Pointer resampler=ResamplerType::New();
	    resampler->SetInput(movingImage);
	    resampler->SetOutputOrigin(fixedImage->GetOrigin());
	    resampler->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
	    resampler->SetOutputSpacing(fixedImage->GetSpacing());  
	    resampler->SetDefaultPixelValue( 0. );
	    //resampler->SetTransform(rigidTransform);
	    resampler->Update();
	    moving=resampler->GetOutput();
	  }
	else
	  moving=movingImage;

	typename DifferenceFilterType::Pointer difference = DifferenceFilterType::New();
	difference->SetValidInput( fixedImage );
	difference->SetTestInput( moving );
    
	try
	  {
	    difference->Update();
	  }
	catch( itk::ExceptionObject & err ) 
	  { 
	    std::cerr << "ExceptionObject caught calculating the difference !" << std::endl; 
	    std::cerr << err << std::endl; 
	    return;
	  }

	typename WriterType::Pointer differenceWriter=WriterType::New();
	writer->SetFileName( m_ArgsInfo.before_arg  );
	writer->SetInput( difference->GetOutput()  );
	writer->Update( );
      }

    return;
  }
}

#endif // __clitkBSplineDeformableRegistrationGenericFilter_txx
