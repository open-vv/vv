#ifndef __clitkVectorBSplineInterpolateImageFunction_h
#define __clitkVectorBSplineInterpolateImageFunction_h
#include "clitkVectorBSplineDecompositionImageFilter.h"

// First make sure that the configuration is available.
// This line can be removed once the optimized versions
// gets integrated into the main directories.
#include "itkConfigure.h"

// #ifdef ITK_USE_OPTIMIZED_REGISTRATION_METHODS
// #include "itkOptBSplineInterpolateImageFunction.h"
// #else

#include <vector>

#include "itkImageLinearIteratorWithIndex.h"
#include "itkVectorInterpolateImageFunction.h"
#include "vnl/vnl_matrix.h"

#include "itkBSplineDecompositionImageFilter.h"
#include "itkConceptChecking.h"
#include "itkCovariantVector.h"


namespace clitk
{

template <
  class TImageType, 
  class TCoordRep = double,
  class TCoefficientType = double >
class ITK_EXPORT VectorBSplineInterpolateImageFunction : 
    public itk::VectorInterpolateImageFunction<TImageType,TCoordRep> 
{
public:
  /** Standard class typedefs. */
  typedef VectorBSplineInterpolateImageFunction       Self;
  typedef itk::VectorInterpolateImageFunction<TImageType,TCoordRep>  Superclass;
  typedef itk::SmartPointer<Self>                    Pointer;
  typedef itk::SmartPointer<const Self>              ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(VectorBSplineInterpolateImageFunction, InterpolateImageFunction);


  /** New macro for creation of through a Smart Pointer */
  itkNewMacro( Self );

  /** OutputType typedef support. */
  typedef typename Superclass::OutputType OutputType;

  /** InputImageType typedef support. */
  typedef typename Superclass::InputImageType InputImageType;

  /** Dimension underlying input image. */
  itkStaticConstMacro(ImageDimension, unsigned int,Superclass::ImageDimension);

  /** Index typedef support. */
  typedef typename Superclass::IndexType IndexType;

  /** ContinuousIndex typedef support. */
  typedef typename Superclass::ContinuousIndexType ContinuousIndexType;

  /** PointType typedef support */
  typedef typename Superclass::PointType PointType;

  //JV the vector dimension
  itkStaticConstMacro(VectorDimension, unsigned int,Superclass::Dimension);


  /** Iterator typedef support */
  typedef itk::ImageLinearIteratorWithIndex<TImageType> Iterator;

  /** Internal Coefficient typedef support */
  typedef TCoefficientType CoefficientDataType;

  //JV
  typedef itk::Vector<CoefficientDataType, itkGetStaticConstMacro(VectorDimension)> CoefficientImagePixelType;
  typedef  itk::Image<CoefficientImagePixelType , itkGetStaticConstMacro(ImageDimension) > CoefficientImageType;

  /** Define filter for calculating the BSpline coefficients */
  //JV make vectorial
  typedef clitk::VectorBSplineDecompositionImageFilter<TImageType, CoefficientImageType> 
  CoefficientFilter;
  typedef typename CoefficientFilter::Pointer CoefficientFilterPointer;

  /** Evaluate the function at a ContinuousIndex position.
   *
   * Returns the B-Spline interpolated image intensity at a 
   * specified point position. No bounds checking is done.
   * The point is assume to lie within the image buffer.
   *
   * ImageFunction::IsInsideBuffer() can be used to check bounds before
   * calling the method. */
  virtual OutputType EvaluateAtContinuousIndex( 
    const ContinuousIndexType & index ) const; 

  /** Derivative typedef support */
  typedef itk::CovariantVector<OutputType,
                          itkGetStaticConstMacro(ImageDimension)
    > CovariantVectorType;

  CovariantVectorType EvaluateDerivative( const PointType & point ) const
  {    
    ContinuousIndexType index;
    this->GetInputImage()->TransformPhysicalPointToContinuousIndex( point, index );
    return ( this->EvaluateDerivativeAtContinuousIndex( index ) );
  } 

  CovariantVectorType EvaluateDerivativeAtContinuousIndex( 
    const ContinuousIndexType & x ) const;


  /** Get/Sets the Spline Order, supports 0th - 5th order splines. The default
   *  is a 3rd order spline. */
  void SetSplineOrder(unsigned int SplineOrder);
  itkGetMacro(SplineOrder, int);


  /** Set the input image.  This must be set by the user. */
  virtual void SetInputImage(const TImageType * inputData);


  /** The UseImageDirection flag determines whether image derivatives are
   * computed with respect to the image grid or with respect to the physical
   * space. When this flag is ON the derivatives are computed with respect to
   * the coodinate system of physical space. The difference is whether we take
   * into account the image Direction or not. The flag ON will take into
   * account the image direction and will result in an extra matrix
   * multiplication compared to the amount of computation performed when the
   * flag is OFF.  This flag is OFF by default.*/
  itkSetMacro( UseImageDirection, bool );
  itkGetMacro( UseImageDirection, bool );
  itkBooleanMacro( UseImageDirection );


protected:
  VectorBSplineInterpolateImageFunction();
  virtual ~VectorBSplineInterpolateImageFunction() {};
  void operator=( const Self& ); //purposely not implemented
  void PrintSelf(std::ostream& os, itk::Indent indent) const;

  // These are needed by the smoothing spline routine.
  std::vector<CoefficientDataType>    m_Scratch;        // temp storage for processing of Coefficients
  typename TImageType::SizeType       m_DataLength;  // Image size
  unsigned int                        m_SplineOrder; // User specified spline order (3rd or cubic is the default)

  typename CoefficientImageType::ConstPointer       m_Coefficients; // Spline coefficients  

private:
  VectorBSplineInterpolateImageFunction( const Self& ); //purposely not implemented
  /** Determines the weights for interpolation of the value x */
  void SetInterpolationWeights( const ContinuousIndexType & x, 
                                const vnl_matrix<long> & EvaluateIndex, 
                                vnl_matrix<double> & weights, 
                                unsigned int splineOrder ) const;

  /** Determines the weights for the derivative portion of the value x */
  void SetDerivativeWeights( const ContinuousIndexType & x, 
                             const vnl_matrix<long> & EvaluateIndex, 
                             vnl_matrix<double> & weights, 
                             unsigned int splineOrder ) const;

  /** Precomputation for converting the 1D index of the interpolation neighborhood 
    * to an N-dimensional index. */
  void GeneratePointsToIndex(  );

  /** Determines the indicies to use give the splines region of support */
  void DetermineRegionOfSupport( vnl_matrix<long> & evaluateIndex, 
                                 const ContinuousIndexType & x, 
                                 unsigned int splineOrder ) const;

  /** Set the indicies in evaluateIndex at the boundaries based on mirror 
    * boundary conditions. */
  void ApplyMirrorBoundaryConditions(vnl_matrix<long> & evaluateIndex, 
                                     unsigned int splineOrder) const;


  Iterator                  m_CIterator;    // Iterator for traversing spline coefficients.
  unsigned long             m_MaxNumberInterpolationPoints; // number of neighborhood points used for interpolation
  std::vector<IndexType>    m_PointsToIndex;  // Preallocation of interpolation neighborhood indicies

  CoefficientFilterPointer     m_CoefficientFilter;
  
  // flag to take or not the image direction into account when computing the
  // derivatives.
  bool m_UseImageDirection;


};

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkVectorBSplineInterpolateImageFunction.txx"
#endif

#endif

//#endif
