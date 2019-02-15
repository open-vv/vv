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
#ifndef __clitkForwardWarpImageFilter_h
#define __clitkForwardWarpImageFilter_h
#include "clitkImageCommon.h"

//itk include
#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkNumericTraits.h"
#if ITK_VERSION_MAJOR <= 4
#include "itkSimpleFastMutexLock.h"
#else
#include <mutex>
#endif

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
#if ITK_VERSION_MAJOR <= 4
    typedef itk::Image<itk::SimpleFastMutexLock, ImageDimension> MutexImageType;
#endif

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
#if ITK_VERSION_MAJOR <= 4
    void SetNumberOfThreads(unsigned int r )
#else
    void SetNumberOfWorkUnits(unsigned int r )
#endif
    {
      m_NumberOfThreadsIsGiven=true;
#if ITK_VERSION_MAJOR <= 4
      m_NumberOfThreads=r;
#else
      m_NumberOfWorkUnits=r;
#endif
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
#if ITK_VERSION_MAJOR <= 4
    unsigned int m_NumberOfThreads;
#else
    unsigned int m_NumberOfWorkUnits;
#endif
    PixelType m_EdgePaddingValue;
    typename DeformationFieldType::Pointer m_DeformationField;
    bool m_ThreadSafe;
 
  };





} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkForwardWarpImageFilter.txx"
#endif

#endif // #define __clitkForwardWarpImageFilter_h
