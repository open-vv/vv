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
#ifndef __clitkVectorBSplineDecompositionImageFilterWithOBD_h
#define __clitkVectorBSplineDecompositionImageFilterWithOBD_h
#include <vector>

#include "itkImageLinearIteratorWithIndex.h"
#include "vnl/vnl_matrix.h"

#include "itkImageToImageFilter.h"

namespace clitk
{

template <class TInputImage, class TOutputImage>
class ITK_EXPORT VectorBSplineDecompositionImageFilterWithOBD : 
    public itk::ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef VectorBSplineDecompositionImageFilterWithOBD       Self;
  typedef itk::ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef itk::SmartPointer<Self>                    Pointer;
  typedef itk::SmartPointer<const Self>              ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(VectorBSplineDecompositionImageFilterWithOBD, ImageToImageFilter);
 
  /** New macro for creation of through a Smart Pointer */
  itkNewMacro( Self );

  /** Inherit input and output image types from Superclass. */
  typedef typename Superclass::InputImageType InputImageType;
  typedef typename Superclass::InputImagePointer InputImagePointer;
  typedef typename Superclass::InputImageConstPointer InputImageConstPointer;
  typedef typename Superclass::OutputImagePointer OutputImagePointer;

  /** Dimension underlying input image. */
  itkStaticConstMacro(ImageDimension, unsigned int,TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,TOutputImage::ImageDimension);

  //JV add the size 
  typedef typename InputImageType::SizeType SizeType;

  //JV vector dimension
  itkStaticConstMacro(VectorDimension, unsigned int,TInputImage::PixelType::Dimension);

  /** Iterator typedef support */
  typedef itk::ImageLinearIteratorWithIndex<TOutputImage> OutputLinearIterator;

  /** Get/Sets the Spline Order, supports 0th - 5th order splines. The default
   *  is a 3rd order spline. */
  void SetSplineOrder(unsigned int SplineOrder);
  itkGetMacro(SplineOrder, int);
  //JV  Set the order by Dimension
  void SetSplineOrders(SizeType);

  //JV modified from the original "double" version
#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(DimensionCheck,
		  (itk::Concept::SameDimension<ImageDimension, OutputImageDimension>));
  itkConceptMacro(InputConvertibleToDoubleCheck,
		  (itk::Concept::Convertible<typename TInputImage::PixelType, itk::Vector<double, VectorDimension> >));
  itkConceptMacro(OutputConvertibleToDoubleCheck,
		  (itk::Concept::Convertible<typename TOutputImage::PixelType, itk::Vector<double, VectorDimension> >));
  itkConceptMacro(InputConvertibleToOutputCheck,
		  (itk::Concept::Convertible<typename TInputImage::PixelType,
                           typename TOutputImage::PixelType>));
  itkConceptMacro(DoubleConvertibleToOutputCheck,
		  (itk::Concept::Convertible<itk::Vector<double, VectorDimension>, typename TOutputImage::PixelType>));
  /** End concept checking */
#endif

protected:
  VectorBSplineDecompositionImageFilterWithOBD();
  virtual ~VectorBSplineDecompositionImageFilterWithOBD() {};
  void PrintSelf(std::ostream& os, itk::Indent indent) const;

  void GenerateData( );

  /** This filter requires all of the input image. */
  void GenerateInputRequestedRegion();

  /** This filter must produce all of its output at once. */
  void EnlargeOutputRequestedRegion( itk::DataObject *output ); 

  /** These are needed by the smoothing spline routine. */
  //JV multiple splineOrders
  SizeType                  m_SplineOrders; //SplineOrder by dimension 

  //JV
  std::vector< itk::Vector<double, VectorDimension> >       m_Scratch;       // temp storage for processing of Coefficients
  typename TInputImage::SizeType   m_DataLength;  // Image size
  unsigned int              m_SplineOrder;   // User specified spline order (3rd or cubic is the default)
  double                    m_SplinePoles[3];// Poles calculated for a given spline order
  int                       m_NumberOfPoles; // number of poles
  double                    m_Tolerance;     // Tolerance used for determining initial causal coefficient
  unsigned int              m_IteratorDirection; // Direction for iterator incrementing


private:
  VectorBSplineDecompositionImageFilterWithOBD( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  /** Determines the poles given the Spline Order. */
  virtual void SetPoles();

  /** Converts a vector of data to a vector of Spline coefficients. */
  virtual bool DataToCoefficients1D();

  /** Converts an N-dimension image of data to an equivalent sized image
   *    of spline coefficients. */
  void DataToCoefficientsND();

  /** Determines the first coefficient for the causal filtering of the data. */
  virtual void SetInitialCausalCoefficient(double z);

  /** Determines the first coefficient for the anti-causal filtering of the data. */
  virtual void SetInitialAntiCausalCoefficient(double z);

  /** Used to initialize the Coefficients image before calculation. */
  void CopyImageToImage();

  /** Copies a vector of data from the Coefficients image to the m_Scratch vector. */
  void CopyCoefficientsToScratch( OutputLinearIterator & );

  /** Copies a vector of data from m_Scratch to the Coefficients image. */
  void CopyScratchToCoefficients( OutputLinearIterator & );
  
};


} // namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkVectorBSplineDecompositionImageFilterWithOBD.txx"
#endif

#endif

