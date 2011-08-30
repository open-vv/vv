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
#ifndef __clitkMultipleBSplineDeformableTransform_h
#define __clitkMultipleBSplineDeformableTransform_h

#include <clitkBSplineDeformableTransform.h>

#include <itkNearestNeighborInterpolateImageFunction.h>

#include <vector>

namespace clitk
{
  template <
    class TCoordRep = double,
    unsigned int NInputDimensions = 3,
    unsigned int NOutputDimensions = 3 >
  class ITK_EXPORT MultipleBSplineDeformableTransform : public itk::Transform< TCoordRep, NInputDimensions, NOutputDimensions >
  {
  public:

    //====================================================================
    // Typedefs
    //====================================================================
    typedef MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>  Self;
    typedef itk::Transform<TCoordRep, NInputDimensions, NOutputDimensions>              Superclass;
    typedef itk::SmartPointer<Self>                                                     Pointer;
    typedef itk::SmartPointer<const Self>                                               ConstPointer;

    /** New macro for creation of through the object factory.*/
    itkNewMacro( Self );

    /** Run-time type information (and related methods). */
    itkTypeMacro( MultipleBSplineDeformableTransform, Transform );

    /** Dimension of the domain space. */
    itkStaticConstMacro(OutputDimension, unsigned int, NOutputDimensions);

    /** Dimension of the input model. */
    itkStaticConstMacro(InputDimension, unsigned int, NInputDimensions);

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

    /** Typedef of the bulk transform. */
    typedef itk::Transform<ScalarType, InputDimension, OutputDimension> BulkTransformType;
    typedef BulkTransformType*                  BulkTransformPointer;

    typedef itk::Image< unsigned char, InputDimension>   ImageLabelType;
    typedef ImageLabelType* ImageLabelPointer;
    //typedef typename ImageLabelType::Pointer ImageLabelPointer;

    typedef itk::NearestNeighborInterpolateImageFunction<ImageLabelType, TCoordRep> ImageLabelInterpolator;
    typedef typename ImageLabelInterpolator::Pointer ImageLabelInterpolatorPointer;

    void SetLabels(ImageLabelPointer labels);
    ImageLabelPointer GetLabels() {return m_labels;}

    itkGetMacro(nLabels, unsigned);

    void SetSplineOrder(const unsigned int &  splineOrder);
    void SetSplineOrders(const SizeType &  splineOrders);
    /*
    itkGetMacro( SplineOrders, SizeType );
    itkGetConstMacro( SplineOrders, SizeType );
    */

    void SetLUTSamplingFactor(const int &  samplingFactor);
    void SetLUTSamplingFactors(const SizeType &  samplingFactors);
    /*
    itkGetMacro( LUTSamplingFactors, SizeType );
    itkGetConstMacro( LUTSamplingFactors,SizeType );
    */

    void SetParameters(const ParametersType & parameters);

    void SetFixedParameters(const ParametersType & parameters);

    void SetParametersByValue(const ParametersType & parameters);

    void SetIdentity();

    /** Get the Transformation Parameters. */
    virtual const ParametersType& GetParameters(void) const;

    /** Get the Transformation Fixed Parameters. */
    virtual const ParametersType& GetFixedParameters(void) const;

    // The coefficientImage
    virtual const std::vector<CoefficientImagePointer>& GetCoefficientImages() const;
    virtual void SetCoefficientImages(std::vector<CoefficientImagePointer>& images);

    /** This method specifies the region over which the grid resides. */
    virtual void SetGridRegion( const RegionType& region );
    /*
    itkGetMacro( GridRegion, RegionType );
    itkGetConstMacro( GridRegion, RegionType );
    */

    /** This method specifies the grid spacing or resolution. */
    virtual void SetGridSpacing( const SpacingType& spacing );
    /*
    itkGetMacro( GridSpacing, SpacingType );
    itkGetConstMacro( GridSpacing, SpacingType );
    */

    /** This method specifies the grid directions . */
    virtual void SetGridDirection( const DirectionType & spacing );
    /*
    itkGetMacro( GridDirection, DirectionType );
    itkGetConstMacro( GridDirection, DirectionType );
    */

    /** This method specifies the grid origin. */
    virtual void SetGridOrigin( const OriginType& origin );
    /*
    itkGetMacro( GridOrigin, OriginType );
    itkGetConstMacro( GridOrigin, OriginType );
    */

    // Set the bulk transform, real pointer
    void SetBulkTransform(BulkTransformPointer b);
    BulkTransformPointer GetBulkTransform(void) {return m_BulkTransform;}

    /** Transform points by a BSpline deformable transformation. */
    OutputPointType  TransformPoint(const InputPointType  &point ) const;

    // JV added for just the deformable part, without bulk
    OutputPointType  DeformablyTransformPoint(const InputPointType  &point ) const;

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
    virtual OutputCovariantVectorType TransformCovariantVector(const InputCovariantVectorType &) const
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

    /** Return the number of parameters per dimension */
    unsigned int GetNumberOfParametersPerDimension(void) const;

    virtual bool IsLinear() const { return false; }

    typedef  clitk::BSplineDeformableTransform<TCoordRep,InputDimension, OutputDimension > TransformType;

  protected:

    void PrintSelf(std::ostream &os, itk::Indent indent) const;

    MultipleBSplineDeformableTransform();
    virtual ~MultipleBSplineDeformableTransform();

    void TransformPointToContinuousIndex( const InputPointType & point, ContinuousIndexType & index ) const;

  private:
    MultipleBSplineDeformableTransform(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    /** The bulk transform. */
    BulkTransformPointer  m_BulkTransform;

    /** Jacobian as OutputDimension number of images. */
    typedef typename JacobianType::ValueType JacobianValueType;
    typedef typename itk::Vector<JacobianValueType,OutputDimension> JacobianPixelType;
    typedef itk::Image<JacobianPixelType, OutputDimension> JacobianImageType;
    typename JacobianImageType::Pointer m_JacobianImage[OutputDimension];
    typedef itk::ImageRegionIterator<JacobianImageType> IteratorType;

    /** Keep a pointer to the input parameters. */
    const ParametersType *  m_InputParametersPointer;

    /** Internal parameters buffer. */
    ParametersType          m_InternalParametersBuffer;

    unsigned int                                    m_nLabels;
    ImageLabelPointer                               m_labels;
    ImageLabelInterpolatorPointer                   m_labelInterpolator;
    std::vector<typename TransformType::Pointer>    m_trans;
    std::vector<ParametersType>                     m_parameters;
    mutable std::vector<CoefficientImagePointer>    m_CoefficientImages;
    mutable int                                     m_LastJacobian;
#if ITK_VERSION_MAJOR >= 4
    mutable JacobianType                            m_SharedDataBSplineJacobian;
#endif

    void InitJacobian();
    // FIXME it seems not used
    bool InsideValidRegion( const ContinuousIndexType& index ) const;
  };

}  // namespace clitk

#if ITK_TEMPLATE_TXX
# include "clitkMultipleBSplineDeformableTransform.txx"
#endif

#endif // __clitkMultipleBSplineDeformableTransform_h
