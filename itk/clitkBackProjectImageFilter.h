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
#ifndef __clitkBackProjectImageFilter_h
#define __clitkBackProjectImageFilter_h
#include "clitkTransformUtilities.h"

//itk include
#include "itkInterpolateImageFunction.h"                                  
#include "itkImageToImageFilter.h"                                  
#include "itkTransform.h"
#include "itkVector.h"


namespace clitk
{

  template <class InputImageType, class OutputImageType>
  class ITK_EXPORT BackProjectImageFilter : 
    public itk::ImageToImageFilter<InputImageType,OutputImageType> 
  {
  public:
    //------------------------------------
    // Standard itk typedefs
    //-----------------------------------
    typedef BackProjectImageFilter                                  Self;
    typedef itk::ImageToImageFilter<InputImageType,OutputImageType> Superclass;
    typedef itk::SmartPointer<Self>                                 Pointer;
    typedef itk::SmartPointer<const Self>                           ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self); 

    /** Run-time type information (and related methods). */
    itkTypeMacro(BackProjectImageFilter, ImageToImageFilter); 

    /** Dimension of the domain space. */
    itkStaticConstMacro(InputImageDimension, unsigned int, Superclass::InputImageDimension);
    itkStaticConstMacro(OutputImageDimension, unsigned int, Superclass::OutputImageDimension);

    //------------------------------------
    // Typedefs
    //------------------------------------
    typedef typename InputImageType::Pointer InputImagePointer;
    typedef typename InputImageType::ConstPointer InputImageConstPointer;
    typedef typename InputImageType::PixelType InputPixelType;
    typedef typename InputImageType::SizeType InputSizeType;
    typedef typename InputImageType::SpacingType InputSpacingType;
    typedef typename InputImageType::DirectionType InputDirectionType;
    typedef typename InputImageType::IndexType InputIndexType;
    typedef typename InputImageType::PointType InputPointType;
    typedef typename itk::ContinuousIndex<double,InputImageDimension> ContinuousInputIndexType;
    typedef typename InputImageType::RegionType InputImageRegionType;
    typedef itk::Vector<double,InputImageDimension+1> HomogeneInputVectorType;
    typedef itk::Vector<double,InputImageDimension> Homogene2DVectorType;
    typedef itk::Point<double,InputImageDimension+1> HomogeneInputPointType;
    typedef itk::Point<double,InputImageDimension> Homogene2DPointType;

    typedef typename OutputImageType::Pointer OutputImagePointer;
    typedef typename OutputImageType::ConstPointer OutputImageConstPointer;
    typedef typename OutputImageType::PixelType OutputPixelType;
    typedef typename OutputImageType::SizeType OutputSizeType;
    typedef typename OutputImageType::SpacingType OutputSpacingType;
    typedef typename OutputImageType::DirectionType OutputDirectionType;
    typedef typename OutputImageType::IndexType OutputIndexType;
    typedef typename OutputImageType::PointType OutputPointType;
    typedef typename itk::ContinuousIndex<double,OutputImageDimension> ContinuousOutputIndexType;
    typedef typename OutputImageType::RegionType OutputImageRegionType;
    typedef itk::Vector<double,OutputImageDimension+1> HomogeneOutputVectorType;
    typedef itk::Point<double,OutputImageDimension+1> HomogeneOutputPointType;

    typedef itk::Matrix<double,4,4> MatrixType;
  
    //------------------------------------
    // Projection geometry
    //-----------------------------------
    void SetIsoCenter( const OutputPointType& i )
    {
      if( m_IsoCenter!=i)
	{
	  m_IsoCenter=i;
	  m_IsInitialized=false;
	  this->Modified();
	}
    }
       
    void SetSourceToScreen(const double &s)
    {
      if( m_SourceToScreen!=s)
	{
	  m_SourceToScreen=s;
	  m_IsInitialized=false;
	  this->Modified();
	}
    }
    
    void SetSourceToAxis( const double& s)
    {
      if( m_SourceToAxis!=s)
	{
	  m_SourceToAxis=s;
	  m_IsInitialized=false;
	  this->Modified();
	}
    }
    
    void SetProjectionAngle(const double& s)
    {
      if( m_SourceToScreen!=s)
	{
	  m_ProjectionAngle=s;
	  m_IsInitialized=false;
	  this->Modified();
	}
    }
    
    void SetRigidTransformMatrix(const MatrixType& m)
    {
      if(m_RigidTransformMatrix != m)
	{
	  m_RigidTransformMatrix=m;
	  m_IsInitialized=false;
	  this->Modified();
	}
    }
       
    void SetEdgePaddingValue(const OutputPixelType& p)
    {
      if (m_EdgePaddingValue!=p)
	{
	  m_EdgePaddingValue=p;
	  m_IsInitialized=false;
	  this->Modified();
	}
    }

    void SetPanelShift(double x, double y)
    {
      m_PanelShift[0] = x;
      m_PanelShift[1] = y;
    }
    //     itkSetMacro(IsoCenter, OutputPointType);
    //     itkGetConstReferenceMacro(IsoCenter, OutputPointType)
    //     itkSetMacro( SourceToScreen, double );
    //     itkGetMacro( SourceToScreen, double );
    //     itkSetMacro( SourceToAxis, double );
    //     itkGetMacro( SourceToAxis, double );
    //     itkSetMacro(RigidTransformMatrix, itk::Matrix<double,4,4>));
    //     itkGetConstReferenceMacro(RigidTransformMatrix, itk::Matrix<double,4,4>);
    //     void SetRigidTransformMatrix(const  itk::Matrix<double,4,4> & m);
    //     itk::Matrix<double,4,4> GetRigidTransformMatrix( void ){return m_RigidTransformMatrix;}
    //     itkSetMacro( EdgePaddingValue, OutputPixelType );
    //     itkGetMacro( EdgePaddingValue, OutputPixelType );
    //     itkSetMacro( ProjectionAngle, double );
    //     itkGetMacro( ProjectionAngle, double );
  
    //-----------------------------------
    //Output image properties
    //-----------------------------------
    void SetOutputSize(const OutputSizeType& p)
    {
      if (m_OutputSize!=p)
	{
	  m_OutputSize=p;
	  m_IsInitialized=false;
	  this->Modified();
	}
    }
     
    /** Set the output image spacing. */
    void SetOutputSpacing(const OutputSpacingType & p)
    {
      if (m_OutputSpacing!=p)
	{
	  m_OutputSpacing=p;
	  m_IsInitialized=false;
	  this->Modified();
	}
    }
    
    /** Set the output image origin. */
    void SetOutputOrigin(const OutputPointType& p)
    {
      if (m_OutputOrigin!=p)
	{
	  m_OutputOrigin=p;
	  m_IsInitialized=false;
	  this->Modified();
	}
    }

    /** Helper method to set the output parameters based on this image */
    void SetOutputParametersFromImage( const OutputImagePointer image );

    /** Helper method to set the output parameters based on this image */
    void SetOutputParametersFromImage( const OutputImageConstPointer  image );


    void Initialize(void) throw (itk::ExceptionObject);

  protected:

    // Constructor
    BackProjectImageFilter();

    // Destructor
    ~BackProjectImageFilter(){};

    // Print the object
    void PrintSelf(std::ostream& os, itk::Indent indent) const;

    // Calculate the projection matrix
    void CalculateProjectionMatrix(void);

    // Calculate the coordinate increments
    void CalculateCoordinateIncrements( void );
 
    /** Overrides GenerateInputRequestedRegion() in order to inform
     * the pipeline execution model of different input requested regions
     * than the output requested region.
     * \sa ImageToImageFilter::GenerateInputRequestedRegion() */
    virtual void GenerateInputRequestedRegion();
    
    // Generate Output Information
    virtual void GenerateOutputInformation (void);

    // Threaded Generate Data
    void BeforeThreadedGenerateData(void );
  
    // Threaded Generate Data
    void ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread, int threadId );

 
    //------------------------------------------------
    //Member data
    //------------------------------------------------
    bool m_IsInitialized;
    
    // Input
    InputImagePointer m_ModifiedInput;

    // Projection geometry
    OutputPointType m_IsoCenter;
    itk::Matrix<double,3,4>  m_ProjectionMatrix;
    HomogeneInputVectorType m_LineInc, m_ColInc, m_PlaneInc;
    // Homogene2DVectorType m_LineInc, m_ColInc, m_PlaneInc;
    MatrixType  m_RigidTransformMatrix;
    double  m_SourceToScreen;
    double  m_SourceToAxis;
    OutputPixelType m_EdgePaddingValue;
    double m_ProjectionAngle;
    double m_PanelShift[2];

    // Output image info
    OutputSizeType                m_OutputSize;        // Size of the output image
    OutputSpacingType             m_OutputSpacing;     // output image spacing
    OutputPointType               m_OutputOrigin;      // output image origin
   
  private:
    BackProjectImageFilter( const Self& ); //purposely not implemented
    void operator=( const Self& ); //purposely not implemented


  };

} // namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkBackProjectImageFilter.txx"
#endif

#endif
