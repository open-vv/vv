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
#ifndef clitkDecomposeAndReconstructImageFilter_h
#define clitkDecomposeAndReconstructImageFilter_h

/* =================================================
 * @file   clitkDecomposeAndReconstructImageFilter.h
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk include
#include "clitkIO.h"
#include "clitkCommon.h"
#include "clitkDecomposeThroughErosionImageFilter.h"
#include "clitkReconstructThroughDilationImageFilter.h"

//itk include
#include "itkImageToImageFilter.h"
#include "itkRelabelComponentImageFilter.h"


namespace clitk 
{

  template <class InputImageType, class OutputImageType>
  class ITK_EXPORT DecomposeAndReconstructImageFilter :
    public itk::ImageToImageFilter<InputImageType, OutputImageType>
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef DecomposeAndReconstructImageFilter                                                 Self;
    typedef itk::ImageToImageFilter<InputImageType, OutputImageType>  Superclass;
    typedef itk::SmartPointer<Self>                                   Pointer;
    typedef itk::SmartPointer<const Self>                             ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( DecomposeAndReconstructImageFilter, ImageToImageFilter );

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
    itkBooleanMacro(FullyConnected);
    itkSetMacro( FullyConnected, bool);
    itkGetConstReferenceMacro( FullyConnected, bool);
    void SetRadius ( const SizeType& s){ m_Radius=s; this->Modified();}
    SizeType GetRadius(void){return m_Radius;}
    itkSetMacro( MaximumNumberOfLabels, unsigned int);
    itkGetConstMacro( MaximumNumberOfLabels, unsigned int);
    itkSetMacro( BackgroundValue, InternalPixelType);
    itkGetConstMacro( BackgroundValue, InternalPixelType);
    itkSetMacro( ForegroundValue, InternalPixelType);
    itkGetConstMacro( ForegroundValue, InternalPixelType);
    itkSetMacro( NumberOfNewLabels, unsigned int);
    itkGetConstMacro( NumberOfNewLabels, unsigned int);
    itkSetMacro( MinimumObjectSize, unsigned int);
    itkGetConstMacro( MinimumObjectSize, unsigned int);
    itkSetMacro( MinimumNumberOfIterations, unsigned int);
    itkGetConstMacro( MinimumNumberOfIterations, unsigned int);
    //     // Convenience macro's: Built-in
    //     itkBooleanMacro (flag); //FlagOn FlagOff
    //     itkGetMacro(name, type);
    //     itkSetMacro(name, type);
    //     itkSetConstMacro( name, type);
    //     itkGetConstMacro( name, type);
    //     itkSetConstReferenceMacro(name, type);
    //     itkGetConstReferenceMacro(name, type);
    //     // Convenience macro's: Smartpointers
    //     itkSetObjectMacro(name, type); 
    //     itkGetObjectMacro(name, type); 
    //     itkSetConstObjectMacro(name, type); 
    //     itkGetConstObjectMacro(name, type); 
    //     itkSetConstReferenceObjectMacro(name, type); 
    //     itkSetConstReference(name, type);
    

  protected:

    //----------------------------------------  
    // Constructor & Destructor
    //----------------------------------------  
    DecomposeAndReconstructImageFilter();
    ~DecomposeAndReconstructImageFilter() {};

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
    void AllocateOutputs(){;}
    //----------------------------------------  
    // Data members
    //----------------------------------------
    bool m_Verbose;
    SizeType m_Radius;
    unsigned int m_NumberOfNewLabels;
    bool m_FullyConnected;
    InputPixelType m_BackgroundValue;
    InputPixelType m_ForegroundValue;
    unsigned int  m_MaximumNumberOfLabels;
    unsigned int m_MinimumObjectSize;
    unsigned int m_MinimumNumberOfIterations;
  };


} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkDecomposeAndReconstructImageFilter.txx"
#endif

#endif // #define clitkDecomposeAndReconstructImageFilter_h


