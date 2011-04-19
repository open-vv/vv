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
#ifndef clitkResampleBSplineDeformableTransformImageFilter_h
#define clitkResampleBSplineDeformableTransformImageFilter_h

/* =================================================
 * @file   clitkResampleBSplineDeformableTransformImageFilter.h
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk include
#include "clitkIO.h"
#include "clitkCommon.h"
#include "clitkVectorBSplineDecompositionImageFilterWithOBD.h"
#include "clitkVectorBSplineResampleImageFunction.h"

//itk include
#include "itkImageToImageFilter.h"
#include "itkVectorResampleImageFilter.h"
#include "itkCastImageFilter.h"


namespace clitk 
{

  template <class InputImageType, class OutputImageType>
  class ITK_EXPORT ResampleBSplineDeformableTransformImageFilter :
    public itk::ImageToImageFilter<InputImageType, OutputImageType>
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef ResampleBSplineDeformableTransformImageFilter                                                 Self;
    typedef itk::ImageToImageFilter<InputImageType, OutputImageType>  Superclass;
    typedef itk::SmartPointer<Self>                                   Pointer;
    typedef itk::SmartPointer<const Self>                             ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( ResampleBSplineDeformableTransformImageFilter, ImageToImageFilter );

    /** Dimension of the domain space. */
    itkStaticConstMacro(InputImageDimension, unsigned int, Superclass::InputImageDimension);

    //----------------------------------------
    // Typedefs
    //----------------------------------------
    typedef typename InputImageType::PointType              InputPointType;
    typedef typename InputImageType::IndexType              IndexType;
    
    typedef typename OutputImageType::RegionType            OutputImageRegionType;
    typedef typename OutputImageType::PointType             OutputPointType;
    typedef typename OutputImageType::SpacingType           OutputSpacingType;
    typedef typename OutputImageType::DirectionType         OutputDirectionType;
    typedef typename OutputImageType::IndexType             OutputIndexType;
    typedef typename OutputImageType::SizeType             OutputSizeType;


    //----------------------------------------
    // Set & Get
    //----------------------------------------    
    itkBooleanMacro(Verbose);
    itkSetMacro( Verbose, bool);
    itkGetConstReferenceMacro( Verbose, bool);

    // Properties of the output 
    itkSetMacro( Size, OutputSizeType );
    itkGetConstReferenceMacro( Size, OutputSizeType );
    itkSetMacro(OutputSpacing, OutputSpacingType);
    virtual void SetOutputSpacing( const double* values);
    itkGetConstReferenceMacro( OutputSpacing, OutputSpacingType );
    itkSetMacro(OutputOrigin, OutputPointType);
    virtual void SetOutputOrigin( const double* values);
    itkGetConstReferenceMacro( OutputOrigin, OutputPointType );
    itkSetMacro(OutputDirection, OutputDirectionType);
    itkGetConstReferenceMacro(OutputDirection, OutputDirectionType);
    itkSetMacro( OutputStartIndex, OutputIndexType );
    itkGetConstReferenceMacro( OutputStartIndex, OutputIndexType );
    void SetOutputParametersFromImage ( typename OutputImageType::Pointer Image )
    {
      this->SetOutputOrigin ( Image->GetOrigin() );
      this->SetOutputSpacing ( Image->GetSpacing() );
      this->SetOutputDirection ( Image->GetDirection() );
      this->SetSize ( Image->GetLargestPossibleRegion().GetSize() );
      this->SetOutputStartIndex(Image->GetLargestPossibleRegion().GetIndex());
    }

    void SetOutputParametersFromConstImage ( typename OutputImageType::ConstPointer Image )
    {
      this->SetOutputOrigin ( Image->GetOrigin() );
      this->SetOutputSpacing ( Image->GetSpacing() );
      this->SetOutputDirection ( Image->GetDirection() );
      this->SetSize ( Image->GetLargestPossibleRegion().GetSize() );
      this->SetOutputStartIndex(Image->GetLargestPossibleRegion().GetIndex());
    }
   

  protected:

    //----------------------------------------  
    // Constructor & Destructor
    //----------------------------------------  
    ResampleBSplineDeformableTransformImageFilter();
    ~ResampleBSplineDeformableTransformImageFilter() {};

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
    void AllocateOutputs(){};

    //----------------------------------------  
    // Data members
    //----------------------------------------
    bool m_Verbose;
    OutputSizeType                m_Size;              // Size of the output image
    OutputSpacingType             m_OutputSpacing;     // output image spacing
    OutputPointType               m_OutputOrigin;      // output image origin
    OutputDirectionType           m_OutputDirection;   // output image direction cosines
    OutputIndexType               m_OutputStartIndex;  // output image start index
    OutputSizeType                m_SplineOrders;

    //----------------------------------------  
    // Components
    //----------------------------------------
    typedef itk::VectorResampleImageFilter<InputImageType, OutputImageType> ResamplerType;
    typedef VectorBSplineResampleImageFunction<InputImageType, double> FunctionType;
    typedef itk::IdentityTransform<double,InputImageDimension> IdentityTransformType; 
    typedef VectorBSplineDecompositionImageFilterWithOBD<OutputImageType, OutputImageType> DecompositionType;
    typename ResamplerType::Pointer m_Resampler;
    typename FunctionType::Pointer m_Function;
    typename IdentityTransformType::Pointer m_Identity;
    typename DecompositionType::Pointer m_Decomposition;


  };


} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkResampleBSplineDeformableTransformImageFilter.txx"
#endif

#endif // #define clitkResampleBSplineDeformableTransformImageFilter_h


