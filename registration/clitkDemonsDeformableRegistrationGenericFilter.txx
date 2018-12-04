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
#ifndef __clitkDemonsDeformableRegistrationGenericFilter_txx
#define __clitkDemonsDeformableRegistrationGenericFilter_txx
#include "clitkDemonsDeformableRegistrationGenericFilter.h"


namespace clitk
{

  //==============================================================================
  //Creating an observer class that allows us to change parameters at subsequent levels
  //==============================================================================
  template <typename MultiRegistrationFilterType>
  class CommandResolutionLevelUpdate : public itk::Command
  {
  public:
    typedef  CommandResolutionLevelUpdate   Self;
    typedef  itk::Command             Superclass;
    typedef  itk::SmartPointer<Self>  Pointer;
    itkNewMacro( Self );

    //Typedefs
    itkStaticConstMacro(ImageDimension, unsigned int, MultiRegistrationFilterType::ImageDimension); 
    typedef typename MultiRegistrationFilterType::FixedImageType FixedImageType;
    typedef typename MultiRegistrationFilterType::MovingImageType MovingImageType;
    typedef typename MultiRegistrationFilterType::DeformationFieldType DeformationFieldType;
    typedef typename MultiRegistrationFilterType::RegistrationType RegistrationType;
    typedef typename RegistrationType::FixedImageType InternalImageType;
    typedef itk::DiffeomorphicDemonsRegistrationFilter<InternalImageType, InternalImageType, DeformationFieldType> DiffFilterType;
    typedef itk::FastSymmetricForcesDemonsRegistrationFilter<InternalImageType, InternalImageType, DeformationFieldType> SymFilterType;

  protected:
    CommandResolutionLevelUpdate()
    {
      m_CurrentLevel=0;
      m_MaxStep=2;
      m_ScaleStep=false;
      m_ScaleSD=false;
    }

  public:

    //Set 
    void SetMaxRMSError(double* m){m_MaxRMSError=m;}
    void SetSD(double* m){m_SD=m;}
    void SetMaxStep(double m){m_MaxStep=m;}
    void SetScaleSD(bool m){m_ScaleSD=m;}
    void SetScaleStep(bool m){m_ScaleStep=m;}
    void SetRegistrationType(unsigned int i){m_RegistrationType=i;}

    //Execute
    void Execute(const itk::Object * caller, const itk::EventObject & event )
    {
      std::cout << "Warning: The const Execute method shouldn't be called" << std::endl;
    }
    void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      
      // Check event
      if( !(itk::IterationEvent().CheckEvent( &event )) )
	{
	  return;
	}
      
      //Cast caller
      MultiRegistrationFilterType * filter = dynamic_cast<  MultiRegistrationFilterType * >( caller );
     
      // Get the level
      m_CurrentLevel=filter->GetCurrentLevel();
      unsigned int numberOfLevels=filter->GetNumberOfLevels();
      unsigned int expandFactor=1<< (numberOfLevels-m_CurrentLevel-1);
      double * sd = m_SD;
      double maxStep=m_MaxStep;

      // Scale the SD
      if (m_ScaleSD)
	{
	  for (unsigned int i=0 ; i<ImageDimension ; i++) sd[i]*=(double)expandFactor;
	  filter->GetRegistrationFilter()->SetStandardDeviations(sd);
	  filter->GetRegistrationFilter()->SetUpdateFieldStandardDeviations(sd);
	}

      // Scale max step: 2 & 3 only!
      if (m_ScaleStep)
	{
	  maxStep= m_MaxStep*expandFactor;
	  if (m_RegistrationType==2)
	    {
	      SymFilterType* symFilter=dynamic_cast< SymFilterType* >(filter->GetRegistrationFilter());
	      symFilter->SetMaximumUpdateStepLength(maxStep);
	    }
	  else if (m_RegistrationType==3)
	    {
	      DiffFilterType* diffFilter=dynamic_cast< DiffFilterType* >(filter->GetRegistrationFilter());
	      diffFilter->SetMaximumUpdateStepLength(maxStep);
	    }
	}
      
      // Set maxRMS
      filter->GetRegistrationFilter()->SetMaximumRMSError(m_MaxRMSError[m_CurrentLevel]);

      //Print All
      std::cout << "--------------------------------------------------" << std::endl;
      std::cout << "Starting resolution level "<<m_CurrentLevel+1<<" of "<<filter->GetNumberOfLevels()<<"..."<<std::endl;
      std::cout << "Setting the standard deviations to [ "<<sd[0];
      for (unsigned int i=1; i<3; i++) std::cout<<", "<< sd[i];
      std::cout <<" ]..."<<std::endl;
      if ( (m_RegistrationType==2) || (m_RegistrationType==3) ) std::cout <<"Setting the maximum step size to "<< maxStep<<"..."<<std::endl;
      std::cout <<"Setting the maximum RMS field change to "<< m_MaxRMSError[m_CurrentLevel]<<"..."<<std::endl;
      std::cout << "--------------------------------------------------" << std::endl;
    }
    
    double *m_MaxRMSError,*m_SD;
    double  m_MaxStep;
    unsigned int m_CurrentLevel, m_RegistrationType;
    bool m_ScaleSD, m_ScaleStep;
 
  };


  //==============================================================================
  // Creating an observer class that allows output at each iteration
  //==============================================================================
  template <typename RegistrationFilterType, typename FixedImageType, typename MovingImageType>
  class CommandIterationUpdate : public itk::Command 
  {
  public:
    typedef  CommandIterationUpdate   Self;
    typedef  itk::Command             Superclass;
    typedef  itk::SmartPointer<Self>  Pointer;
    typedef  itk::SmartPointer<const Self>  ConstPointer;
    itkNewMacro( Self );
    
    
    //find the multiresolution filter
    //     typedef typename  RegistrationFilterType::FixedImageType InternalImageType;
    //     typedef typename  RegistrationFilterType::MovingImageType MovingImageType;
    typedef typename  RegistrationFilterType::DisplacementFieldType DisplacementFieldType;
    typedef clitk::MultiResolutionPDEDeformableRegistration<FixedImageType, MovingImageType, DisplacementFieldType> MultiResolutionRegistrationType;
    typedef CommandResolutionLevelUpdate<MultiResolutionRegistrationType> LevelObserver;
    
  protected:
    CommandIterationUpdate(){};

  public:

    // Sets
    void SetStop( int* s){m_Stop=s;}
    void SetLevelObserver(LevelObserver* o ){m_LevelObserver=o;}


    //Execute
    void Execute(const itk::Object *, const itk::EventObject & )
    {
      std::cout << "Warning: The const Execute method shouldn't be called" << std::endl;
    } 

    void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      if( !(itk::IterationEvent().CheckEvent( &event )) )
        {
	  return;
        }

      //Cast 
      RegistrationFilterType * filter =  dynamic_cast<  RegistrationFilterType * >( caller );
      
      if(filter)
        {
	  // Get info
	  m_Iteration=filter->GetElapsedIterations();
	  m_Metric=filter->GetMetric();
	  
	  // Output
	  std::cout << m_Iteration<<"\t Field Tolerance= "<<filter->GetMaximumRMSError();
	  std::cout <<"\t DVF Change= " << filter->GetRMSChange()<<"\t RMS= "<<m_Metric;

	  // Using stop criterion?
	  if (m_Stop[m_LevelObserver->m_CurrentLevel]>=0)
	    {
	      // First iteration
	      if(m_Iteration==1)
		{
		  m_Minimum=m_Metric;
		  m_StopCounter=0;
		}
	      
	      // Less then minimum
	      else if(m_Metric<m_Minimum)
		{
		  m_StopCounter=0; 
		  m_Minimum=m_Metric;
		}

	      //Not less then minimum
	      else 
		{
		  m_StopCounter++;
		  if (m_StopCounter>=m_Stop[m_LevelObserver->m_CurrentLevel])
		    filter->StopRegistration();
		}
	      
	      // Output
	      std::cout <<"\t Stop= "<<m_StopCounter<<" / "<<m_Stop[m_LevelObserver->m_CurrentLevel]<<std::endl;
	    }

	  // Not using stop criterion 
	  else std::cout <<std::endl;
	}
    }

    double m_Minimum, m_Metric;
    int m_StopCounter, m_Iteration;
    int * m_Stop;
    LevelObserver* m_LevelObserver; 
  };


  //==============================================================================
  // Update with the number of dimensions
  //==============================================================================
  template<unsigned int Dimension>
  void DemonsDeformableRegistrationGenericFilter::UpdateWithDim(std::string PixelType)
  {
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
      if (m_Verbose) std::cout  << "Launching warp in "<< Dimension <<"D and float..." << std::endl;
      UpdateWithDimAndPixelType<Dimension, float>();
    }
  }

  

  //==============================================================================
  // Update with the number of dimensions and pixeltype
  //==============================================================================
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  template<unsigned int ImageDimension, class PixelType>
  void DemonsDeformableRegistrationGenericFilter::UpdateWithDimAndPixelType()
  {
    //=======================================================
    // Run-time
    //=======================================================
    bool threadsGiven=m_ArgsInfo.threads_given;
    int threads=m_ArgsInfo.threads_arg;
    
    typedef itk::Image< PixelType, ImageDimension >  FixedImageType;
    typedef itk::Image< PixelType, ImageDimension >  MovingImageType;
    typedef itk::Image< float, ImageDimension >  InternalImageType;
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
    typename FixedImageType::RegionType fixedRegion = fixedImage->GetBufferedRegion();
   

    //=======================================================
    //Output
    //=======================================================
    typedef itk::Vector< float, ImageDimension >    VectorPixelType;
    typedef itk::Image<  VectorPixelType, ImageDimension > DeformationFieldType;


    //=======================================================
    //Pyramids
    //=======================================================
    typedef itk::RecursiveMultiResolutionPyramidImageFilter< FixedImageType, InternalImageType>    FixedImagePyramidType;
    typedef itk::RecursiveMultiResolutionPyramidImageFilter< MovingImageType, InternalImageType>    MovingImagePyramidType;
    // typedef itk::MultiResolutionPyramidImageFilter< FixedImageType, FixedImageType>    FixedImagePyramidType;
    // typedef itk::MultiResolutionPyramidImageFilter< MovingImageType, MovingImageType>    MovingImagePyramidType;
    typename FixedImagePyramidType::Pointer fixedImagePyramid = FixedImagePyramidType::New();
    typename MovingImagePyramidType::Pointer movingImagePyramid = MovingImagePyramidType::New();

    
    // =======================================================
    // Define the registation filters
    // =======================================================
    typedef itk::PDEDeformableRegistrationFilter< InternalImageType, InternalImageType, DeformationFieldType>   RegistrationFilterType;
    typename RegistrationFilterType::Pointer pdeFilter;
    typedef clitk::MultiResolutionPDEDeformableRegistration<FixedImageType,MovingImageType,DeformationFieldType >  MultiResolutionRegistrationFilterType;
    typename MultiResolutionRegistrationFilterType::Pointer multiResolutionFilter =  MultiResolutionRegistrationFilterType::New();
    typedef itk::ESMDemonsRegistrationFunction<InternalImageType, InternalImageType, DeformationFieldType> RegistrationFunctionType;


    // =======================================================
    // The multiresolution scheme
    // =======================================================
    if (threadsGiven) {
#if ITK_VERSION_MAJOR <= 4
      multiResolutionFilter->SetNumberOfThreads(threads);
#else
      multiResolutionFilter->SetNumberOfWorkUnits(threads);
#endif
    }
    unsigned int nLevels=m_ArgsInfo.levels_arg;
    if (m_Verbose) std::cout<<"Setting the number of resolution levels to "<<nLevels<<"..."<<std::endl;
    multiResolutionFilter->SetFixedImage( fixedImage );
    multiResolutionFilter->SetMovingImage( movingImage );
    multiResolutionFilter->SetNumberOfLevels( nLevels );
    multiResolutionFilter->SetFixedImagePyramid( fixedImagePyramid );
    multiResolutionFilter->SetMovingImagePyramid( movingImagePyramid );
    if (threadsGiven) {
#if ITK_VERSION_MAJOR <= 4
      multiResolutionFilter->SetNumberOfThreads( threads );
#else
      multiResolutionFilter->SetNumberOfWorkUnits( threads );
#endif
    }

    //------------------------------------
    //Set the number of iterations
    //------------------------------------
    std::vector<unsigned int> nIterations(nLevels);
    for (unsigned int i=0 ; i<nLevels; i++)
      {
	if (m_ArgsInfo.maxIter_given==nLevels)
	  {
	    nIterations[i] =m_ArgsInfo.maxIter_arg[i];
	  }
	else
	  nIterations[i]=m_ArgsInfo.maxIter_arg[0];
      }
    multiResolutionFilter->SetNumberOfIterations( &(nIterations[0]) );
    if(m_Verbose) {
      std::cout<<"Setting the number of iterations to: "<<nIterations[0];
      for (unsigned int i=1; i<nLevels; i++)
	std::cout<<", "<<nIterations [i];
      std::cout<<std::endl;
    }
    
    //------------------------------------
    //Set the max RMS error for the field update
    //------------------------------------
    std::vector<double> maxRMSError(nLevels);
    for (unsigned int i=0 ; i<nLevels; i++)
      {
	if (m_ArgsInfo.maxRMSError_given==nLevels)	
	  maxRMSError[i] =m_ArgsInfo.maxRMSError_arg[i];
	else
	  maxRMSError[i]=m_ArgsInfo.maxRMSError_arg[0];
      }
    if(m_Verbose) {
      std::cout<<"Setting the max root mean squared error to: "<<maxRMSError[0];
      for (unsigned int i=1; i<nLevels; i++)
	std::cout<<", "<<maxRMSError[i];
      std::cout<<std::endl;
    }

    //------------------------------------
    //Get the stop criterion
    //------------------------------------
    std::vector<int> stop(nLevels);
    for (unsigned int i=0; i<nLevels; i++)
      if (m_ArgsInfo.stop_given==nLevels)
	stop[i]=m_ArgsInfo.stop_arg[i];
      else
	stop[i]=m_ArgsInfo.stop_arg[0];
    if(m_Verbose) {
      std::cout<<"Setting the stop criterion to : "<<stop[0];
      for (unsigned int i=1; i<nLevels; i++)
	std::cout<<", "<<stop[i];
      std::cout<<std::endl;
    }
    
    //------------------------------------
    //Grad type
    //------------------------------------
    typename RegistrationFunctionType::GradientType grad=RegistrationFunctionType::Symmetric;
    switch (m_ArgsInfo.gradType_arg)
      {
      case 0:
	grad= RegistrationFunctionType::Symmetric;
	break;
      case 1:
	grad= RegistrationFunctionType::Fixed;
	break;
      case 2:
	grad= RegistrationFunctionType::WarpedMoving;
	break;
      case 3:
	grad= RegistrationFunctionType::MappedMoving;
	break;
      }

    //------------------------------------
    // Set smoothing standard deviations
    //------------------------------------
    double sd [ImageDimension];
    for (unsigned int i=0; i<ImageDimension; i++)
      if (m_ArgsInfo.sd_given==ImageDimension)
	sd[i]=m_ArgsInfo.sd_arg[i];
      else
	sd[i]=m_ArgsInfo.sd_arg[0];
    if(m_Verbose) {
      std::cout<<"Setting the final standard deviations for smoothing to: "<<sd[0];
      for (unsigned int i=1; i<ImageDimension; i++)
	std::cout<<", "<<sd[i];
      std::cout<<std::endl;
    }
    
    // =======================================================
    // The level observer: adjust settings at each level
    // =======================================================
    typename CommandResolutionLevelUpdate<MultiResolutionRegistrationFilterType>::Pointer levelObserver = 
      CommandResolutionLevelUpdate<MultiResolutionRegistrationFilterType>::New();
    multiResolutionFilter->AddObserver( itk::IterationEvent(), levelObserver );
    levelObserver->SetMaxRMSError(&(maxRMSError[0]));
    levelObserver->SetMaxStep(m_ArgsInfo.maxStep_arg);
    levelObserver->SetSD(sd);
    levelObserver->SetScaleStep(m_ArgsInfo.scaleStep_flag);
    levelObserver->SetScaleSD(m_ArgsInfo.scaleSD_flag);
    levelObserver->SetRegistrationType(m_ArgsInfo.demons_arg);


    // =======================================================
    // The type of filter
    // =======================================================
    switch (m_ArgsInfo.demons_arg){
      
    case 0:
      {
	typedef itk::DemonsRegistrationFilter< InternalImageType, InternalImageType, DeformationFieldType > DemonsFilterType;
	typename DemonsFilterType::Pointer m =DemonsFilterType::New();
	
	//Set Parameters for this filter
	m->SetIntensityDifferenceThreshold( m_ArgsInfo.intThreshold_arg);
	m->SetUseMovingImageGradient( m_ArgsInfo.movGrad_flag);
	typename CommandIterationUpdate<DemonsFilterType, FixedImageType, MovingImageType>::Pointer observer = CommandIterationUpdate<DemonsFilterType, FixedImageType, MovingImageType>::New();
	observer->SetStop(&(stop[0]));
	observer->SetLevelObserver(levelObserver);
	m->AddObserver( itk::IterationEvent(), observer );
	if (m_Verbose)  std::cout<<"Using the demons registration filter..."<<std::endl;
	  pdeFilter=m;
	  break;
	}
    
    case 1:
      {
	typedef itk::SymmetricForcesDemonsRegistrationFilter< InternalImageType, InternalImageType, DeformationFieldType > DemonsFilterType; 
	typename DemonsFilterType::Pointer m =DemonsFilterType::New();
	
	//Set Parameters for this filter
	m->SetIntensityDifferenceThreshold( m_ArgsInfo.intThreshold_arg);
	typename CommandIterationUpdate<DemonsFilterType, FixedImageType, MovingImageType>::Pointer observer = CommandIterationUpdate<DemonsFilterType, FixedImageType, MovingImageType>::New();
	observer->SetStop(&(stop[0]));
	observer->SetLevelObserver(levelObserver);
	m->AddObserver( itk::IterationEvent(), observer );
	if (m_Verbose) std::cout<<"Using the symmetric forces demons registration filter..."<<std::endl;
	pdeFilter=m;
	break;
      }
      
    case 2:
      {
	typedef itk::FastSymmetricForcesDemonsRegistrationFilter< InternalImageType, InternalImageType, DeformationFieldType > DemonsFilterType;
	typename DemonsFilterType::Pointer m = DemonsFilterType::New();
	
	//Set Parameters for this filter
	m->SetIntensityDifferenceThreshold( m_ArgsInfo.intThreshold_arg);
	m->SetMaximumUpdateStepLength( m_ArgsInfo.maxStep_arg);
	m->SetUseGradientType(grad);
	typename CommandIterationUpdate<DemonsFilterType, FixedImageType, MovingImageType>::Pointer observer = CommandIterationUpdate<DemonsFilterType, FixedImageType, MovingImageType>::New();
	observer->SetStop(&(stop[0]));
	observer->SetLevelObserver(levelObserver);
	m->AddObserver( itk::IterationEvent(), observer );
  	if (m_Verbose) std::cout<<"Using the fast symmetric forces demons registration filter..."<<std::endl;
	pdeFilter=m;
	break;
      }

    case 3:
      {
	typedef itk::DiffeomorphicDemonsRegistrationFilter< InternalImageType, InternalImageType, DeformationFieldType > DemonsFilterType;
	typename DemonsFilterType::Pointer m =  DemonsFilterType::New();
	
	//Set Parameters for this filter
	m->SetMaximumUpdateStepLength( m_ArgsInfo.maxStep_arg);
	m->SetUseFirstOrderExp(m_ArgsInfo.firstOrder_flag);
	m->SetUseGradientType(grad);
	typename CommandIterationUpdate<DemonsFilterType, FixedImageType, MovingImageType>::Pointer observer = CommandIterationUpdate<DemonsFilterType, FixedImageType, MovingImageType>::New();
	observer->SetStop(&(stop[0]));
	observer->SetLevelObserver(levelObserver);
	m->AddObserver( itk::IterationEvent(), observer );
	if (m_Verbose) std::cout<<"Using the diffeomorphic demons registration filter..."<<std::endl;
	pdeFilter=m;
	break;
      }
    }



    //Set common options
    pdeFilter->SetStandardDeviations( sd );
    pdeFilter->SetUpdateFieldStandardDeviations( sd );
    //JV TODO
    // pdeFilter->SetMaximumError(m_ArgsInfo.maxError_arg);
    // pdeFilter->SetMaximumKernelWidth(m_ArgsInfo.maxError_arg);
    pdeFilter->SetSmoothDisplacementField(!m_ArgsInfo.fluid_flag);
    pdeFilter->SetSmoothUpdateField(m_ArgsInfo.fluid_flag);
    pdeFilter->SetUseImageSpacing( m_ArgsInfo.spacing_flag );

    //Pass to the multi resolution scheme
    multiResolutionFilter->SetRegistrationFilter( pdeFilter );
   
    
    // =======================================================
    // The initial solution
    // =======================================================
    if (m_ArgsInfo.init_given)
      {
	typedef itk::ImageFileReader<DeformationFieldType> DeformationFieldReaderType;      
	typename DeformationFieldReaderType::Pointer defReader=DeformationFieldReaderType::New();
	defReader->SetFileName(m_ArgsInfo.init_arg);
	defReader->Update();
	multiResolutionFilter->SetArbitraryInitialDeformationField(defReader->GetOutput());
      }
   
  
    // =======================================================
    // Execute
    // =======================================================
    try
      {
	multiResolutionFilter->Update();
      }
    catch( itk::ExceptionObject & excp )
      {
	std::cerr <<"Error executing the demons filter: "<< excp << std::endl;
	return;
      }

    
    //=======================================================
    // Get the output
    //=======================================================
    typename DeformationFieldType::Pointer deformationField=multiResolutionFilter->GetOutput();


    //=======================================================
    // Write the DVF
    //=======================================================
    typedef itk::ImageFileWriter< DeformationFieldType >  FieldWriterType;
    typename FieldWriterType::Pointer fieldWriter = FieldWriterType::New();
    fieldWriter->SetInput( deformationField );
    fieldWriter->SetFileName( m_ArgsInfo.vf_arg );
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
    // Warp the moving image
    //=======================================================
    typedef itk::WarpImageFilter< MovingImageType, FixedImageType, DeformationFieldType >    WarpFilterType;
    typename WarpFilterType::Pointer warp = WarpFilterType::New();

    warp->SetDisplacementField( deformationField );
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
	typename DifferenceFilterType::Pointer difference = DifferenceFilterType::New();
	difference->SetValidInput( fixedImage );
	difference->SetTestInput( movingImage );
    
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
 
}//end namespace

#endif // __clitkDemonsDeformableRegistrationGenericFilter_txx
