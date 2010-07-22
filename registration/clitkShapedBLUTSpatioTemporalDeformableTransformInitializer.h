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
#ifndef __clitkShapedBLUTSpatioTemporalDeformableTransformInitializer_h
#define __clitkShapedBLUTSpatioTemporalDeformableTransformInitializer_h
#include "clitkResampleBSplineDeformableTransformImageFilter.h"

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkImageFileReader.h"
#include <iostream>

namespace clitk
{


template < class TTransform, class TImage >
class ITK_EXPORT ShapedBLUTSpatioTemporalDeformableTransformInitializer : public itk::Object
{
public:
  /** Standard class typedefs. */
  typedef ShapedBLUTSpatioTemporalDeformableTransformInitializer     Self;
  typedef itk::Object                               Superclass;
  typedef itk::SmartPointer<Self>                   Pointer;
  typedef itk::SmartPointer<const Self>             ConstPointer;

  /** New macro for creation of through a Smart Pointer. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( ShapedBLUTSpatioTemporalDeformableTransformInitializer, Object );

  // Typedefs
  typedef TTransform                                TransformType;
  typedef typename TransformType::Pointer           TransformPointer;
  typedef typename TransformType::RegionType        RegionType;
  typedef typename TransformType::OriginType        OriginType;
  typedef typename TransformType::DirectionType        DirectionType;
  typedef typename RegionType::SizeType             SizeType;
  typedef typename TransformType::SpacingType       SpacingType;
  itkStaticConstMacro(InputDimension, unsigned int, TransformType::InputDimension);
  typedef   TImage                                  ImageType;
  typedef typename ImageType::ConstPointer        ImagePointer;
  typedef typename TransformType::CoefficientImageType CoefficientImageType;
  typedef typename TransformType::ParametersType ParametersType;

  // Set and Get
  itkBooleanMacro(Verbose);
  itkSetMacro( Verbose, bool);
  itkGetConstReferenceMacro( Verbose, bool);
  itkSetObjectMacro( Transform,   TransformType   );
  itkGetConstObjectMacro( Transform,   TransformType   );
  itkSetObjectMacro( Image,  ImageType  );
  itkGetConstObjectMacro( Image,  ImageType  );

  void SetSplineOrder(const unsigned int & splineOrder)
  {
    SizeType s;
    s.Fill(splineOrder);
    this->SetSplineOrders(s);
  }
  void SetSplineOrders(const SizeType & splineOrders)
  {
    m_SplineOrders=splineOrders;
  }
  void SetNumberOfControlPointsInsideTheImage(  SizeType & n  )
  {
    m_NumberOfControlPointsInsideTheImage=n;
    m_NumberOfControlPointsIsGiven=true;
    this->Modified();
  }
  void SetNumberOfControlPointsInsideTheImage(  int * n)
  {
    SizeType s;
    for (unsigned int i=0;i<InputDimension;i++) 
      s[i]=n[i];
    this->SetNumberOfControlPointsInsideTheImage( s );
  }
  void SetNumberOfControlPointsInsideTheImage(  unsigned int & n )
  {
    SizeType s;
    s.Fill( n );;
    this->SetNumberOfControlPointsInsideTheImage( s );
  }
  void SetControlPointSpacing( SpacingType n )
  {
    m_ControlPointSpacing= n;
    m_ControlPointSpacingIsGiven=true;
    this->Modified();
  }
  void SetControlPointSpacing( double*& n )
  {
    SpacingType s( n );
    this->SetControlPointSpacing(s);
  }
  void SetControlPointSpacing( double n )
  {
    SpacingType s;
    s.Fill( n );
    this->SetControlPointSpacing(s);
  }
  void SetSamplingFactors(  SizeType n  )
  {
    m_SamplingFactors=n;
    m_SamplingFactorIsGiven=true;
    this->Modified();
  }
  void SetSamplingFactors(  int *& n)
  {
    SizeType s;
    for (unsigned int i=0;i<InputDimension;i++) 
      s[i]=n[i];
    this-> SetSamplingFactors( s );
  }
  void SetSamplingFactors( unsigned  int n )
  {
    SizeType s;
    s.Fill( n );
    this-> SetSamplingFactors( s );
  }
  virtual void InitializeTransform();
  void SetInitialParameters(const typename CoefficientImageType::Pointer coefficientImage, ParametersType& params);
  void SetInitialParameters(const std::string & s, ParametersType& params);
//   void SetInitialPaddedParameters(const typename CoefficientImageType::Pointer coefficientImage, ParametersType& params);
//   void SetInitialPaddedParameters(const std::string & s, ParametersType& params);

  // For easy acces, declared public
  std::vector<SizeType> m_NumberOfControlPointsInsideTheImageArray;
  std::vector<SizeType> m_SamplingFactorsArray;
  std::vector<SpacingType> m_ControlPointSpacingArray;

  SpacingType   m_ControlPointSpacing;
  SizeType   m_SamplingFactors;
  SizeType   m_SplineOrders;
  SpacingType   m_ChosenSpacing;
  SizeType   m_NumberOfControlPointsInsideTheImage;
  bool m_NumberOfControlPointsIsGiven;
  bool m_ControlPointSpacingIsGiven;
  bool m_SamplingFactorIsGiven;
  bool m_TransformRegionIsGiven;
  unsigned int m_TrajectoryShape;

  typename TransformType::ParametersType* m_Parameters;

protected:
  ShapedBLUTSpatioTemporalDeformableTransformInitializer();
  ~ShapedBLUTSpatioTemporalDeformableTransformInitializer(){};

private:
  ShapedBLUTSpatioTemporalDeformableTransformInitializer(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  bool m_Verbose;
  TransformPointer    m_Transform;
  ImagePointer        m_Image;
  bool m_BC1;
  bool m_BC2;

}; //class ShapedBLUTSpatioTemporalDeformableTransformInitializer


}  // namespace clitk


#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkShapedBLUTSpatioTemporalDeformableTransformInitializer.txx"
#endif

#endif /* __clitkShapedBLUTSpatioTemporalDeformableTransformInitializer_h */
