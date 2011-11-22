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
#ifndef __clitkComposeVFFilter_h
#define __clitkComposeVFFilter_h
#include "clitkImageCommon.h"

#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkNumericTraits.h"

namespace clitk
{
  
  template <class InputImageType, class OutputImageType>  
  class ITK_EXPORT ComposeVFFilter : public itk::ImageToImageFilter<InputImageType, OutputImageType>
  
  {
  public:
    typedef ComposeVFFilter     Self;
    typedef itk::ImageToImageFilter<InputImageType, OutputImageType>     Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    /** Method for creation through the object factory. */
    itkNewMacro(Self);  

    /** Run-time type information (and related methods) */
    itkTypeMacro( ComposeVFFilter, ImageToImageFilter );
  
    /** Determine the image dimension. */
    itkStaticConstMacro(ImageDimension, unsigned int,
			InputImageType::ImageDimension );

    /** Typedef to describe the output image region type. */
    typedef typename OutputImageType::RegionType OutputImageRegionType;
    
    //========================================================================================
    //typedefs
    typedef double CoordRepType;
    typedef typename OutputImageType::PixelType PixelType;
    typedef itk::Point<CoordRepType, ImageDimension> PointType;

    //Set Methods(inline)
    void SetEdgePaddingValue(PixelType m);
    void SetVerbose(bool m){m_Verbose=m;}
    void SetInput1(typename InputImageType::Pointer m)
    {
      //call the superclass method (at least one input required!)
      this->SetInput(m);
      m_Input1=m;
    }
    void SetInput2(typename InputImageType::Pointer m){m_Input2=m;}

  protected:
    ComposeVFFilter();
    ~ComposeVFFilter() {};


    //========================================================================================
    //Threaded execution should implement generate threaded data
    void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, itk::ThreadIdType threadId );
  
    bool m_Verbose;
    PixelType m_EdgePaddingValue;
    typename InputImageType::Pointer m_Input1;
    typename InputImageType::Pointer m_Input2;
  };


} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkComposeVFFilter.txx"
#endif

#endif // #define __clitkComposeVFFilter_h
