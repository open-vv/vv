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
#ifndef __clitkShapedBLUTSpatioTemporalDeformableTransform_h
#define __clitkShapedBLUTSpatioTemporalDeformableTransform_h
#include "clitkVectorBSplineResampleImageFunctionWithLUT.h"
#include "clitkExtractImageFilter.h"
#include "clitkLinearCombinationImageFilter.h"

//itk include
#include "itkTransform.h"
#include "itkImage.h"
#include "itkImageRegion.h"
#include "itkSpatialObject.h"
#include "itkPasteImageFilter.h"

namespace clitk
{

  template <
    class TCoordRep = double,               // Data type for scalars, coordinate representation,vectors
    unsigned int NInputDimensions = 3,        // Number of input dimensions
    unsigned int NOutputDimensions = 3 >      // Number of output dimensions
  class ITK_EXPORT ShapedBLUTSpatioTemporalDeformableTransform : 
    public itk::Transform< TCoordRep, NInputDimensions, NOutputDimensions >
  {
  public:

    //====================================================================
    // Typedefs
    //====================================================================
    typedef ShapedBLUTSpatioTemporalDeformableTransform                         Self;
    typedef itk::Transform< TCoordRep, NInputDimensions, NOutputDimensions > Superclass;
    typedef itk::SmartPointer<Self>                                 Pointer;
    typedef itk::SmartPointer<const Self>                           ConstPointer;
      
    /** New macro for creation of through the object factory.*/
    itkNewMacro( Self );

    /** Run-time type information (and related methods). */
    itkTypeMacro( ShapedBLUTSpatioTemporalDeformableTransform, Transform );

    /** Dimension of the output space. */
    itkStaticConstMacro(OutputDimension, unsigned int, NOutputDimensions);

    /** Dimension of the input space. */
    itkStaticConstMacro(InputDimension, unsigned int, NInputDimensions);

    //JV the number of spatial dimensions
    itkStaticConstMacro(SpaceDimension, unsigned int, NInputDimensions-1);

    /** Standard scalar type for this class. */
    typedef typename Superclass::ScalarType ScalarType;

    /** Standard parameters container. */
    typedef typename Superclass::ParametersType ParametersType;

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
    typedef typename itk::Vector<ParametersValueType, SpaceDimension>   PixelType;
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
    typedef itk::Vector<TCoordRep, InputDimension> OutputSpacingType;

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

    // The padded coefficient image
    virtual CoefficientImagePointer GetPaddedCoefficientImage()
    { return m_PaddedCoefficientImage; }
    virtual const CoefficientImagePointer  GetPaddedCoefficientImage() const
    { return m_PaddedCoefficientImage; }
    // virtual void SetPaddedCoefficientImage(CoefficientImagePointer image);  

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

    //Set mask, inside transform applies, outside zero, real pointer
    void SetMask(MaskPointer m){m_Mask=m;}

    // JV the shape
    itkSetMacro( TransformShape , unsigned int  );
    itkGetMacro( TransformShape , unsigned int  );
    itkGetConstMacro( TransformShape, unsigned int  );

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
    virtual unsigned int GetNumberOfParameters(void) const;

    //JV Return the padded number of parameters
    virtual unsigned int GetPaddedNumberOfParameters(void) const;

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
    /** Print contents of an BSplineSpatioTemporalDeformableTransform. */
    void PrintSelf(std::ostream &os, itk::Indent indent) const;


    ShapedBLUTSpatioTemporalDeformableTransform();
    virtual ~ShapedBLUTSpatioTemporalDeformableTransform();

    /** Wrap flat array into images of coefficients. */
    void WrapAsImages();

    // JV Pad/Extract the coefficient image
    void PadCoefficientImage(void);
    typename  CoefficientImageType::Pointer ExtractTemporalRow(const typename CoefficientImageType::Pointer& coefficientImage, unsigned int temporalIndex);
    //void ExtractCoefficientImage(void);

    /** Wrap flat array into images of coefficients. */
    inline void WrapRegion(const RegionType& support, 
			   RegionType& first, 
			   RegionType& second, 
			   RegionType& third, 
			   std::vector<RegionType>& bc, 
			   std::vector<double>& bcValues,
			   std::vector<RegionType>& bc2, 
			   std::vector<double>& bc2Values,
 			   std::vector<RegionType>& bc3, 
			   std::vector<double>& bc3Values,
			   unsigned int& m_InitialOffset ) const;

    /** Convert an input point to a continuous index inside the BSpline grid */
    void TransformPointToContinuousIndex( const InputPointType & point, ContinuousIndexType & index ) const;

  private:
    ShapedBLUTSpatioTemporalDeformableTransform(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    /** The bulk transform. */
    BulkTransformPointer  m_BulkTransform;
    MaskPointer m_Mask;

    // JV added for BLUT interpolator
    SizeType m_SplineOrders;
    //OutputSpacingType m_OutputSpacing;
    SizeType m_LUTSamplingFactors;

    /** Variables defining the coefficient grid extend. */
    RegionType    m_GridRegion;
    SpacingType   m_GridSpacing;
    DirectionType m_GridDirection;
    OriginType    m_GridOrigin;

    // JV additional variables for the padded region
    RegionType    m_PaddedGridRegion;
    OriginType    m_PaddedGridOrigin;    
  

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
    CoefficientImagePointer   m_PaddedCoefficientImage;

    /** Jacobian as OutputDimension number of images. */
    typedef typename JacobianType::ValueType JacobianValueType;
    typedef typename itk::Vector<JacobianValueType,SpaceDimension> JacobianPixelType;
    typedef itk::Image<JacobianPixelType, OutputDimension> JacobianImageType;
    typename JacobianImageType::Pointer m_JacobianImage[OutputDimension];
    typename JacobianImageType::Pointer m_PaddedJacobianImage[OutputDimension];
    typedef itk::ImageRegionIterator<JacobianImageType> IteratorType;

    //JV for J calculation
    IndexType m_NullIndex;
    SizeType m_NullSize;

    mutable RegionType    m_SupportRegion;
    mutable IndexType    m_SupportIndex;
    mutable RegionType    m_FirstRegion;
    mutable RegionType    m_SecondRegion;
    mutable RegionType    m_ThirdRegion;
    mutable unsigned int m_ThirdSize;
    mutable unsigned int m_InitialOffset;

    mutable std::vector<IteratorType>  m_BCIterators[SpaceDimension];
    mutable std::vector<double>   m_BCValues;
    mutable std::vector<RegionType>  m_BCRegions;
    mutable unsigned int m_BCSize;

    mutable std::vector<IteratorType>  m_BC2Iterators[SpaceDimension];
    mutable std::vector<double>   m_BC2Values;
    mutable std::vector<RegionType>  m_BC2Regions;
    mutable unsigned int m_BC2Size;

    mutable std::vector<IteratorType>  m_BC3Iterators[SpaceDimension];
    mutable std::vector<double>   m_BC3Values;
    mutable std::vector<RegionType>  m_BC3Regions;
    mutable unsigned int m_BC3Size;

    mutable RegionType m_BCRegion;
    mutable IteratorType m_FirstIterator[SpaceDimension];
    mutable IteratorType m_SecondIterator[SpaceDimension];
    mutable IteratorType m_ThirdIterator[SpaceDimension];
    mutable ContinuousIndexType m_Index;

    //JV add a padded Jacobian matrix
    mutable JacobianType m_PaddedJacobian;
    mutable JacobianPixelType m_ZeroVector;

    /** Keep track of last support region used in computing the Jacobian
     * for fast resetting of Jacobian to zero.
     */
    mutable IndexType m_LastJacobianIndex;
   
    /** Keep a pointer to the input parameters. */
    const ParametersType *  m_InputParametersPointer;

    /** Internal parameters buffer. */
    ParametersType          m_InternalParametersBuffer;

    //JV  the BLUT interpolator
    typename VectorInterpolatorType::Pointer m_VectorInterpolator;

    // the coefficients to apply the BC
    std::vector<std::vector<double> > m_Weights;
    std::vector<std::vector<double> > m_WeightRatio;
  
    /** Check if a continuous index is inside the valid region. */
    bool InsideValidRegion( const ContinuousIndexType& index ) const;

    // JV Shape
    unsigned int m_TransformShape;

#if ITK_VERSION_MAJOR >= 4
    mutable JacobianType                            m_SharedDataBSplineJacobian;
#endif

  }; //class ShapedBLUTSpatioTemporalDeformableTransform


}  // namespace itk

#if ITK_TEMPLATE_TXX
# include "clitkShapedBLUTSpatioTemporalDeformableTransform.txx"
#endif


#endif // __clitkShapedBLUTSpatioTemporalDeformableTransform_h 
