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
#ifndef __clitkInvertVFFilter_h
#define __clitkInvertVFFilter_h
#include "clitkImageCommon.h"

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
    void SetNumberOfThreads(unsigned int r ) ITK_OVERRIDE
    {
      m_NumberOfThreadsIsGiven=true;
      m_NumberOfThreads=r;
    }
    itkSetMacro(ThreadSafe, bool);
    itkSetMacro(OutputSpacing, SpacingType);
    itkSetMacro(OutputSize, SizeType);

  
  protected:
    InvertVFFilter();
    ~InvertVFFilter() {};
    void GenerateData( ) ITK_OVERRIDE;
    
    bool m_Verbose;
    bool m_NumberOfThreadsIsGiven;
    SpacingType m_OutputSpacing;
    SizeType m_OutputSize;
    unsigned int m_NumberOfThreads;
    PixelType m_EdgePaddingValue;
    bool m_ThreadSafe;
  };


} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkInvertVFFilter.txx"
#endif

#endif // #define __clitkInvertVFFilter_h
