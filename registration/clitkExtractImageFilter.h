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
#ifndef __clitkExtractImageFilter_h
#define __clitkExtractImageFilter_h
#include "itkImageToImageFilter.h"
#include "itkSmartPointer.h"
#include "itkExtractImageFilterRegionCopier.h"

namespace clitk
{

template <class TInputImage, class TOutputImage>
class ITK_EXPORT ExtractImageFilter:
    public itk::ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef ExtractImageFilter         Self;
  typedef itk::ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Run-time type information (and related methods). */
  itkTypeMacro(ExtractImageFilter, ImageToImageFilter);

  /** Image type information. */
  typedef TInputImage  InputImageType;
  typedef TOutputImage OutputImageType;

  /** Typedef to describe the output and input image region types. */
  typedef typename TOutputImage::RegionType OutputImageRegionType;
  typedef typename TInputImage::RegionType InputImageRegionType;

  /** Typedef to describe the type of pixel. */
  typedef typename TOutputImage::PixelType OutputImagePixelType;
  typedef typename TInputImage::PixelType InputImagePixelType;

  /** Typedef to describe the output and input image index and size types. */
  typedef typename TOutputImage::IndexType OutputImageIndexType;
  typedef typename TInputImage::IndexType InputImageIndexType;
  typedef typename TOutputImage::SizeType OutputImageSizeType;
  typedef typename TInputImage::SizeType InputImageSizeType;

  /** ImageDimension enumeration */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  typedef itk::ImageToImageFilterDetail::ExtractImageFilterRegionCopier<
    itkGetStaticConstMacro(InputImageDimension), 
    itkGetStaticConstMacro(OutputImageDimension)> ExtractImageFilterRegionCopierType;

  void SetExtractionRegion(InputImageRegionType extractRegion);
  itkGetMacro(ExtractionRegion, InputImageRegionType);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(InputCovertibleToOutputCheck,
		  (itk::Concept::Convertible<InputImagePixelType, OutputImagePixelType>));
  /** End concept checking */
#endif

protected:
  ExtractImageFilter();
  ~ExtractImageFilter() {};
  void PrintSelf(std::ostream& os, itk::Indent indent) const;


  virtual void GenerateOutputInformation();


  virtual void CallCopyOutputRegionToInputRegion(InputImageRegionType &destRegion,
                                                 const OutputImageRegionType &srcRegion);


  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                            itk::ThreadIdType threadId );
  InputImageRegionType m_ExtractionRegion;
  OutputImageRegionType m_OutputImageRegion;

private:
  ExtractImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
};

  
} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkExtractImageFilter.txx"
#endif
  
#endif
