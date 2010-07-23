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
#ifndef __clitkRayCastInterpolateImageFunctionWithOrigin_h
#define __clitkRayCastInterpolateImageFunctionWithOrigin_h
#include "clitkBSplineDeformableTransform.h"

#include "itkInterpolateImageFunction.h"
#include "itkTransform.h"
#include "itkVector.h"

namespace clitk
{


template <class TInputImage, class TCoordRep = float>
class ITK_EXPORT RayCastInterpolateImageFunctionWithOrigin : 
    public itk::InterpolateImageFunction<TInputImage,TCoordRep> 
{
public:
  /** Standard class typedefs. */
  typedef RayCastInterpolateImageFunctionWithOrigin                 Self;
  typedef itk::InterpolateImageFunction<TInputImage,TCoordRep> Superclass;
  typedef itk::SmartPointer<Self>                              Pointer;
  typedef itk::SmartPointer<const Self>                        ConstPointer;

  /** Constants for the image dimensions */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);

  /** 
   * Type of the Transform Base class 
   * The fixed image should be a 3D image
   */
  typedef itk::Transform<TCoordRep,3,3> TransformType;

  typedef typename TransformType::Pointer            TransformPointer;
  typedef typename TransformType::InputPointType     InputPointType;
  typedef typename TransformType::OutputPointType    OutputPointType;
  typedef typename TransformType::ParametersType     TransformParametersType;
  typedef typename TransformType::JacobianType       TransformJacobianType;

  typedef typename Superclass::InputPixelType        PixelType;

  typedef typename TInputImage::SizeType             SizeType;

  typedef itk::Vector<TCoordRep, 3>                       DirectionType;

  /**  Type of the Interpolator Base class */
  typedef itk::InterpolateImageFunction<TInputImage,TCoordRep> InterpolatorType;

  typedef typename InterpolatorType::Pointer         InterpolatorPointer;

  //JV Type of the Interpolator for the transform  
  typedef itk::InterpolateImageFunction<TInputImage,TCoordRep> DeformableTransformInterpolatorType;
  typedef typename DeformableTransformInterpolatorType::Pointer        DeformableTransformInterpolatorPointer;


  /** Run-time type information (and related methods). */
  itkTypeMacro(RayCastInterpolateImageFunctionWithOrigin, InterpolateImageFunction);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** OutputType typedef support. */
  typedef typename Superclass::OutputType OutputType;

  /** InputImageType typedef support. */
  typedef typename Superclass::InputImageType InputImageType;

  /** RealType typedef support. */
  typedef typename Superclass::RealType RealType;

  /** Dimension underlying input image. */
  itkStaticConstMacro(ImageDimension, unsigned int,Superclass::ImageDimension);

  /** Point typedef support. */
  typedef typename Superclass::PointType PointType;

  /** Index typedef support. */
  typedef typename Superclass::IndexType IndexType;

  /** ContinuousIndex typedef support. */
  typedef typename Superclass::ContinuousIndexType ContinuousIndexType;

  /** \brief
   * Interpolate the image at a point position.
   *
   * Returns the interpolated image intensity at a 
   * specified point position. No bounds checking is done.
   * The point is assume to lie within the image buffer.
   *
   * ImageFunction::IsInsideBuffer() can be used to check bounds before
   * calling the method. 
   */
  virtual OutputType Evaluate( const PointType& point ) const;

  /** Interpolate the image at a continuous index position
   *
   * Returns the interpolated image intensity at a 
   * specified index position. No bounds checking is done.
   * The point is assume to lie within the image buffer.
   *
   * Subclasses must override this method.
   *
   * ImageFunction::IsInsideBuffer() can be used to check bounds before
   * calling the method. 
   */
  virtual OutputType EvaluateAtContinuousIndex( 
    const ContinuousIndexType &index ) const;


  /** Connect the Transform. */
  itkSetObjectMacro( Transform, TransformType );
  /** Get a pointer to the Transform.  */
  itkGetObjectMacro( Transform, TransformType );
 
  /** Connect the Interpolator. */
  itkSetObjectMacro( Interpolator, InterpolatorType );
  /** Get a pointer to the Interpolator.  */
  itkGetObjectMacro( Interpolator, InterpolatorType );

  /** Connect the focalPoint. */
  itkSetMacro( FocalPoint, InputPointType );
  /** Get a pointer to the focalpoint.  */
  itkGetMacro( FocalPoint, InputPointType );

  /** Connect the threshold. */
  itkSetMacro( Threshold, double );
  /** Get a pointer to the threshhold.  */
  itkGetMacro( Threshold, double );
 
  /** Check if a point is inside the image buffer.
   * \warning For efficiency, no validity checking of
   * the input image pointer is done. */
  inline bool IsInsideBuffer( const PointType & ) const
    { 
    return true;
    }
  bool IsInsideBuffer( const ContinuousIndexType &  ) const
    {
    return true;
    }
  bool IsInsideBuffer( const IndexType &  ) const
    { 
    return true;
    }

  //JV transform is deformable
  itkSetMacro( TransformIsDeformable, bool );
  itkGetMacro( TransformIsDeformable, bool );
  
  //JV interpolator for deformable transform
  itkSetObjectMacro( DeformableTransformInterpolator, DeformableTransformInterpolatorType );
  itkGetObjectMacro( DeformableTransformInterpolator, DeformableTransformInterpolatorType );

protected:

  /// Constructor
  RayCastInterpolateImageFunctionWithOrigin();

  /// Destructor
  ~RayCastInterpolateImageFunctionWithOrigin(){};

  /// Print the object
  void PrintSelf(std::ostream& os, itk::Indent indent) const;
  
  /// Transformation used to calculate the new focal point position
  TransformPointer m_Transform;

  /// The focal point or position of the ray source
  InputPointType m_FocalPoint;

  /// The threshold above which voxels along the ray path are integrated.
  double m_Threshold;

  /// Pointer to the interpolator
  InterpolatorPointer m_Interpolator;

  //JV Pointer to the deformable transform interpolator
  DeformableTransformInterpolatorPointer m_DeformableTransformInterpolator;


  //JV Transform is deformable: fetch interpolated value at transformed intersect
  bool m_TransformIsDeformable;
  

private:
  RayCastInterpolateImageFunctionWithOrigin( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented


};

} // namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkRayCastInterpolateImageFunctionWithOrigin.txx"
#endif

#endif
