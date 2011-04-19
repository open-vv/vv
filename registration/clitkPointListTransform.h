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
#ifndef __clitkPointListTransform_h
#define __clitkPointListTransform_h
#include "clitkList.h"
#include "clitkLists.h"

// itk
#include "itkTransform.h"
#include "itkVectorInterpolateImageFunction.h"

namespace clitk
{

  template < class TScalarType=double, unsigned int NDimensions=4,unsigned int NOutputDimensions=4 >        
  class ITK_EXPORT PointListTransform : public itk::Transform< TScalarType, NDimensions, NOutputDimensions >
  {
  public:
    /** Standard class typedefs. */
    typedef PointListTransform                                      Self;
    typedef itk::Transform< TScalarType, NDimensions, NOutputDimensions > Superclass;
    typedef itk::SmartPointer<Self>                                 Pointer;
    typedef itk::SmartPointer<const Self>                           ConstPointer;
      
    /** New macro for creation of through the object factory. */
    itkNewMacro( Self );

    /** Run-time type information (and related methods). */
    itkTypeMacro( PointListTransform, Transform );

    /** Dimension of the domain space. */
    itkStaticConstMacro(SpaceDimension, unsigned int, NDimensions-1);
    itkStaticConstMacro(ImageDimension, unsigned int, NDimensions);

    // typedef 
    typedef  itk::Point<double, SpaceDimension> SpacePointType;
    typedef  itk::Point<double, ImageDimension> SpaceTimePointType;
    typedef clitk::List<SpacePointType> PointListType;
    typedef clitk::Lists<SpacePointType> PointListsType;
    typedef itk::Vector<double, SpaceDimension> PointListImagePixelType;
    typedef itk::Image<PointListImagePixelType, 1> PointListImageType; 
    typedef itk::VectorInterpolateImageFunction<PointListImageType, double> InterpolatorType;

    void SetPointList (PointListType p){m_PointList=p;}
    void SetPointLists (PointListsType p){m_PointLists=p;}
    void SetInterpolator (typename InterpolatorType::Pointer i){ m_Interpolator=i;}

    /** Standard coordinate point type for this class. */
    typedef itk::Point<TScalarType, itkGetStaticConstMacro(ImageDimension)> InputPointType;
    typedef itk::Point<TScalarType, itkGetStaticConstMacro(ImageDimension)> OutputPointType;


    PointListType GetCorrespondingPointList(const InputPointType &inputPoint) const; 
    OutputPointType TransformPoint(const InputPointType  &point ) const;

    // Methods needed to inherit from an itk::Transform which should not be used
    // in this class
    /** Type of the input parameters. */
    typedef typename Superclass::ParametersType        ParametersType;
    typedef typename Superclass::ParametersValueType   ParametersValueType;

    /** Type of the Jacobian matrix. */
    typedef typename Superclass::JacobianType		JacobianType;

    /** Standard vector type for this class. */
    typedef typename Superclass::InputVectorType InputVectorType;
    typedef typename Superclass::OutputVectorType OutputVectorType;

    /** Standard covariant vector type for this class */
    typedef typename Superclass::InputCovariantVectorType	InputCovariantVectorType;
    typedef typename Superclass::OutputCovariantVectorType	OutputCovariantVectorType;

    /** Standard vnl_vector type for this class. */
    typedef typename Superclass::InputVnlVectorType	InputVnlVectorType;
    typedef typename Superclass::OutputVnlVectorType	OutputVnlVectorType;

    void SetParameters(const ParametersType&)
    {
      itkExceptionMacro( << "PointListTransform doesn't declare SetParameters" );
    }

    void SetFixedParameters(const ParametersType&)
    {
      itkExceptionMacro( << "PointListTransform doesn't declare SetFixedParameters" );
    }

    virtual OutputVectorType TransformVector(const InputVectorType &) const
    {
      itkExceptionMacro(<< "Method not applicable for deformable transform." );
      return OutputVectorType();
    }

    virtual OutputVnlVectorType TransformVector(const InputVnlVectorType &) const
    {
      itkExceptionMacro(<< "Method not applicable for deformable transform. ");
      return OutputVnlVectorType();
    }

    virtual OutputCovariantVectorType TransformCovariantVector(const InputCovariantVectorType &) const
    {
      itkExceptionMacro(<< "Method not applicable for deformable transfrom. ");
      return OutputCovariantVectorType();
    }

    virtual const JacobianType& GetJacobian(const InputPointType  &point ) const
    {
      itkExceptionMacro( << "PointListTransform doesn't declare GetJacobian" );
      return this->m_Jacobian;
    }

  protected:
    PointListTransform();
    ~PointListTransform(){;}

  private:
    PointListTransform(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    PointListsType m_PointLists;
    mutable PointListType m_PointList;
    typename InterpolatorType::Pointer m_Interpolator;


  }; //class PointListTransform



}  // namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkPointListTransform.txx"
#endif

#endif /* __clitkPointListTransform_h */
