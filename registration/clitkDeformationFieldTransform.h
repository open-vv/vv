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
#ifndef __clitkDeformationFieldTransform_h
#define __clitkDeformationFieldTransform_h
#include "clitkList.h"
#include "clitkLists.h"

// itk
#include "itkTransform.h"
#include "itkVectorInterpolateImageFunction.h"
#include "itkVectorLinearInterpolateImageFunction.h"

namespace clitk
{

  template < class TScalarType=double, unsigned int InputDimension=4,unsigned int OutputDimension=4, unsigned int SpaceDimension=3 >        
  class ITK_EXPORT DeformationFieldTransform : public itk::Transform< TScalarType, InputDimension, OutputDimension >
  {
  public:
    /** Standard class typedefs. */
    typedef DeformationFieldTransform                                      Self;
    typedef itk::Transform< TScalarType, InputDimension, OutputDimension > Superclass;
    typedef itk::SmartPointer<Self>                                 Pointer;
    typedef itk::SmartPointer<const Self>                           ConstPointer;
      
    /** New macro for creation of through the object factory. */
    itkNewMacro( Self );

    /** Run-time type information (and related methods). */
    itkTypeMacro( DeformationFieldTransform, Transform );

    /** Standard coordinate point type for this class. */
    typedef itk::Point<TScalarType, InputDimension> InputPointType;
    typedef itk::Point<TScalarType, OutputDimension> OutputPointType;

    // typedef 
    typedef  itk::Point<double, SpaceDimension> SpacePointType;
    typedef itk::Vector<double, SpaceDimension> DisplacementType;
    typedef itk::Image<DisplacementType, InputDimension> DeformationFieldType; 
    typedef itk::VectorInterpolateImageFunction<DeformationFieldType, double> InterpolatorType;
    typedef itk::VectorLinearInterpolateImageFunction<DeformationFieldType, double> DefaultInterpolatorType;

    // Members
    void SetDeformationField (typename DeformationFieldType::Pointer p){m_DeformationField=p; m_Interpolator->SetInputImage(m_DeformationField);}
    void SetInterpolator (typename InterpolatorType::Pointer i){ m_Interpolator=i; m_Interpolator->SetInputImage(m_DeformationField);}
    OutputPointType TransformPoint(const InputPointType  &point ) const;
   
  protected:
    DeformationFieldTransform();
    ~DeformationFieldTransform(){;}

  private:
    DeformationFieldTransform(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    typename DeformationFieldType::Pointer m_DeformationField;
    typename InterpolatorType::Pointer m_Interpolator;


  }; //class DeformationFieldTransform



}  // namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkDeformationFieldTransform.txx"
#endif

#endif /* __clitkDeformationFieldTransform_h */
