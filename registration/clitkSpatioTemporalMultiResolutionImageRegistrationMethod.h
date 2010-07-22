/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/
#ifndef __clitkSpatioTemporalMultiResolutionImageRegistrationMethod_h
#define __clitkSpatioTemporalMultiResolutionImageRegistrationMethod_h
#include "clitkSpatioTemporalMultiResolutionPyramidImageFilter.h"

#include "itkProcessObject.h"
#include "itkImageToImageMetric.h"
#include "itkSingleValuedNonLinearOptimizer.h"
#include "itkNumericTraits.h"
#include "itkDataObjectDecorator.h"

namespace clitk
{

template <typename TFixedImage, typename TMovingImage>
class ITK_EXPORT SpatioTemporalMultiResolutionImageRegistrationMethod : public itk::ProcessObject 
{
public:
  /** Standard class typedefs. */
  typedef SpatioTemporalMultiResolutionImageRegistrationMethod  Self;
  typedef itk::ProcessObject                      Superclass;
  typedef SmartPointer<Self>                      Pointer;
  typedef SmartPointer<const Self>                ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(SpatioTemporalMultiResolutionImageRegistrationMethod, ProcessObject);

  /**  Type of the Fixed image. */
  typedef          TFixedImage                     FixedImageType;
  typedef typename FixedImageType::ConstPointer    FixedImageConstPointer;
  typedef typename FixedImageType::RegionType      FixedImageRegionType;

  /**  Type of the Moving image. */
  typedef          TMovingImage                    MovingImageType;
  typedef typename MovingImageType::ConstPointer   MovingImageConstPointer;

  /**  Type of the metric. */
  typedef itk::ImageToImageMetric< FixedImageType,
                              MovingImageType >    MetricType;
  typedef typename MetricType::Pointer             MetricPointer;

  /**  Type of the Transform . */
  typedef typename MetricType::TransformType       TransformType;
  typedef typename TransformType::Pointer          TransformPointer;

  /** Type for the output: Using Decorator pattern for enabling
   *  the Transform to be passed in the data pipeline */
  typedef itk::DataObjectDecorator< TransformType >  TransformOutputType;
  typedef typename TransformOutputType::Pointer      TransformOutputPointer;
  typedef typename TransformOutputType::ConstPointer TransformOutputConstPointer;
  
  /**  Type of the Interpolator. */
  typedef typename MetricType::InterpolatorType    InterpolatorType;
  typedef typename InterpolatorType::Pointer       InterpolatorPointer;

  /**  Type of the optimizer. */
  typedef itk::SingleValuedNonLinearOptimizer           OptimizerType;

  //JV replace with clitk Spatio Temporal Type
  /** Type of the Fixed image multiresolution pyramid. */
  typedef SpatioTemporalMultiResolutionPyramidImageFilter< FixedImageType,
                                             FixedImageType >
                                                   FixedImagePyramidType;
  typedef typename FixedImagePyramidType::Pointer  FixedImagePyramidPointer;

  /** Type of pyramid schedule type */
  typedef typename FixedImagePyramidType::ScheduleType ScheduleType;

  //JV replace with clitk Spatio Temporal Type
  /** Type of the moving image multiresolution pyramid. */
  typedef SpatioTemporalMultiResolutionPyramidImageFilter< MovingImageType,
                                             MovingImageType >
                                                   MovingImagePyramidType;
  typedef typename MovingImagePyramidType::Pointer MovingImagePyramidPointer;

  /** Type of the Transformation parameters This is the same type used to
   *  represent the search space of the optimization algorithm */
  typedef  typename MetricType::TransformParametersType    ParametersType;

  /** Smart Pointer type to a DataObject. */
  typedef typename itk::DataObject::Pointer DataObjectPointer;

  /** Method that initiates the registration. */
  void StartRegistration();

  /** Method to stop the registration. */
  void StopRegistration();

  /** Set/Get the Fixed image. */
  itkSetConstObjectMacro( FixedImage, FixedImageType );
  itkGetConstObjectMacro( FixedImage, FixedImageType ); 

  /** Set/Get the Moving image. */
  itkSetConstObjectMacro( MovingImage, MovingImageType );
  itkGetConstObjectMacro( MovingImage, MovingImageType );

  /** Set/Get the Optimizer. */
  itkSetObjectMacro( Optimizer,  OptimizerType );
  itkGetObjectMacro( Optimizer,  OptimizerType );

  /** Set/Get the Metric. */
  itkSetObjectMacro( Metric, MetricType );
  itkGetObjectMacro( Metric, MetricType );

  /** Set/Get the Metric. */
  itkSetMacro( FixedImageRegion, FixedImageRegionType );
  itkGetConstReferenceMacro( FixedImageRegion, FixedImageRegionType );

  /** Set/Get the Transfrom. */
  itkSetObjectMacro( Transform, TransformType );
  itkGetObjectMacro( Transform, TransformType );

  /** Set/Get the Interpolator. */
  itkSetObjectMacro( Interpolator, InterpolatorType );
  itkGetObjectMacro( Interpolator, InterpolatorType );

  /** Set/Get the Fixed image pyramid. */
  itkSetObjectMacro( FixedImagePyramid, FixedImagePyramidType );
  itkGetObjectMacro( FixedImagePyramid, FixedImagePyramidType ); 

  /** Set/Get the Moving image pyramid. */
  itkSetObjectMacro( MovingImagePyramid, MovingImagePyramidType );
  itkGetObjectMacro( MovingImagePyramid, MovingImagePyramidType );

  /** Set/Get the schedules . */
  void SetSchedules( const ScheduleType & fixedSchedule,
                    const ScheduleType & movingSchedule );
  itkGetConstMacro( FixedImagePyramidSchedule, ScheduleType ); 
  itkGetConstMacro( MovingImagePyramidSchedule, ScheduleType ); 

  /** Set/Get the number of multi-resolution levels. */
  void SetNumberOfLevels( unsigned long numberOfLevels );
  itkGetMacro( NumberOfLevels, unsigned long );

  /** Get the current resolution level being processed. */
  itkGetMacro( CurrentLevel, unsigned long );

  /** Set/Get the initial transformation parameters. */
  itkSetMacro( InitialTransformParameters, ParametersType );
  itkGetConstReferenceMacro( InitialTransformParameters, ParametersType );

  /** Set/Get the initial transformation parameters of the next resolution
   level to be processed. The default is the last set of parameters of
   the last resolution level. */
  itkSetMacro( InitialTransformParametersOfNextLevel, ParametersType );
  itkGetConstReferenceMacro( InitialTransformParametersOfNextLevel, ParametersType );

  /** Get the last transformation parameters visited by 
   * the optimizer. */
  itkGetConstReferenceMacro( LastTransformParameters, ParametersType );  

  /** Returns the transform resulting from the registration process  */
  const TransformOutputType * GetOutput() const;

  /** Make a DataObject of the correct type to be used as the specified
   * output. */
  virtual DataObjectPointer MakeOutput(unsigned int idx);

  /** Method to return the latest modified time of this object or
   * any of its cached ivars */
  unsigned long GetMTime() const;  
  
protected:
  SpatioTemporalMultiResolutionImageRegistrationMethod();
  virtual ~SpatioTemporalMultiResolutionImageRegistrationMethod() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Method invoked by the pipeline in order to trigger the computation of 
   * the registration. */
  void  GenerateData ();

  /** Initialize by setting the interconnects between the components.
      This method is executed at every level of the pyramid with the
      values corresponding to this resolution
   */
  void Initialize() throw (ExceptionObject);

  /** Compute the size of the fixed region for each level of the pyramid. */
  void PreparePyramids( void );

  /** Set the current level to be processed */  
  itkSetMacro( CurrentLevel, unsigned long );

private:
  SpatioTemporalMultiResolutionImageRegistrationMethod(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  MetricPointer                    m_Metric;
  OptimizerType::Pointer           m_Optimizer;

  MovingImageConstPointer          m_MovingImage;
  FixedImageConstPointer           m_FixedImage;

  TransformPointer                 m_Transform;
  InterpolatorPointer              m_Interpolator;

  MovingImagePyramidPointer        m_MovingImagePyramid;
  FixedImagePyramidPointer         m_FixedImagePyramid;

  ParametersType                   m_InitialTransformParameters;
  ParametersType                   m_InitialTransformParametersOfNextLevel;
  ParametersType                   m_LastTransformParameters;

  FixedImageRegionType               m_FixedImageRegion;
  std::vector<FixedImageRegionType>  m_FixedImageRegionPyramid;

  unsigned long                    m_NumberOfLevels;
  unsigned long                    m_CurrentLevel;

  bool                             m_Stop;

  ScheduleType                     m_FixedImagePyramidSchedule;
  ScheduleType                     m_MovingImagePyramidSchedule;

  bool                             m_ScheduleSpecified;
  bool                             m_NumberOfLevelsSpecified;
  
};


} // end namespace clitk


#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkSpatioTemporalMultiResolutionImageRegistrationMethod.txx"
#endif

#endif
