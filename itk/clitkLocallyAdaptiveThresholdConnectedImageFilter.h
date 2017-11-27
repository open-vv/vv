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
#ifndef __clitkLocallyAdaptiveThresholdConnectedImageFilter_h
#define __clitkLocallyAdaptiveThresholdConnectedImageFilter_h
#include "itkImage.h"
#include "itkImageToImageFilter.h"

namespace clitk {

/** \class LocallyAdaptiveThresholdConnectedImageFilter
 * \brief Label pixels that are connected to a seed and lie within a neighborhood
 * 
 * LocallyAdaptiveThresholdConnectedImageFilter labels pixels with ReplaceValue that
 * are connected to an initial Seed AND whose neighbors all lie within a
 * Lower and Upper threshold range.
 *
 * \ingroup RegionGrowingSegmentation 
 */
template <class TInputImage, class TOutputImage>
class ITK_EXPORT LocallyAdaptiveThresholdConnectedImageFilter:
    public itk::ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef LocallyAdaptiveThresholdConnectedImageFilter             Self;
  typedef itk::ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef itk::SmartPointer<Self>                           Pointer;
  typedef itk::SmartPointer<const Self>                     ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods).  */
  itkTypeMacro(LocallyAdaptiveThresholdConnectedImageFilter,
               ImageToImageFilter);

  typedef TInputImage                         InputImageType;
  typedef typename InputImageType::Pointer    InputImagePointer;
  typedef typename InputImageType::RegionType InputImageRegionType; 
  typedef typename InputImageType::PixelType  InputImagePixelType; 
  typedef typename InputImageType::IndexType  IndexType;
  typedef typename InputImageType::SizeType   InputImageSizeType;
  
  typedef TOutputImage                         OutputImageType;
  typedef typename OutputImageType::Pointer    OutputImagePointer;
  typedef typename OutputImageType::RegionType OutputImageRegionType; 
  typedef typename OutputImageType::PixelType  OutputImagePixelType; 
  
  void PrintSelf ( std::ostream& os, itk::Indent indent ) const ITK_OVERRIDE;

  /** Clear the seeds */
  void ClearSeeds();

  /** Set seed point. */
  void SetSeed(const IndexType & seed);

  /** Add a seed point */
  void AddSeed ( const IndexType & seed );

  // Set/Get the lower threshold. The default is 0.
  itkSetMacro(Lower, InputImagePixelType);
  itkGetConstMacro(Lower, InputImagePixelType);

  // Set/Get the upper threshold. The default is the largest possible value for the InputPixelType.
  itkSetMacro(Upper, InputImagePixelType);
  itkGetConstMacro(Upper, InputImagePixelType);
  
  /** Set/Get value to replace thresholded pixels. Pixels that lie *
   *  within Lower and Upper (inclusive) will be replaced with this
   *  value. The default is 1. */
  itkSetMacro(ReplaceValue, OutputImagePixelType);
  itkGetConstMacro(ReplaceValue, OutputImagePixelType);

  /** Set the radius of the neighborhood used for a mask. */
  itkSetMacro(Radius, InputImageSizeType);

  /** Get the radius of the neighborhood used to compute the median */
  itkGetConstReferenceMacro(Radius, InputImageSizeType);

  /** ImageDimension constants */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(InputEqualityComparableCheck,
		  (itk::Concept::EqualityComparable<InputImagePixelType>));
  itkConceptMacro(OutputEqualityComparableCheck,
		  (itk::Concept::EqualityComparable<OutputImagePixelType>));
  itkConceptMacro(SameDimensionCheck,
		  (itk::Concept::SameDimension<InputImageDimension, OutputImageDimension>));
  itkConceptMacro(InputOStreamWritableCheck,
		  (itk::Concept::OStreamWritable<InputImagePixelType>));
  itkConceptMacro(OutputOStreamWritableCheck,
		  (itk::Concept::OStreamWritable<OutputImagePixelType>));
  /** End concept checking */
#endif

  // JV
  itkBooleanMacro(LowerBorderIsGiven);
  itkSetMacro( LowerBorderIsGiven, bool);
  itkGetConstReferenceMacro( LowerBorderIsGiven, bool);
  itkBooleanMacro(UpperBorderIsGiven);
  itkSetMacro( UpperBorderIsGiven, bool);
  itkGetConstReferenceMacro( UpperBorderIsGiven, bool);
  itkSetMacro( Multiplier, double);
  itkGetConstMacro( Multiplier, double);
  itkBooleanMacro(MaximumSDIsGiven);
  itkSetMacro( MaximumSDIsGiven, bool);
  itkGetConstReferenceMacro( MaximumSDIsGiven, bool);
  itkSetMacro( MaximumSD, double);
  itkGetConstMacro( MaximumSD, double);

protected:
  LocallyAdaptiveThresholdConnectedImageFilter();
  ~LocallyAdaptiveThresholdConnectedImageFilter(){};
  std::vector<IndexType> m_Seeds;
  InputImagePixelType    m_Lower;
  InputImagePixelType    m_Upper;
  OutputImagePixelType   m_ReplaceValue;
  InputImageSizeType     m_Radius;

  // JV 
  bool m_LowerBorderIsGiven;
  bool m_UpperBorderIsGiven;
  bool m_MaximumSDIsGiven;
  double m_Multiplier;
  double m_MaximumSD;

  // Override since the filter needs all the data for the algorithm
  void GenerateInputRequestedRegion() ITK_OVERRIDE;

  // Override since the filter produces the entire dataset
  void EnlargeOutputRequestedRegion(itk::DataObject *output) ITK_OVERRIDE;
  void GenerateData() ITK_OVERRIDE;
  
private:
  LocallyAdaptiveThresholdConnectedImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkLocallyAdaptiveThresholdConnectedImageFilter.txx"
#endif

#endif
