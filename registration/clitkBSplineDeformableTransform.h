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
#ifndef __clitkBSplineDeformableTransform_h
#define __clitkBSplineDeformableTransform_h
#include "clitkVectorBSplineResampleImageFunctionWithLUT.h"

//itk include
#include "itkTransform.h"
#include "itkImage.h"
#include "itkImageRegion.h"
#include "itkSpatialObject.h"

namespace clitk
{
  // Forward declaration needed for friendship
  template <class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  class ITK_EXPORT MultipleBSplineDeformableTransform;

  template <
    class TCoordRep = double,               // Data type for scalars, coordinate representation,vectors
    unsigned int NInputDimensions = 3,        // Number of input dimensions
    unsigned int NOutputDimensions = 3 >      // Number of output dimensions
  class ITK_EXPORT BSplineDeformableTransform : 
    public itk::Transform< TCoordRep, NInputDimensions, NOutputDimensions >
  {
  public:

    //====================================================================
    // Typedefs
    //====================================================================
    typedef BSplineDeformableTransform                         Self;
    typedef itk::Transform< TCoordRep, NInputDimensions, NOutputDimensions > Superclass;
    typedef itk::SmartPointer<Self>                                 Pointer;
    typedef itk::SmartPointer<const Self>                           ConstPointer;
      
    /** New macro for creation of through the object factory.*/
    itkNewMacro( Self );

    /** Run-time type information (and related methods). */
    itkTypeMacro( BSplineDeformableTransform, Transform );

    /** Dimension of the domain space. */
    itkStaticConstMacro(OutputDimension, unsigned int, NOutputDimensions);

    /** Dimension of the input model. */
    itkStaticConstMacro(InputDimension, unsigned int, NInputDimensions);

    /** Standard scalar type for this class. */
    typedef typename Superclass::ScalarType ScalarType;

    /** Standard parameters container. */
    typedef typename Superclass::ParametersType ParametersType;
#if ITK_VERSION_MAJOR >= 4
    typedef typename Superclass::NumberOfParametersType NumberOfParametersType;
#endif

    /** Standard Jacobian container. */
    typedef typename Superclass::JacobianType JacobianType;

    /** Standard vector type for this class. */
    typedef itk::Vector<TCoordRep,
			InputDimension> InputVectorType;
    typedef itk::Vector<TCoordRep,
			OutputDimension> OutputVectorType;
  
    /** Standard covariant vector type for this class. */
    typedef itk::CovariantVector<TCoordRep,
				 InputDimension> InputCovariantVectorType;
    typedef itk::CovariantVector<TCoordRep,
				 OutputDimension> OutputCovariantVectorType;

    /** Standard vnl_vector type for this class. */
    typedef vnl_vector_fixed<TCoordRep,
			     InputDimension> InputVnlVectorType;
    typedef vnl_vector_fixed<TCoordRep,
			     OutputDimension> OutputVnlVectorType;

    /** Standard coordinate point type for this class. */
    typedef itk::Point<TCoordRep,
		       InputDimension> InputPointType;
    typedef itk::Point<TCoordRep,
		       OutputDimension> OutputPointType;

    //JV Parameters as images with OutputDimension number of components per Pixel
    typedef typename ParametersType::ValueType                          ParametersValueType;
    typedef typename itk::Vector<ParametersValueType, OutputDimension>  PixelType;
    typedef itk::Image<PixelType, InputDimension>                       CoefficientImageType;
    typedef typename CoefficientImageType::Pointer                      CoefficientImagePointer;

  
    /** Typedefs for specifying the extend to the grid. */
    typedef itk::ImageRegion<InputDimension>                RegionType;
    typedef typename RegionType::IndexType                  IndexType;
    typedef typename RegionType::SizeType                   SizeType;
    typedef typename CoefficientImageType::SpacingType      SpacingType;
    typedef typename CoefficientImageType::DirectionType    DirectionType;
    typedef typename CoefficientImageType::PointType        OriginType;
    typedef itk::ContinuousIndex<TCoordRep, InputDimension> ContinuousIndexType;
  
    //JV added for the BLUT interpolator
    //typedef itk::Vector<TCoordRep, InputDimension> OutputSpacingType;

    //JV m_VectorInterpolator
    typedef VectorBSplineResampleImageFunctionWithLUT
    <CoefficientImageType, TCoordRep> VectorInterpolatorType;
    typedef  typename  VectorInterpolatorType::CoefficientDataType CoefficientDataType;
    typedef  typename  VectorInterpolatorType::CoefficientDataType WeightsDataType;

    /** Typedef of the bulk transform. */
    typedef itk::Transform<ScalarType, InputDimension, OutputDimension> BulkTransformType;
    typedef BulkTransformType*                  BulkTransformPointer;

    /** Typedef of the mask */
    typedef itk::SpatialObject<  InputDimension >   MaskType;
    typedef MaskType*    MaskPointer;

    //====================================================================
    // Set et Gets
    //====================================================================
    //JV  added for the BLUT interpolator
    void SetSplineOrder(const unsigned int &  splineOrder);
    void SetSplineOrders(const SizeType &  splineOrders);
    itkGetMacro( SplineOrders, SizeType );
    itkGetConstMacro( SplineOrders, SizeType );
    void SetLUTSamplingFactor(const int &  samplingFactor);
    void SetLUTSamplingFactors(const SizeType &  samplingFactors);
    itkGetMacro( LUTSamplingFactors, SizeType );
    itkGetConstMacro( LUTSamplingFactors,SizeType );
    //void SetOutputSpacing(const OutputSpacingType & outputSpacing);
    //itkGetMacro( OutputSpacing, OutputSpacingType );
    //itkGetConstMacro( OutputSpacing, OutputSpacingType );

    void SetParameters(const ParametersType & parameters);
  
    void SetFixedParameters(const ParametersType & parameters);

    void SetParametersByValue(const ParametersType & parameters);

    void SetIdentity();

    /** Get the Transformation Parameters. */
    virtual const ParametersType& GetParameters(void) const;

    /** Get the Transformation Fixed Parameters. */
    virtual const ParametersType& GetFixedParameters(void) const;
  
    // The coefficientImage
    virtual CoefficientImagePointer GetCoefficientImage()
    { return m_CoefficientImage; }
    virtual const CoefficientImagePointer  GetCoefficientImage() const
    { return m_CoefficientImage; }
    virtual void SetCoefficientImage(CoefficientImagePointer image);  

    /** This method specifies the region over which the grid resides. */
    virtual void SetGridRegion( const RegionType& region );
    itkGetMacro( GridRegion, RegionType );
    itkGetConstMacro( GridRegion, RegionType );

    /** This method specifies the grid spacing or resolution. */
    virtual void SetGridSpacing( const SpacingType& spacing );
    itkGetMacro( GridSpacing, SpacingType );
    itkGetConstMacro( GridSpacing, SpacingType );

    /** This method specifies the grid directions . */
    virtual void SetGridDirection( const DirectionType & spacing );
    itkGetMacro( GridDirection, DirectionType );
    itkGetConstMacro( GridDirection, DirectionType );

    /** This method specifies the grid origin. */
    virtual void SetGridOrigin( const OriginType& origin );
    itkGetMacro( GridOrigin, OriginType );
    itkGetConstMacro( GridOrigin, OriginType );
    
    // Set the bulk transform, real pointer
    // itkSetConstObjectMacro( BulkTransform, BulkTransformType );
    // itkGetConstObjectMacro( BulkTransform, BulkTransformType );
    void SetBulkTransform(BulkTransformPointer b){m_BulkTransform=b;}
    BulkTransformPointer GetBulkTransform(void) {return m_BulkTransform;}

    // Set mask, inside transform applies, outside zero, real pointer
    void SetMask(MaskPointer m){m_Mask=m;}
    MaskPointer GetMask(void){return m_Mask;}
    // itkSetConstObjectMacro( Mask, MaskType );
    // itkGetConstObjectMacro( Mask, MaskType );

    /** Transform points by a BSpline deformable transformation. */
    OutputPointType  TransformPoint(const InputPointType  &point ) const;
  
    // JV added for just the deformable part, without bulk
    OutputPointType  DeformablyTransformPoint(const InputPointType  &point ) const;
  
    /** Parameter index array type. */
    typedef itk::Array<unsigned long> ParameterIndexArrayType;

    /** Transform points by a BSpline deformable transformation. 
     * On return, weights contains the interpolation weights used to compute the 
     * deformation and indices of the x (zeroth) dimension coefficient parameters
     * in the support region used to compute the deformation.
     * Parameter indices for the i-th dimension can be obtained by adding
     * ( i * this->GetNumberOfParametersPerDimension() ) to the indices array.
     */

    // JV not implemented
    //  virtual void TransformPoint( const InputPointType & inputPoint,
    // 				 OutputPointType & outputPoint,
    // 				 WeightsType & weights,
    // 				 ParameterIndexArrayType & indices, 
    // 				 bool & inside ) const;
    //     virtual void DeformablyTransformPoint( const InputPointType & inputPoint,
    // 					   OutputPointType & outputPoint,
    // 					   WeightsType & weights,
    // 					   ParameterIndexArrayType & indices, 
    // 					   bool & inside ) const;
    //     virtual void GetJacobian( const InputPointType & inputPoint,
    //     			      WeightsType & weights,
    //     			      ParameterIndexArrayType & indices
    //     			      ) const;
   
    /** Method to transform a vector - 
     *  not applicable for this type of transform. */
    virtual OutputVectorType TransformVector(const InputVectorType &) const
    { 
      itkExceptionMacro(<< "Method not applicable for deformable transform." );
      return OutputVectorType(); 
    }

    /** Method to transform a vnl_vector - 
     *  not applicable for this type of transform */
    virtual OutputVnlVectorType TransformVector(const InputVnlVectorType &) const
    { 
      itkExceptionMacro(<< "Method not applicable for deformable transform. ");
      return OutputVnlVectorType(); 
    }

    /** Method to transform a CovariantVector - 
     *  not applicable for this type of transform */
    virtual OutputCovariantVectorType TransformCovariantVector(
							       const InputCovariantVectorType &) const
    { 
      itkExceptionMacro(<< "Method not applicable for deformable transfrom. ");
      return OutputCovariantVectorType(); 
    } 
    
    /** Compute the Jacobian Matrix of the transformation at one point */
#if ITK_VERSION_MAJOR >= 4
    virtual void ComputeJacobianWithRespectToParameters (const InputPointType &p, JacobianType &jacobian) const;
    virtual void ComputeJacobianWithRespectToPosition (const InputPointType &p, JacobianType &jacobian) const
    {
      itkExceptionMacro( "ComputeJacobianWithRespectToPosition not yet implemented for " << this->GetNameOfClass() );
    }
#else
    virtual const JacobianType& GetJacobian(const InputPointType  &point ) const;
#endif

    /** Return the number of parameters that completely define the Transfom */
#if ITK_VERSION_MAJOR >= 4
    virtual NumberOfParametersType GetNumberOfParameters(void) const;
#else
    virtual unsigned int GetNumberOfParameters(void) const;
#endif

    /** Return the number of parameters per dimension */
    unsigned int GetNumberOfParametersPerDimension(void) const;

    /** Return the region of the grid wholly within the support region */
    itkGetConstReferenceMacro( ValidRegion, RegionType );

    /** Indicates that this transform is linear. That is, given two
     * points P and Q, and scalar coefficients a and b, then
     *
     *           T( a*P + b*Q ) = a * T(P) + b * T(Q)
     */
    virtual bool IsLinear() const { return false; }

   //unsigned int GetNumberOfAffectedWeights() const;

  protected:
    /** Print contents of an BSplineDeformableTransform. */
    void PrintSelf(std::ostream &os, itk::Indent indent) const;


    BSplineDeformableTransform();
    virtual ~BSplineDeformableTransform();

    /** Wrap flat array into images of coefficients. */
    void WrapAsImages();

    /** Convert an input point to a continuous index inside the BSpline grid */
    void TransformPointToContinuousIndex( const InputPointType & point, ContinuousIndexType & index ) const;

  private:
    BSplineDeformableTransform(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    /** The bulk transform. */
    BulkTransformPointer  m_BulkTransform;
    MaskPointer m_Mask;

    // JV added for BLUT interpolator
    SizeType m_SplineOrders;
    SizeType m_LUTSamplingFactors;

    /** Variables defining the coefficient grid extend. */
    RegionType    m_GridRegion;
    SpacingType   m_GridSpacing;
    DirectionType m_GridDirection;
    OriginType    m_GridOrigin;

    DirectionType m_PointToIndex;
    DirectionType m_IndexToPoint;
  
    RegionType    m_ValidRegion;

    /** Variables defining the interpolation support region. */
    SizeType m_Offset;
    itk::FixedArray<bool,InputDimension>    m_SplineOrderOdd;
    SizeType      m_SupportSize;
    IndexType     m_ValidRegionLast;
  
    /** Array holding images wrapped from the flat parameters. */
    CoefficientImagePointer   m_WrappedImage;

    /** Vector image representing the B-spline coefficients 
     *  in each dimension. */
    CoefficientImagePointer   m_CoefficientImage;

    /** Jacobian as OutputDimension number of images. */
    typedef typename JacobianType::ValueType JacobianValueType;
    typedef typename itk::Vector<JacobianValueType,OutputDimension> JacobianPixelType;
    typedef itk::Image<JacobianPixelType, OutputDimension> JacobianImageType;
    typename JacobianImageType::Pointer m_JacobianImage[OutputDimension];
    typedef itk::ImageRegionIterator<JacobianImageType> IteratorType;

    /** Keep track of last support region used in computing the Jacobian
     * for fast resetting of Jacobian to zero.
     */
    //JV for J calculation
    mutable RegionType    m_SupportRegion;
    mutable IndexType     m_SupportIndex;
    mutable IndexType m_LastJacobianIndex;
    mutable IteratorType m_Iterator[OutputDimension];
    mutable JacobianPixelType m_ZeroVector;
    mutable ContinuousIndexType m_Index;
    mutable bool m_NeedResetJacobian;

    /** Keep a pointer to the input parameters. */
    const ParametersType *  m_InputParametersPointer;

    /** Internal parameters buffer. */
    ParametersType          m_InternalParametersBuffer;

    //JV  the BLUT interpolator
    typename VectorInterpolatorType::Pointer m_VectorInterpolator;
  
    /** Check if a continuous index is inside the valid region. */
    bool InsideValidRegion( const ContinuousIndexType& index ) const;

    // VD Use external data container for JacobianImage
    unsigned SetJacobianImageData(JacobianPixelType * jacobianDataPointer, unsigned dim);

    // VD Reset Jacobian
    void ResetJacobian() const;

    // VD Add MultipleBSplineDeformableTransform as friend to facilitate wrapping
    friend class MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>;
#if ITK_VERSION_MAJOR >= 4
    mutable JacobianType                            m_SharedDataBSplineJacobian;
#endif

  }; //class BSplineDeformableTransform


}  // namespace itk

#if ITK_TEMPLATE_TXX
# include "clitkBSplineDeformableTransform.txx"
#endif


#endif // __clitkBSplineDeformableTransform_h 
