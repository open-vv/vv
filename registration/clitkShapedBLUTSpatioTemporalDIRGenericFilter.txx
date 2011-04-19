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
#ifndef clitkShapedBLUTSpatioTemporalDIRGenericFilter_txx
#define clitkShapedBLUTSpatioTemporalDIRGenericFilter_txx

/* =================================================
 * @file   clitkShapedBLUTSpatioTemporalDIRGenericFilter.txx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/

#include "clitkShapedBLUTSpatioTemporalDIRGenericFilter.h"

namespace clitk
{

  //==============================================================================
  // Creating an observer class that allows output at each iteration
  //==============================================================================
  template <typename TRegistration, class args_info_clitkShapedBLUTSpatioTemporalDIR>
  class CommandIterationUpdate : public itk::Command 
  {
  public:
    typedef  CommandIterationUpdate   Self;
    typedef  itk::Command             Superclass;
    typedef  itk::SmartPointer<Self>  Pointer;
    itkNewMacro( Self );

    // Registration
    typedef   TRegistration                              RegistrationType;
    typedef   RegistrationType *                         RegistrationPointer;

    // Transform
    typedef typename RegistrationType::FixedImageType FixedImageType;
    typedef typename FixedImageType::RegionType RegionType;
    itkStaticConstMacro(ImageDimension, unsigned int,FixedImageType::ImageDimension); 
    typedef clitk::ShapedBLUTSpatioTemporalDeformableTransform<double, ImageDimension, ImageDimension> TransformType;
    typedef clitk::ShapedBLUTSpatioTemporalDeformableTransformInitializer<TransformType, FixedImageType> InitializerType;
    typedef typename InitializerType::CoefficientImageType CoefficientImageType;
    typedef itk::CastImageFilter<CoefficientImageType, CoefficientImageType> CastImageFilterType;
    typedef typename TransformType::ParametersType ParametersType;
    typedef typename InitializerType::Pointer InitializerPointer;

  protected:
    CommandIterationUpdate() { m_IterationCounter=0;}
  public:
    typedef   clitk::GenericOptimizer<args_info_clitkShapedBLUTSpatioTemporalDIR>     OptimizerType;
    typedef   const OptimizerType   *           OptimizerPointer;
  
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
      
      // Output
      m_Optimizer->OutputIterationInfo();
      m_IterationCounter++;

      // Write intermediate result
      if (m_ArgsInfo.current_given && (m_IterationCounter> m_ArgsInfo.current_arg) )
	{
	  // Write && Reset
	  writeImage<CoefficientImageType>(m_Initializer->GetTransform()->GetCoefficientImage(), m_ArgsInfo.coeff_arg, m_ArgsInfo.verbose_flag );
	  m_IterationCounter=0;
	}
    }

   // Members
    void SetOptimizer(OptimizerPointer o){m_Optimizer=o;}
    OptimizerPointer m_Optimizer;

    void SetInitializer(InitializerPointer i){m_Initializer=i;}
    InitializerPointer m_Initializer;
    
    void SetArgsInfo(args_info_clitkShapedBLUTSpatioTemporalDIR a){m_ArgsInfo=a;}
    args_info_clitkShapedBLUTSpatioTemporalDIR m_ArgsInfo;

    int m_IterationCounter;
  };


  //==============================================================================
  //Creating an observer class that allows us to change parameters at subsequent levels
  //==============================================================================
  template <typename TRegistration, class args_info_clitkShapedBLUTSpatioTemporalDIR>
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

    // Registration
    typedef   TRegistration                              RegistrationType;
    typedef   RegistrationType *                         RegistrationPointer;

    // Transform
    typedef typename RegistrationType::FixedImageType FixedImageType;
    typedef typename FixedImageType::RegionType RegionType;
    itkStaticConstMacro(ImageDimension, unsigned int,FixedImageType::ImageDimension); 
    typedef clitk::ShapedBLUTSpatioTemporalDeformableTransform<double, ImageDimension, ImageDimension> TransformType;
    typedef clitk::ShapedBLUTSpatioTemporalDeformableTransformInitializer<TransformType, FixedImageType> InitializerType;
    typedef typename InitializerType::CoefficientImageType CoefficientImageType;
    typedef itk::CastImageFilter<CoefficientImageType, CoefficientImageType> CastImageFilterType;
    typedef typename TransformType::ParametersType ParametersType;
    typedef typename InitializerType::Pointer InitializerPointer;
    typedef  typename  CommandIterationUpdate< TRegistration, args_info_clitkShapedBLUTSpatioTemporalDIR>::Pointer CommandIterationUpdatePointer;

    // Optimizer
    typedef clitk::GenericOptimizer<args_info_clitkShapedBLUTSpatioTemporalDIR> GenericOptimizerType;
    typedef typename GenericOptimizerType::Pointer GenericOptimizerPointer;

    // Metric
    typedef typename RegistrationType::FixedImageType    InternalImageType;
    typedef clitk::GenericMetric<args_info_clitkShapedBLUTSpatioTemporalDIR, InternalImageType, InternalImageType> GenericMetricType;
    typedef typename GenericMetricType::Pointer GenericMetricPointer;

    // Two arguments are passed to the Execute() method: the first
    // is the pointer to the object which invoked the event and the 
    // second is the event that was invoked.
    void Execute(itk::Object * object, const itk::EventObject & event)
    {
      if( !(itk::IterationEvent().CheckEvent( &event )) )
	{
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
      if (currentLevel>1)
	{
	  // fixed image region pyramid
	  typedef clitk::MultiResolutionPyramidRegionFilter<InternalImageType> FixedImageRegionPyramidType;
	  typename FixedImageRegionPyramidType::Pointer fixedImageRegionPyramid=FixedImageRegionPyramidType::New();
	  fixedImageRegionPyramid->SetRegion(m_MetricRegion);
	  fixedImageRegionPyramid->SetSchedule(registration->GetFixedImagePyramid()->GetSchedule());

	  // Reinitialize the metric (!= number of samples)
	  m_GenericMetric= GenericMetricType::New();
	  m_GenericMetric->SetArgsInfo(m_ArgsInfo);
	  m_GenericMetric->SetFixedImage(registration->GetFixedImagePyramid()->GetOutput(registration->GetCurrentLevel()));
	  if (m_ArgsInfo.referenceMask_given)  m_GenericMetric->SetFixedImageMask(registration->GetMetric()->GetFixedImageMask());
	  m_GenericMetric->SetFixedImageRegion(fixedImageRegionPyramid->GetOutput(registration->GetCurrentLevel()));
	  typedef itk::ImageToImageMetric< InternalImageType, InternalImageType >  MetricType;
	  typename  MetricType::Pointer metric=m_GenericMetric->GetMetricPointer();
	  registration->SetMetric(metric);	

	  // Get the current coefficient image and make a COPY
	  typename itk::ImageDuplicator<CoefficientImageType>::Pointer caster=itk::ImageDuplicator<CoefficientImageType>::New();
	  caster->SetInputImage(m_Initializer->GetTransform()->GetCoefficientImage());
	  caster->Update();
	  typename CoefficientImageType::Pointer currentCoefficientImage=caster->GetOutput();

	  // Write the intermediate result?
	  if (m_ArgsInfo.intermediate_given>=numberOfLevels)
	    writeImage<CoefficientImageType>(currentCoefficientImage, m_ArgsInfo.intermediate_arg[currentLevel-2], m_ArgsInfo.verbose_flag);
	  
	  // Set the new transform properties
	  m_Initializer->SetImage(registration->GetFixedImagePyramid()->GetOutput(currentLevel-1));
	  if( m_Initializer->m_ControlPointSpacingIsGiven) 
	    m_Initializer->SetControlPointSpacing(m_Initializer->m_ControlPointSpacingArray[registration->GetCurrentLevel()]);
	  if( m_Initializer->m_NumberOfControlPointsIsGiven) 
	    m_Initializer->SetNumberOfControlPointsInsideTheImage(m_Initializer->m_NumberOfControlPointsInsideTheImageArray[registration->GetCurrentLevel()]);
  
	  // Reinitialize the transform
	  if (m_ArgsInfo.verbose_flag) std::cout<<"Initializing transform for level "<<currentLevel<<" of "<<numberOfLevels<<"..."<<std::endl;
	  m_Initializer->InitializeTransform();
	  ParametersType* newParameters= new typename TransformType::ParametersType(m_Initializer->GetTransform()->GetNumberOfParameters());

	  // Reinitialize an Optimizer (!= number of parameters)
	  m_GenericOptimizer = GenericOptimizerType::New();
	  m_GenericOptimizer->SetArgsInfo(m_ArgsInfo);
	  m_GenericOptimizer->SetMaximize(m_Maximize);
	  m_GenericOptimizer->SetNumberOfParameters(m_Initializer->GetTransform()->GetNumberOfParameters());
	  typedef itk::SingleValuedNonLinearOptimizer OptimizerType;
	  OptimizerType::Pointer optimizer = m_GenericOptimizer->GetOptimizerPointer();
	  optimizer->AddObserver( itk::IterationEvent(), m_CommandIterationUpdate);
	  registration->SetOptimizer(optimizer);						  
	  m_CommandIterationUpdate->SetOptimizer(m_GenericOptimizer);
	  
	  // Set the previous transform parameters to the registration
	  // if(m_Initializer->m_Parameters!=NULL )delete m_Initializer->m_Parameters;
	  m_Initializer->SetInitialParameters(currentCoefficientImage,*newParameters); 
 	  registration->SetInitialTransformParametersOfNextLevel(*newParameters);  
	}
    }

    void Execute(const itk::Object * , const itk::EventObject & )
    { return; }


    // Members
    void SetInitializer(InitializerPointer i){m_Initializer=i;}
    InitializerPointer m_Initializer;

    void SetArgsInfo(args_info_clitkShapedBLUTSpatioTemporalDIR a){m_ArgsInfo=a;}
    args_info_clitkShapedBLUTSpatioTemporalDIR m_ArgsInfo;

    void SetCommandIterationUpdate(CommandIterationUpdatePointer c){m_CommandIterationUpdate=c;};
    CommandIterationUpdatePointer m_CommandIterationUpdate;

    GenericOptimizerPointer m_GenericOptimizer;
    void SetMaximize(bool b){m_Maximize=b;}
    bool m_Maximize;

    GenericMetricPointer m_GenericMetric;
    void SetMetricRegion(RegionType i){m_MetricRegion=i;}
    RegionType m_MetricRegion;
    
 
  };


  //==============================================================================
  // Update with the number of dimensions
  //==============================================================================
  template<unsigned int Dimension>
  void 
  ShapedBLUTSpatioTemporalDIRGenericFilter::UpdateWithDim(std::string PixelType)
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
    
    //     else if (PixelType == "unsigned_char"){ 
    //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
    //       UpdateWithDimAndPixelType<Dimension, unsigned char>();
    //     }
    
    //     else if (PixelType == "char"){ 
    //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
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
  void 
  ShapedBLUTSpatioTemporalDIRGenericFilter::UpdateWithDimAndPixelType()
  {


    //=============================================================================
    //Input
    //=============================================================================
    bool threadsGiven=m_ArgsInfo.threads_given;
    int threads=m_ArgsInfo.threads_arg;
    
    typedef itk::Image< PixelType, ImageDimension >  FixedImageType;
    typedef itk::Image< PixelType, ImageDimension >  MovingImageType;
    const unsigned int SpaceDimension = ImageDimension-1;
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
    typename FixedImageType::Pointer croppedFixedImage=fixedImage;


    //=======================================================
    // Regions
    //=======================================================

    // The original input region    
    typename FixedImageType::RegionType fixedImageRegion = fixedImage->GetLargestPossibleRegion();

    // The transform region with respect to the input region:
    // where should the transform be DEFINED (depends on mask)
    typename FixedImageType::RegionType transformRegion = fixedImage->GetLargestPossibleRegion();
    typename FixedImageType::RegionType::SizeType transformRegionSize=transformRegion.GetSize();
    typename FixedImageType::RegionType::IndexType transformRegionIndex=transformRegion.GetIndex();
    typename FixedImageType::PointType transformRegionOrigin=fixedImage->GetOrigin();

    // The metric region with respect to the extracted transform region: 
    // where should the metric be CALCULATED (depends on transform)
    typename FixedImageType::RegionType metricRegion = fixedImage->GetLargestPossibleRegion();
    typename FixedImageType::RegionType::SizeType metricRegionSize=metricRegion.GetSize();
    typename FixedImageType::RegionType::IndexType metricRegionIndex=metricRegion.GetIndex();
    typename FixedImageType::PointType metricRegionOrigin=fixedImage->GetOrigin();

    //===========================================================================
    // If given, we connect a mask to reference or target
    //============================================================================
    typedef itk::ImageMaskSpatialObject<  ImageDimension >   MaskType;
    typename MaskType::Pointer  fixedMask=NULL;
    if (m_ArgsInfo.referenceMask_given)
      {
	fixedMask= MaskType::New();
	typedef itk::Image< unsigned char, ImageDimension >   ImageMaskType;
	typedef itk::ImageFileReader< ImageMaskType >    MaskReaderType;
	typename MaskReaderType::Pointer  maskReader = MaskReaderType::New();
	maskReader->SetFileName(m_ArgsInfo.referenceMask_arg);
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
	fixedMask->SetImage( maskReader->GetOutput() );

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
	
	// Crop the fixedImage to the bounding box to facilitate multi-resolution 
	typedef itk::ExtractImageFilter<FixedImageType,FixedImageType> ExtractImageFilterType; 
	typename ExtractImageFilterType::Pointer extractImageFilter=ExtractImageFilterType::New();
	extractImageFilter->SetInput(fixedImage);
	extractImageFilter->SetExtractionRegion(transformRegion);
	extractImageFilter->Update();
	croppedFixedImage=extractImageFilter->GetOutput();

	// Update the metric region	
	metricRegion = croppedFixedImage->GetLargestPossibleRegion();
	metricRegionIndex=metricRegion.GetIndex();
	metricRegionSize=metricRegion.GetSize();
	croppedFixedImage->TransformIndexToPhysicalPoint(metricRegionIndex, metricRegionOrigin);

	// Set start index to zero (with respect to croppedFixedImage/transform region)
	metricRegionIndex.Fill(0);
	metricRegion.SetIndex(metricRegionIndex);
	croppedFixedImage->SetRegions(metricRegion);
	croppedFixedImage->SetOrigin(metricRegionOrigin);

      }

    typedef itk::ImageMaskSpatialObject<  ImageDimension >   MaskType;
    typename MaskType::Pointer  movingMask=NULL;
    if (m_ArgsInfo.targetMask_given)
      {
	movingMask= MaskType::New();
	typedef itk::Image< unsigned char, ImageDimension >   ImageMaskType;
	typedef itk::ImageFileReader< ImageMaskType >    MaskReaderType;
	typename MaskReaderType::Pointer  maskReader = MaskReaderType::New();
	maskReader->SetFileName(m_ArgsInfo.targetMask_arg);
	try 
	  { 
	    maskReader->Update(); 
	  } 
	catch( itk::ExceptionObject & err ) 
	  { 
	    std::cerr << "ExceptionObject caught !" << std::endl; 
	    std::cerr << err << std::endl; 
	  } 
	if (m_Verbose)std::cout <<"Target image mask was read..." <<std::endl;
	
	movingMask->SetImage( maskReader->GetOutput() );
      }


    //=======================================================
    // Output Regions
    //=======================================================

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
    // Pyramids (update them for transform initializer)
    //=======================================================
    typedef clitk::RecursiveSpatioTemporalMultiResolutionPyramidImageFilter< FixedImageType, FixedImageType>    FixedImagePyramidType;
    typedef clitk::RecursiveSpatioTemporalMultiResolutionPyramidImageFilter< MovingImageType, MovingImageType>    MovingImagePyramidType;
    typename FixedImagePyramidType::Pointer fixedImagePyramid = FixedImagePyramidType::New();
    typename MovingImagePyramidType::Pointer movingImagePyramid = MovingImagePyramidType::New();
    fixedImagePyramid->SetUseShrinkImageFilter(false);
    fixedImagePyramid->SetInput(croppedFixedImage);
    fixedImagePyramid->SetNumberOfLevels(m_ArgsInfo.levels_arg);
    movingImagePyramid->SetUseShrinkImageFilter(false);    
    movingImagePyramid->SetInput(movingImage);
    movingImagePyramid->SetNumberOfLevels(m_ArgsInfo.levels_arg);
    if (m_Verbose) std::cout<<"Creating the image pyramid..."<<std::endl;
    fixedImagePyramid->Update();
    movingImagePyramid->Update();
    typedef clitk::MultiResolutionPyramidRegionFilter<FixedImageType> FixedImageRegionPyramidType;
    typename FixedImageRegionPyramidType::Pointer fixedImageRegionPyramid=FixedImageRegionPyramidType::New();
    fixedImageRegionPyramid->SetRegion(metricRegion);
    fixedImageRegionPyramid->SetSchedule(fixedImagePyramid->GetSchedule());




    //     //=======================================================
    //     // Rigid Transform
    //     //=======================================================
    //     typedef itk::Euler3DTransform <double> RigidTransformType;
    //     RigidTransformType::Pointer rigidTransform;
    //     if (m_ArgsInfo.rigid_given)
    //       {
    // 	rigidTransform=RigidTransformType::New();
    // 	itk::Matrix<double,4,4> rigidTransformMatrix=clitk::ReadMatrix3D(m_ArgsInfo.rigid_arg);
    
    // 	//Set the rotation
    // 	itk::Matrix<double,3,3> finalRotation = clitk::GetRotationalPartMatrix3D(rigidTransformMatrix);
    // 	rigidTransform->SetMatrix(finalRotation);
    
    // 	//Set the translation
    // 	itk::Vector<double,3> finalTranslation = clitk::GetTranslationPartMatrix3D(rigidTransformMatrix);
    // 	rigidTransform->SetTranslation(finalTranslation);
    
    //       }


    //=======================================================
    // ShapedBSplineSpatioTemporal Transform
    //=======================================================
    typedef  clitk::ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep,ImageDimension, ImageDimension > TransformType;
    typename TransformType::Pointer transform= TransformType::New();
    transform->SetTransformShape(m_ArgsInfo.shape_arg);
    if (fixedMask) transform->SetMask( fixedMask );
    // if (rigidTransform) transform->SetBulkTransform( rigidTransform );	

    //-------------------------------------------------------------------------
    // The transform initializer
    //-------------------------------------------------------------------------
    typedef clitk::ShapedBLUTSpatioTemporalDeformableTransformInitializer< TransformType,FixedImageType> InitializerType;
    typename InitializerType::Pointer initializer = InitializerType::New();
    initializer->SetVerbose(m_Verbose);
    initializer->SetImage(fixedImagePyramid->GetOutput(0));
    initializer->SetTransform(transform);

    //-------------------------------------------------------------------------
    // Order
    //-------------------------------------------------------------------------
    typename FixedImageType::RegionType::SizeType splineOrders ;
    splineOrders.Fill(3);
    if (m_ArgsInfo.order_given)
      for(unsigned int i=0; i<ImageDimension;i++) 
	splineOrders[i]=m_ArgsInfo.order_arg[i];
    if (m_Verbose) std::cout<<"Setting the spline orders  to "<<splineOrders<<"..."<<std::endl;
    initializer->SetSplineOrders(splineOrders);

    //-------------------------------------------------------------------------
    // Levels
    //-------------------------------------------------------------------------
  
    // Spacing
    if (m_ArgsInfo.spacing_given)
      {
	initializer->m_ControlPointSpacingArray.resize(m_ArgsInfo.levels_arg);
	initializer->SetControlPointSpacing(m_ArgsInfo.spacing_arg);
	initializer->m_ControlPointSpacingArray[m_ArgsInfo.levels_arg-1]=initializer->m_ControlPointSpacing;
	if (m_Verbose) std::cout<<"Using a control point spacing of "<<initializer->m_ControlPointSpacingArray[m_ArgsInfo.levels_arg-1]
				<<" at level "<<m_ArgsInfo.levels_arg<<" of "<<m_ArgsInfo.levels_arg<<"..."<<std::endl;   
	
	for (int i=1; i<m_ArgsInfo.levels_arg; i++ )
	  {
	    initializer->m_ControlPointSpacingArray[m_ArgsInfo.levels_arg-1-i]=initializer->m_ControlPointSpacingArray[m_ArgsInfo.levels_arg-i]*2;
	    initializer->m_ControlPointSpacingArray[m_ArgsInfo.levels_arg-1-i][ImageDimension-1]=2;
	    if (m_Verbose) std::cout<<"Using a control point spacing of "<<initializer->m_ControlPointSpacingArray[m_ArgsInfo.levels_arg-1-i]
				    <<" at level "<<m_ArgsInfo.levels_arg-i<<" of "<<m_ArgsInfo.levels_arg<<"..."<<std::endl;   
	  }
	  
      }  
    
    // Control
    if (m_ArgsInfo.control_given)
      {
	initializer->m_NumberOfControlPointsInsideTheImageArray.resize(m_ArgsInfo.levels_arg);
	initializer->SetNumberOfControlPointsInsideTheImage(m_ArgsInfo.control_arg);
	initializer->m_NumberOfControlPointsInsideTheImageArray[m_ArgsInfo.levels_arg-1]=initializer->m_NumberOfControlPointsInsideTheImage;
	if (m_Verbose) std::cout<<"Using "<< initializer->m_NumberOfControlPointsInsideTheImageArray[m_ArgsInfo.levels_arg-1]<<"control points inside the image"
				<<" at level "<<m_ArgsInfo.levels_arg<<" of "<<m_ArgsInfo.levels_arg<<"..."<<std::endl;   
	
	for (int i=1; i<m_ArgsInfo.levels_arg; i++ )
	  {
	    for(unsigned int j=0;j<ImageDimension-1;j++)
	      initializer->m_NumberOfControlPointsInsideTheImageArray[m_ArgsInfo.levels_arg-1-i][j]=ceil ((double)initializer->m_NumberOfControlPointsInsideTheImageArray[m_ArgsInfo.levels_arg-i][j]/2.);
	      //	    initializer->m_NumberOfControlPointsInsideTheImageArray[m_ArgsInfo.levels_arg-1-i]=ceil ((double)initializer->m_NumberOfControlPointsInsideTheImageArray[m_ArgsInfo.levels_arg-i]/2.);
	    if (m_Verbose) std::cout<<"Using "<< initializer->m_NumberOfControlPointsInsideTheImageArray[m_ArgsInfo.levels_arg-1-i]<<"control points inside the image"
				    <<" at level "<<m_ArgsInfo.levels_arg<<" of "<<m_ArgsInfo.levels_arg<<"..."<<std::endl;   
	
	  }
      }  

    // Inialize on the first level 
    if (m_ArgsInfo.verbose_flag) std::cout<<"Initializing transform for level 1 of "<<m_ArgsInfo.levels_arg<<"..."<<std::endl;
    if (m_ArgsInfo.spacing_given) initializer->SetControlPointSpacing(	initializer->m_ControlPointSpacingArray[0]);
    if (m_ArgsInfo.control_given) initializer->SetNumberOfControlPointsInsideTheImage(initializer->m_NumberOfControlPointsInsideTheImageArray[0]);	
    if (m_ArgsInfo.samplingFactor_given) initializer->SetSamplingFactors(m_ArgsInfo.samplingFactor_arg);	
 
    // Initialize 
    initializer->InitializeTransform();

    //-------------------------------------------------------------------------
    // Initial parameters (passed by reference)
    //-------------------------------------------------------------------------
    typedef typename TransformType::ParametersType     ParametersType;
    const unsigned int numberOfParameters =    transform->GetNumberOfParameters();
    ParametersType parameters(numberOfParameters);
    parameters.Fill( 0.0 );
    transform->SetParameters( parameters );
    if (m_ArgsInfo.initCoeff_given) initializer->SetInitialParameters(m_ArgsInfo.initCoeff_arg, parameters);

	    
    //=======================================================
    // Interpolator
    //=======================================================
    typedef clitk::GenericInterpolator<args_info_clitkShapedBLUTSpatioTemporalDIR, FixedImageType,TCoordRep > GenericInterpolatorType;
    typename   GenericInterpolatorType::Pointer genericInterpolator=GenericInterpolatorType::New();
    genericInterpolator->SetArgsInfo(m_ArgsInfo);
    typedef itk::InterpolateImageFunction< FixedImageType, TCoordRep >  InterpolatorType;
    typename  InterpolatorType::Pointer interpolator=genericInterpolator->GetInterpolatorPointer();


    //=======================================================
    // Metric
    //=======================================================
    typedef clitk::GenericMetric< args_info_clitkShapedBLUTSpatioTemporalDIR, FixedImageType,MovingImageType > GenericMetricType;
    typename GenericMetricType::Pointer genericMetric=GenericMetricType::New();
    genericMetric->SetArgsInfo(m_ArgsInfo);
    genericMetric->SetFixedImage(fixedImagePyramid->GetOutput(0));
    if (fixedMask) genericMetric->SetFixedImageMask(fixedMask);
    genericMetric->SetFixedImageRegion(fixedImageRegionPyramid->GetOutput(0));
    typedef itk::ImageToImageMetric< FixedImageType, MovingImageType >  MetricType;
    typename  MetricType::Pointer metric=genericMetric->GetMetricPointer();
    if (movingMask) metric->SetMovingImageMask(movingMask);

#ifdef ITK_USE_OPTIMIZED_REGISTRATION_METHODS
    if (threadsGiven) metric->SetNumberOfThreads( threads );
#else
    if (m_Verbose) std::cout<<"Not setting the number of threads (not compiled with USE_OPTIMIZED_REGISTRATION_METHODS)..."<<std::endl;
#endif


    //=======================================================
    // Optimizer
    //=======================================================
    typedef clitk::GenericOptimizer<args_info_clitkShapedBLUTSpatioTemporalDIR> GenericOptimizerType;
    GenericOptimizerType::Pointer genericOptimizer = GenericOptimizerType::New();
    genericOptimizer->SetArgsInfo(m_ArgsInfo);
    genericOptimizer->SetMaximize(genericMetric->GetMaximize());
    genericOptimizer->SetNumberOfParameters(transform->GetNumberOfParameters());
    typedef itk::SingleValuedNonLinearOptimizer OptimizerType;
    OptimizerType::Pointer optimizer = genericOptimizer->GetOptimizerPointer();


    //=======================================================
    // Registration
    //=======================================================
    typedef clitk::SpatioTemporalMultiResolutionImageRegistrationMethod<  FixedImageType, MovingImageType >    RegistrationType;
    typename RegistrationType::Pointer   registration  = RegistrationType::New();
    registration->SetMetric(        metric        );
    registration->SetOptimizer(     optimizer     );
    registration->SetInterpolator(  interpolator  );
    registration->SetTransform (transform);
    if(threadsGiven) registration->SetNumberOfThreads(threads);
    registration->SetFixedImage(  croppedFixedImage   );
    registration->SetMovingImage(  movingImage   );
    registration->SetFixedImageRegion( metricRegion );
    registration->SetFixedImagePyramid( fixedImagePyramid );
    registration->SetMovingImagePyramid( movingImagePyramid );
    registration->SetInitialTransformParameters( transform->GetParameters() );
    registration->SetNumberOfLevels( m_ArgsInfo.levels_arg );
    if (m_Verbose) std::cout<<"Setting the number of resolution levels to "<<m_ArgsInfo.levels_arg<<"..."<<std::endl;
       
  
    //================================================================================================
    // Observers
    //================================================================================================
    if (m_Verbose)
      {
	// Output iteration info
	typedef	CommandIterationUpdate <RegistrationType,args_info_clitkShapedBLUTSpatioTemporalDIR> CommandIterationUpdateType;
	typename CommandIterationUpdateType::Pointer observer = CommandIterationUpdateType::New();
	observer->SetInitializer(initializer);
	observer->SetArgsInfo(m_ArgsInfo);
	observer->SetOptimizer(genericOptimizer);
	optimizer->AddObserver( itk::IterationEvent(), observer );
	
	// Output level info
	typedef RegistrationInterfaceCommand<RegistrationType,args_info_clitkShapedBLUTSpatioTemporalDIR> CommandType;
	typename CommandType::Pointer command = CommandType::New();
	command->SetInitializer(initializer);
	command->SetArgsInfo(m_ArgsInfo);
	command->SetCommandIterationUpdate(observer);
	command->SetMaximize(genericMetric->GetMaximize());
	command->SetMetricRegion(metricRegion);
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
    if (m_Verbose)
      {
	std::cout<<"Stop condition description: "
		 <<registration->GetOptimizer()->GetStopConditionDescription()<<std::endl;
      }

 
    //=======================================================
    // Get the BSplineSpatioTemporal coefficient images and write them
    //=======================================================
    if (m_ArgsInfo.coeff_given)
      { 
	typedef typename TransformType::CoefficientImageType CoefficientImageType;
	typename CoefficientImageType::Pointer coefficientImage =transform->GetCoefficientImage();
	typedef itk::ImageFileWriter<CoefficientImageType> CoeffWriterType;
	typename CoeffWriterType::Pointer coeffWriter=CoeffWriterType::New();
	coeffWriter->SetInput(coefficientImage);
	coeffWriter->SetFileName(m_ArgsInfo.coeff_arg);
	coeffWriter->Update();
	
      }
    if (m_ArgsInfo.padCoeff_given)
      { 
	typedef  itk::Image<itk::Vector<TCoordRep, SpaceDimension>, ImageDimension> CoefficientImageType;
	typename CoefficientImageType::Pointer coefficientImage =transform->GetPaddedCoefficientImage();
	typedef itk::ImageFileWriter<CoefficientImageType> CoeffWriterType;
	typename CoeffWriterType::Pointer coeffWriter=CoeffWriterType::New();
	coeffWriter->SetInput(coefficientImage);
	coeffWriter->SetFileName(m_ArgsInfo.padCoeff_arg);
	coeffWriter->Update();
      }    
  

    //=======================================================
    // Generate the DVF (4D with 3 comps and 4D with 4 comps)
    //=======================================================
    typedef itk::Vector< float, SpaceDimension >  DisplacementType;
    typedef itk::Vector< float, ImageDimension >  Displacement4DType;
    typedef itk::Image< DisplacementType, ImageDimension >  DeformationFieldType;
    typedef itk::Image< Displacement4DType, ImageDimension >  DeformationField4DType;
  
    typename DeformationFieldType::Pointer field = DeformationFieldType::New();
    typename DeformationField4DType::Pointer field4D = DeformationField4DType::New();
    field->SetRegions( fixedImageRegion );
    field->SetOrigin( fixedImage->GetOrigin() );
    field->SetSpacing( fixedImage->GetSpacing() );
    field->SetDirection( fixedImage->GetDirection() );
    field->Allocate();
    field4D->SetRegions( fixedImageRegion );
    field4D->SetOrigin( fixedImage->GetOrigin() );
    field4D->SetSpacing( fixedImage->GetSpacing() );
    field4D->SetDirection( fixedImage->GetDirection() );
    field4D->Allocate();
  
    typedef itk::ImageRegionIteratorWithIndex< DeformationFieldType > FieldIterator;
    typedef itk::ImageRegionIteratorWithIndex< DeformationField4DType > Field4DIterator;
    FieldIterator fi( field, fixedImageRegion );
    Field4DIterator fi4D( field4D, fixedImageRegion );
    fi.GoToBegin();
    fi4D.GoToBegin();
  
    typename TransformType::InputPointType  fixedPoint;
    typename TransformType::OutputPointType movingPoint;
    typename DeformationFieldType::IndexType index;
  
    DisplacementType displacement;
    Displacement4DType displacement4D;
    displacement4D[ImageDimension-1]=0;
    while( ! fi.IsAtEnd() )
      {
	index = fi.GetIndex();
	field->TransformIndexToPhysicalPoint( index, fixedPoint );
	movingPoint = transform->TransformPoint( fixedPoint );
	for (unsigned int i=0; i<SpaceDimension; i++)
	  {
	    displacement[i] = movingPoint[i] - fixedPoint[i];
	    displacement4D[i] = movingPoint[i] - fixedPoint[i];
	  }
	fi.Set( displacement );
	fi4D.Set(displacement4D);
	++fi;
	++fi4D;
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
    typedef itk::WarpImageFilter< MovingImageType, FixedImageType, DeformationField4DType >    WarpFilterType;
    typename WarpFilterType::Pointer warp = WarpFilterType::New();

    warp->SetDeformationField( field4D );
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
}//end clitk

#endif //#define clitkShapedBLUTSpatioTemporalDIRGenericFilter_txx
