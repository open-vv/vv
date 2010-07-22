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
#ifndef clitkReconstructThroughDilationImageFilter_h
#define clitkReconstructThroughDilationImageFilter_h

/* =================================================
 * @file   clitkReconstructThroughDilationImageFilter.h
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk include
#include "clitkIO.h"
#include "clitkCommon.h"
#include "clitkConditionalBinaryDilateImageFilter.h"

//itk include
#include "itkImageToImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkDifferenceImageFilter.h"
#include "itkThresholdImageFilter.h"

namespace clitk 
{

  template <class InputImageType, class OutputImageType>
  class ITK_EXPORT ReconstructThroughDilationImageFilter :
    public itk::ImageToImageFilter<InputImageType, OutputImageType>
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef ReconstructThroughDilationImageFilter                                                 Self;
    typedef itk::ImageToImageFilter<InputImageType, OutputImageType>  Superclass;
    typedef itk::SmartPointer<Self>                                   Pointer;
    typedef itk::SmartPointer<const Self>                             ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( ReconstructThroughDilationImageFilter, ImageToImageFilter );

    /** Dimension of the domain space. */
    itkStaticConstMacro(InputImageDimension, unsigned int, Superclass::InputImageDimension);
    itkStaticConstMacro(OutputImageDimension, unsigned int, Superclass::OutputImageDimension);

    //----------------------------------------
    // Typedefs
    //----------------------------------------
    typedef typename OutputImageType::RegionType OutputImageRegionType;
    typedef int InternalPixelType;
    typedef typename InputImageType::PixelType InputPixelType;
    typedef typename OutputImageType::PixelType OutputPixelType;
    typedef typename InputImageType::SizeType SizeType;

    //----------------------------------------
    // Set & Get
    //----------------------------------------    
    itkBooleanMacro(Verbose);
    itkSetMacro( Verbose, bool);
    itkGetConstReferenceMacro( Verbose, bool);
    void SetRadius ( const SizeType& s){ m_Radius=s; this->Modified();}
    SizeType GetRadius(void){return m_Radius;}
    itkSetMacro( ErosionPaddingValue, OutputPixelType);
    itkGetConstMacro( ErosionPaddingValue, OutputPixelType)
    itkSetMacro( MaximumNumberOfLabels, unsigned int);
    itkGetConstMacro( MaximumNumberOfLabels, unsigned int);
    itkSetMacro( BackgroundValue, InternalPixelType);
    itkGetConstMacro( BackgroundValue, InternalPixelType);
    itkSetMacro( ForegroundValue, InternalPixelType);
    itkGetConstMacro( ForegroundValue, InternalPixelType);

  protected:

    //----------------------------------------  
    // Constructor & Destructor
    //----------------------------------------  
    ReconstructThroughDilationImageFilter();
    ~ReconstructThroughDilationImageFilter() {};

    //----------------------------------------  
    // Update
    //----------------------------------------  
    // Generate Data
    void GenerateData(void);

    //     // Threaded Generate Data
    //     void BeforeThreadedGenerateData(void );
    //     void ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread, int threadId );
    //     void AfterThreadedGenerateData(void );
    //     // Override defaults
    //     virtual void GenerateInputRequestedRegion();
    //     virtual void GenerateOutputInformation (void);
    //     virtual void EnlargeOutputRequestedRegion(DataObject *data);
    //     void AllocateOutputs();
    //----------------------------------------  
    // Data members
    //----------------------------------------
    bool m_Verbose;
    InternalPixelType m_BackgroundValue;
    InternalPixelType m_ForegroundValue;
    unsigned int m_MaximumNumberOfLabels;
    OutputPixelType m_ErosionPaddingValue;
    SizeType m_Radius;

  };


} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkReconstructThroughDilationImageFilter.txx"
#endif

#endif // #define clitkReconstructThroughDilationImageFilter_h


