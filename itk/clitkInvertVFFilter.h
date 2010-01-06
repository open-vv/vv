#ifndef __clitkInvertVFFilter_h
#define __clitkInvertVFFilter_h

//clitk include
#include "clitkIOCommon.h"

#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkNumericTraits.h"

namespace clitk
{
  
  template <class InputImageType, class OutputImageType>  
  class ITK_EXPORT InvertVFFilter : public itk::ImageToImageFilter<InputImageType, OutputImageType>
  
  {
  public:
    typedef InvertVFFilter     Self;
    typedef itk::ImageToImageFilter<InputImageType, OutputImageType>     Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    /** Method for creation through the object factory. */
    itkNewMacro(Self);  

    /** Run-time type information (and related methods) */
    itkTypeMacro( InvertVFFilter, ImageToImageFilter );
  
    /** Determine the image dimension. */
    itkStaticConstMacro(ImageDimension, unsigned int,
			InputImageType::ImageDimension );
    
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

    //Set Methods(inline)
    itkSetMacro( Verbose, bool);
    itkSetMacro( EdgePaddingValue, PixelType );
    void SetNumberOfThreads(unsigned int r )
    {
      m_NumberOfThreadsIsGiven=true;
      m_NumberOfThreads=r;
    }
    itkSetMacro(ThreadSafe, bool);

  
  protected:
    InvertVFFilter();
    ~InvertVFFilter() {};
    void GenerateData( );
   
    bool m_Verbose;
    bool m_NumberOfThreadsIsGiven;
    unsigned int m_NumberOfThreads;
    PixelType m_EdgePaddingValue;
    bool m_ThreadSafe;
  };


} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkInvertVFFilter.txx"
#endif

#endif // #define __clitkInvertVFFilter_h
