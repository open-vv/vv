#ifndef __clitkForwardWarpImageFilter_h
#define __clitkForwardWarpImageFilter_h

//clitk include
#include "clitkImageCommon.h"

//itk include
#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkNumericTraits.h"
#include "itkSimpleFastMutexLock.h"

namespace clitk
{
  
  template <  class InputImageType,  class OutputImageType,  class DeformationFieldType  >  
  class ForwardWarpImageFilter : public itk::ImageToImageFilter<InputImageType, OutputImageType>
  
  {
  public:
    typedef ForwardWarpImageFilter     Self;
    typedef itk::ImageToImageFilter<InputImageType,OutputImageType>     Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;

   
    /** Method for creation through the object factory. */
    itkNewMacro(Self);  
  
    /** Determine the image dimension. */
    itkStaticConstMacro(ImageDimension, unsigned int,
			InputImageType::ImageDimension );
    itkStaticConstMacro(InputImageDimension, unsigned int,
			OutputImageType::ImageDimension );
    itkStaticConstMacro(DeformationFieldDimension, unsigned int,
			DeformationFieldType::ImageDimension );


    //Some other typedefs
    typedef double CoordRepType;
    typedef itk::Image<double, ImageDimension> WeightsImageType;
    typedef itk::Image<itk::SimpleFastMutexLock, ImageDimension> MutexImageType;

    /** Point type */
    typedef itk::Point<CoordRepType,itkGetStaticConstMacro(ImageDimension)> PointType;

    /** Inherit some types from the superclass. */
    typedef typename OutputImageType::IndexType        IndexType;
    typedef typename OutputImageType::SizeType         SizeType;
    typedef typename OutputImageType::PixelType        PixelType;
    typedef typename OutputImageType::SpacingType      SpacingType;
    
    //Set & Get Methods (inline)
    itkSetMacro( Verbose, bool);
    itkSetMacro( EdgePaddingValue, PixelType );
    itkSetMacro( DeformationField, typename DeformationFieldType::Pointer);
    void SetNumberOfThreads(unsigned int r )
    {
      m_NumberOfThreadsIsGiven=true;
      m_NumberOfThreads=r;
    }
    itkSetMacro(ThreadSafe, bool);
 
  
    //ITK concept checking, why not?  
#ifdef ITK_USE_CONCEPT_CHECKING
    /** Begin concept checking */
    itkConceptMacro(SameDimensionCheck1,
		    (itk::Concept::SameDimension<ImageDimension, InputImageDimension>));
    itkConceptMacro(SameDimensionCheck2,
		    (itk::Concept::SameDimension<ImageDimension, DeformationFieldDimension>));
    itkConceptMacro(InputHasNumericTraitsCheck,
		    (itk::Concept::HasNumericTraits<typename InputImageType::PixelType>));
    itkConceptMacro(DeformationFieldHasNumericTraitsCheck,
		    (itk::Concept::HasNumericTraits<typename DeformationFieldType::PixelType::ValueType>));
    /** End concept checking */
#endif

  protected:
    ForwardWarpImageFilter();
    ~ForwardWarpImageFilter() {};
    void GenerateData( );

  
  private:
    bool m_Verbose;
    bool m_NumberOfThreadsIsGiven;
    unsigned int m_NumberOfThreads;
    PixelType m_EdgePaddingValue;
    typename DeformationFieldType::Pointer m_DeformationField;
    bool m_ThreadSafe;
 
  };





} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkForwardWarpImageFilter.txx"
#endif

#endif // #define __clitkForwardWarpImageFilter_h
