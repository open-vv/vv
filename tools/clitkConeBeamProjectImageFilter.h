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
#ifndef __clitkConeBeamProjectImageFilter_h
#define __clitkConeBeamProjectImageFilter_h
#include "clitkImageCommon.h"
#include "clitkImageCommon.h"
#include "clitkTransformUtilities.h"
#include "clitkExtractImageFilter.h"
#include "itkRayCastInterpolateImageFunctionWithOrigin.h"

//ITK include
#include "itkLightObject.h"
#include "itkResampleImageFilter.h"
#include "itkEuler3DTransform.h"
#include "itkExtractImageFilter.h"
#include "itkFlipImageFilter.h"

namespace clitk
{
  
  template <class InputImageType, class OutputImageType >  
  class ITK_EXPORT ConeBeamProjectImageFilter : public itk::LightObject
  
  {
  public:

    typedef ConeBeamProjectImageFilter     Self;
    typedef itk::LightObject     Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    /** Method for creation through the object factory. */
    itkNewMacro(Self);  

    /** Determine the image dimension. */
    itkStaticConstMacro(InputImageDimension, unsigned int,
			InputImageType::ImageDimension );
    
    itkStaticConstMacro(OutputImageDimension, unsigned int,
			OutputImageType::ImageDimension );


    //========================================================================================
    //Typedefs 
    typedef typename InputImageType::Pointer InputImagePointer;
    typedef typename InputImageType::ConstPointer InputImageConstPointer;
    typedef typename InputImageType::PixelType InputPixelType;
    typedef typename InputImageType::SizeType InputSizeType;
    typedef typename InputImageType::SpacingType InputSpacingType;
    typedef typename InputImageType::DirectionType InputDirectionType;
    typedef typename InputImageType::IndexType InputIndexType;
    typedef typename InputImageType::PointType InputPointType;

    typedef typename OutputImageType::Pointer OutputImagePointer;
    typedef typename OutputImageType::ConstPointer OutputImageConstPointer;
    typedef typename OutputImageType::PixelType OutputPixelType;
    typedef typename OutputImageType::SizeType OutputSizeType;
    typedef typename OutputImageType::SpacingType OutputSpacingType;
    typedef typename OutputImageType::DirectionType OutputDirectionType;
    typedef typename OutputImageType::IndexType OutputIndexType;
    typedef typename OutputImageType::PointType OutputPointType;

    typedef itk::Image<OutputPixelType, InputImageDimension> InternalImageType;

    //Pipeline
    typedef itk::Euler3DTransform <double> TransformType;
    typedef itk::ResampleImageFilter<InputImageType, InternalImageType > ResampleImageFilterType;
    typedef itk::RayCastInterpolateImageFunctionWithOrigin<InputImageType,double> InterpolatorType;
    typedef clitk::ExtractImageFilter<InternalImageType, OutputImageType> ExtractImageFilterType;
    typedef itk::FlipImageFilter<OutputImageType> FlipImageFilterType;
    typedef itk::Matrix<double,4,4> MatrixType;

    //========================================================================================
    //Set Methods
    //========================================================================================
    // Run-time
    void SetVerbose(const bool m){m_Verbose=m;}
    void SetNumberOfThreads(const unsigned int v)
    {
      m_NumberOfThreadsIsGiven=true;
      m_NumberOfThreads=v;
    }

    void SetInput(const InputImagePointer m) {m_Input=m;} //no reinitialize
    
    //=================================================================================
    //Geometry (reinitialize)
    void SetIsoCenter( const InputPointType & i )
    {
      if( m_IsoCenter!=i)
	{
	  m_IsoCenter=i;
	  m_IsInitialized=false;
	}
    }
       
    void SetSourceToScreen(const double& s)
    {
      if( m_SourceToScreen!=s)
	{
	  m_SourceToScreen=s;
	  m_IsInitialized=false;
	}
    }
    
    void SetSourceToAxis( const double& s)
    {
      if( m_SourceToAxis!=s)
	{
	  m_SourceToAxis=s;
	  m_IsInitialized=false;
	}
    }
    
    void SetProjectionAngle( const double& s)
    {
      if( m_SourceToScreen!=s)
	{
	  m_ProjectionAngle=s;
	  m_IsInitialized=false;
	}
    }
    
    void SetRigidTransformMatrix(const MatrixType& m)
    {
      if(m_RigidTransformMatrix != m)
	{
	  m_RigidTransformMatrix=m;
	  m_IsInitialized=false;
	}
    }
       
    void SetEdgePaddingValue(const OutputPixelType& p)
    {
      if (m_EdgePaddingValue!=p)
	{
	  m_EdgePaddingValue=p;
	  m_IsInitialized=false;
	}
    }
        
 
    //========================================================================================
    //Output image properties
    /** Get the size of the output image. */
    void SetOutputSize(const OutputSizeType& p)
    {
      if (m_OutputSize!=p)
	{
	  m_OutputSize=p;
	  m_IsInitialized=false;
	}
    }
     
    /** Set the output image spacing. */
    void SetOutputSpacing(const OutputSpacingType& p)
    {
      if (m_OutputSpacing!=p)
	{
	  m_OutputSpacing=p;
	  m_IsInitialized=false;
	}
    }
    
    /** Set the output image origin. */
    void SetOutputOrigin(const OutputPointType& p)
    {
      if (m_OutputOrigin!=p)
	{
	  m_OutputOrigin=p;
	  m_IsInitialized=false;
	}
    }

    /** Helper method to set the output parameters based on this image */
    void SetOutputParametersFromImage( const OutputImagePointer image );

    /** Helper method to set the output parameters based on this image */
    void SetOutputParametersFromImage( const OutputImageConstPointer image );

    //========================================================================================
    //Update
    void Initialize(void)throw (itk::ExceptionObject);
    void Update(void);
    OutputImagePointer GetOutput(void);

  protected:
    ConeBeamProjectImageFilter();
    ~ConeBeamProjectImageFilter() {};

    // Run time  
    bool m_Verbose;
    bool m_NumberOfThreadsIsGiven;
    unsigned int m_NumberOfThreads;
    bool m_IsInitialized;
    //std::string m_Mask;

    // Data
    InputImageConstPointer m_Input;
    OutputImagePointer m_Output;

    // Geometry
    InputPointType m_IsoCenter;
    double m_SourceToScreen;
    double m_SourceToAxis;
    double m_ProjectionAngle;
    MatrixType m_RigidTransformMatrix;
    OutputPixelType m_EdgePaddingValue;

    // Pipe 
    TransformType::Pointer m_Transform;
    typename ResampleImageFilterType::Pointer m_Resampler;
    typename InterpolatorType::Pointer m_Interpolator;
    typename ExtractImageFilterType::Pointer m_ExtractImageFilter;
    typename FlipImageFilterType::Pointer m_FlipImageFilter;

    // Output image info
    OutputSizeType                m_OutputSize;        // Size of the output image
    OutputSpacingType             m_OutputSpacing;     // output image spacing
    OutputPointType               m_OutputOrigin;      // output image origin
  };


} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkConeBeamProjectImageFilter.txx"
#endif

#endif // #define __clitkConeBeamProjectImageFilter_h
