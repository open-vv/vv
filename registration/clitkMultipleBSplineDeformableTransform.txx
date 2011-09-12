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
#ifndef __clitkMultipleBSplineDeformableTransform_txx
#define __clitkMultipleBSplineDeformableTransform_txx

// clitk
#include "clitkMultipleBSplineDeformableTransformInitializer.h"

// itk
#include <itkRelabelComponentImageFilter.h>

namespace clitk
{
  // Constructor with default arguments
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
#if ITK_VERSION_MAJOR >= 4
  ::MultipleBSplineDeformableTransform() : Superclass(0)
#else
  ::MultipleBSplineDeformableTransform() : Superclass(OutputDimension, 0)
#endif
  {
    m_nLabels = 1;
    m_labels = 0;
    m_labelInterpolator = 0;
    m_trans.resize(1);
    m_trans[0] = TransformType::New();
    m_parameters.resize(1);

    this->m_FixedParameters.SetSize(NInputDimensions * (NInputDimensions + 5) + 4);
    this->m_FixedParameters.Fill(0.0);

    m_InternalParametersBuffer = ParametersType(0);
    // Make sure the parameters pointer is not NULL after construction.
    m_InputParametersPointer = &m_InternalParametersBuffer;
    m_BulkTransform = 0;
    m_LastJacobian = -1;
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::~MultipleBSplineDeformableTransform()
  {
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetLabels(ImageLabelPointer labels)
  {
    GetFixedParameters(); // Update m_FixedParameters
    if (labels)
    {
      typedef itk::RelabelComponentImageFilter<ImageLabelType, ImageLabelType> RelabelImageFilterType;
      typename RelabelImageFilterType::Pointer relabelImageFilter = RelabelImageFilterType::New();
      relabelImageFilter->SetInput(labels);
      relabelImageFilter->Update();
      m_labels = relabelImageFilter->GetOutput();
      m_nLabels = relabelImageFilter->GetNumberOfObjects();
      m_trans.resize(m_nLabels);
      m_parameters.resize(m_nLabels);
      for (unsigned i = 0; i < m_nLabels; ++i)
        m_trans[i] = TransformType::New();
      m_labelInterpolator = ImageLabelInterpolator::New();
      m_labelInterpolator->SetInputImage(m_labels);
    }
    else
    {
      m_nLabels = 1;
      m_trans.resize(1);
      m_parameters.resize(1);
      m_trans[0] = TransformType::New();
    }
    this->m_FixedParameters[(5+NInputDimensions)*NInputDimensions + 2] = (double)((size_t) m_labels);
    this->m_FixedParameters[(5+NInputDimensions)*NInputDimensions + 3] = m_nLabels;
    SetFixedParameters(this->m_FixedParameters);
  }

#define LOOP_ON_LABELS(FUNC, ARGS)          \
  for (unsigned i = 0; i < m_nLabels; ++i)  \
    m_trans[i]->FUNC(ARGS);

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetSplineOrder(const unsigned int & splineOrder)
  {
    LOOP_ON_LABELS(SetSplineOrder, splineOrder);
    this->Modified();
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetSplineOrders(const SizeType & splineOrders)
  {
    LOOP_ON_LABELS(SetSplineOrders, splineOrders);
    this->Modified();
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetLUTSamplingFactor( const int & samplingFactor)
  {
    LOOP_ON_LABELS(SetLUTSamplingFactor, samplingFactor);
    this->Modified();
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetLUTSamplingFactors( const SizeType & samplingFactors)
  {
    LOOP_ON_LABELS(SetLUTSamplingFactors, samplingFactors);
    this->Modified();
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetGridRegion( const RegionType & region )
  {
    LOOP_ON_LABELS(SetGridRegion, region);
    this->Modified();
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetGridSpacing( const SpacingType & spacing )
  {
    LOOP_ON_LABELS(SetGridSpacing, spacing);
    this->Modified();
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetGridDirection( const DirectionType & direction )
  {
    LOOP_ON_LABELS(SetGridDirection, direction);
    this->Modified();
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetGridOrigin( const OriginType& origin )
  {
    LOOP_ON_LABELS(SetGridOrigin, origin);
    this->Modified();
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetIdentity()
  {
    LOOP_ON_LABELS(SetIdentity, );
    if ( m_InputParametersPointer )
    {
      ParametersType * parameters =
        const_cast<ParametersType *>( m_InputParametersPointer );
      parameters->Fill( 0.0 );
    }
    this->Modified();
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline const std::vector<typename MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>::CoefficientImagePointer>&
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::GetCoefficientImages() const
  {
    m_CoefficientImages.resize(m_nLabels);
    for (unsigned i = 0; i < m_nLabels; ++i)
      m_CoefficientImages[i] = m_trans[i]->GetCoefficientImage();
    return m_CoefficientImages;
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetCoefficientImages(std::vector<CoefficientImagePointer>& images)
  {
    if (images.size() != m_nLabels)
    {
      itkExceptionMacro( << "Mismatched between the number of labels "
          << m_nLabels
          << " and the number of coefficient images "
          << images.size());
    }
    for (unsigned i = 0; i < m_nLabels; ++i)
      m_trans[i]->SetCoefficientImage(images[i]);
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetParameters( const ParametersType & parameters )
  {
    if ( parameters.Size() != this->GetNumberOfParameters() )
      {
	itkExceptionMacro(<<"Mismatched between parameters size "
			  << parameters.size()
			  << " and region size "
			  << this->GetNumberOfParameters() );
      }

    // Clean up buffered parameters
    m_InternalParametersBuffer = ParametersType( 0 );

    // Keep a reference to the input parameters
    m_InputParametersPointer = &parameters;

    double * dataPointer = const_cast<double *>(m_InputParametersPointer->data_block());
    unsigned tot = 0;
    for (unsigned i = 0; i < m_nLabels; ++i)
    {
      unsigned int numberOfParameters = m_trans[i]->GetNumberOfParameters();
      m_parameters[i].SetData(dataPointer + tot, numberOfParameters);
      m_trans[i]->SetParameters(m_parameters[i]);
      tot += numberOfParameters;
    }
    InitJacobian();

    this->Modified();
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetFixedParameters( const ParametersType & parameters )
  {
    // BSplineSeformableTransform parameters + m_labels pointer
    if ( parameters.Size() != NInputDimensions * (5 + NInputDimensions) + 4)
      {
	itkExceptionMacro(<< "Mismatched between parameters size "
			  << parameters.size()
			  << " and number of fixed parameters "
			  << NInputDimensions * (5 + NInputDimensions) + 4);
      }
    ImageLabelPointer tmp2 = ((ImageLabelPointer)( (size_t)parameters[(5+NInputDimensions)*NInputDimensions+2]));
    if (tmp2 != m_labels)
    {
      if (tmp2)
      {
        m_labels = tmp2;
        m_nLabels = parameters[(5+NInputDimensions) * NInputDimensions + 3 ];
        m_trans.resize(m_nLabels);
        m_parameters.resize(m_nLabels);
        for (unsigned i = 0; i < m_nLabels; ++i)
          m_trans[i] = TransformType::New();
        m_labelInterpolator = ImageLabelInterpolator::New();
        m_labelInterpolator->SetInputImage(m_labels);
      }
      else
      {
        m_labels = tmp2;
        m_nLabels = 1;
        m_trans.resize(1);
        m_parameters.resize(1);
        m_trans[0] = TransformType::New();
      }
    }
    unsigned int numberOfParameters = NInputDimensions * (5 + NInputDimensions) + 2;
    ParametersType tmp(numberOfParameters);
    for (unsigned j = 0; j < numberOfParameters; ++j)
      tmp.SetElement(j, parameters.GetElement(j));
    LOOP_ON_LABELS(SetFixedParameters, tmp);
    this->m_FixedParameters = parameters;
    this->Modified();
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetParametersByValue( const ParametersType & parameters )
  {
    if ( parameters.Size() != this->GetNumberOfParameters() )
      {
	itkExceptionMacro(<<"Mismatched between parameters size "
			  << parameters.size()
			  << " and region size "
			  << this->GetNumberOfParameters() );
      }

    // Copy it
    m_InternalParametersBuffer = parameters;
    m_InputParametersPointer = &m_InternalParametersBuffer;

    for (unsigned i = 0, tot = 0; i < m_nLabels; ++i)
    {
      unsigned int numberOfParameters = m_trans[i]->GetNumberOfParameters();
      ParametersType tmp(numberOfParameters);
      for (unsigned j = 0; j < numberOfParameters; ++j, ++tot)
        tmp.SetElement(j, parameters.GetElement(tot));
      m_trans[i]->SetParametersByValue(tmp);
    }
    InitJacobian();
    this->Modified();
  }


  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetBulkTransform(BulkTransformPointer b)
  {
    m_BulkTransform = b;
    LOOP_ON_LABELS(SetBulkTransform, b);
  }

#undef LOOP_ON_LABELS

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
#if ITK_VERSION_MAJOR >= 4
  inline typename MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>::NumberOfParametersType
#else
  inline unsigned int
#endif
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::GetNumberOfParameters(void) const
  {
    unsigned int sum = 0;
    for (unsigned i = 0; i < m_nLabels; ++i)
      sum += m_trans[i]->GetNumberOfParameters();
    return sum;
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline unsigned int
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::GetNumberOfParametersPerDimension(void) const
  {
    unsigned int sum = 0;
    for (unsigned i = 0; i < m_nLabels; ++i)
      sum += m_trans[i]->GetNumberOfParametersPerDimension();
    return sum;
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline const typename MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>::ParametersType &
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::GetParameters() const
  {
    if (NULL == m_InputParametersPointer)
      {
	itkExceptionMacro( <<"Cannot GetParameters() because m_InputParametersPointer is NULL. Perhaps SetCoefficientImage() has been called causing the NULL pointer." );
      }

    return (*m_InputParametersPointer);
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline const typename MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>::ParametersType &
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::GetFixedParameters() const
  {
    const ParametersType& trans0Para = m_trans[0]->GetFixedParameters();
    for (unsigned i = 0; i < trans0Para.Size() ; ++i)
      this->m_FixedParameters.SetElement(i, trans0Para.GetElement(i));
    this->m_FixedParameters[(5+NInputDimensions)*NInputDimensions + 2] = (double)((size_t) m_labels);
    this->m_FixedParameters[(5+NInputDimensions)*NInputDimensions + 3] = m_nLabels;
    return this->m_FixedParameters;
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::PrintSelf(std::ostream &os, itk::Indent indent) const
  {
    this->Superclass::PrintSelf(os, indent);
    os << indent << "nLabels" << m_nLabels << std::endl;
    itk::Indent ind = indent.GetNextIndent();

    for (unsigned i = 0; i < m_nLabels; ++i)
    {
      os << indent << "Label " << i << std::endl;
      m_trans[i]->Print(os, ind);
    }
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline bool
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::InsideValidRegion( const ContinuousIndexType& index ) const
  {
    bool res = false;
    for (unsigned i = 0; i < m_nLabels; ++i)
      res |= m_trans[i]->InsideValidRegion(index);
    return res;
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline typename MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>::OutputPointType
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::TransformPoint(const InputPointType &inputPoint) const
  {
    int lidx = 0;
    if (m_labels)
      lidx = m_labelInterpolator->Evaluate(inputPoint) - 1;
    if (lidx == -1)
      return inputPoint;
    return m_trans[lidx]->TransformPoint(inputPoint);
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline typename MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>::OutputPointType
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::DeformablyTransformPoint(const InputPointType &inputPoint) const
  {
    int lidx = 0;
    if (m_labels)
      lidx = m_labelInterpolator->Evaluate(inputPoint) - 1;
    if (lidx == -1)
      return inputPoint;
    return m_trans[lidx]->DeformablyTransformPoint(inputPoint);
  }

#if ITK_VERSION_MAJOR >= 4
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::ComputeJacobianWithRespectToParameters (const InputPointType &point, JacobianType &jacobian) const
  {
    if (m_LastJacobian != -1)
      m_trans[m_LastJacobian]->ResetJacobian();

    int lidx = 0;
    if (m_labels)
      lidx = m_labelInterpolator->Evaluate(point) - 1;
    if (lidx == -1)
    {
      m_LastJacobian = lidx;
      jacobian = this->m_SharedDataBSplineJacobian;
      return;
    }

    JacobianType unused;
    m_trans[lidx]->ComputeJacobianWithRespectToParameters(point, unused);
    m_LastJacobian = lidx;

    jacobian = this->m_SharedDataBSplineJacobian;
  }

#else
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline const typename MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>::JacobianType &
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::GetJacobian( const InputPointType & point ) const
  {
    if (m_LastJacobian != -1)
      m_trans[m_LastJacobian]->ResetJacobian();

    int lidx = 0;
    if (m_labels)
      lidx = m_labelInterpolator->Evaluate(point) - 1;
    if (lidx == -1)
    {
      m_LastJacobian = lidx;
      return this->m_Jacobian;
    }

    m_trans[lidx]->GetJacobian(point);
    m_LastJacobian = lidx;

    return this->m_Jacobian;
  }
#endif

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions,NOutputDimensions>
  ::TransformPointToContinuousIndex( const InputPointType & point, ContinuousIndexType & index ) const
  {
    m_trans[0]->TransformPointToContinuousIndex(point, index);
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline void
  MultipleBSplineDeformableTransform<TCoordRep, NInputDimensions,NOutputDimensions>::InitJacobian()
  {
    unsigned numberOfParameters = this->GetNumberOfParameters();
#if ITK_VERSION_MAJOR >= 4
    this->m_SharedDataBSplineJacobian.set_size(OutputDimension, numberOfParameters);
    JacobianPixelType * jacobianDataPointer = reinterpret_cast<JacobianPixelType *>(this->m_SharedDataBSplineJacobian.data_block());
#else
    this->m_Jacobian.set_size(OutputDimension, numberOfParameters);
    JacobianPixelType * jacobianDataPointer = reinterpret_cast<JacobianPixelType *>(this->m_Jacobian.data_block());
#endif
    memset(jacobianDataPointer, 0,  numberOfParameters * sizeof (JacobianPixelType));

    unsigned tot = 0;
    for (unsigned int j = 0; j < OutputDimension; j++)
    {
      for (unsigned i = 0; i < m_nLabels; ++i)
      {
        unsigned numberOfPixels = m_trans[i]->SetJacobianImageData(jacobianDataPointer, j);
        jacobianDataPointer += numberOfPixels;
        tot += numberOfPixels;
      }
    }
    assert(tot == numberOfParameters);
  }

}  // namespace clitk

#endif // __clitkMultipleBSplineDeformableTransform_txx
