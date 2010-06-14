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

#ifndef CLITKAFFINEREGISTRATIONGENERICFILTER_CXX
#define CLITKAFFINEREGISTRATIONGENERICFILTER_CXX

#include "clitkAffineRegistrationGenericFilter.h"

namespace clitk
{

//====================================================================
// Constructor
AffineRegistrationGenericFilter::AffineRegistrationGenericFilter()
{
  m_Verbose=false;
}

//====================================================================
// Update
void AffineRegistrationGenericFilter::Update()
{
  //Read the PixelType and dimension of the reference image
  int Dimension;
  std::string PixelType;
  clitk::ReadImageDimensionAndPixelType(m_ArgsInfo.reference_arg, Dimension, PixelType);
  if (Dimension == 2) UpdateWithDim<2>(PixelType);
  else if (Dimension == 3) UpdateWithDim<3>(PixelType);
  else {
    itkExceptionMacro(<< "Reference Image dimension is " << Dimension
                      << " but I can only work on 2D and 3D images.");
  }
}

//void AffineRegistrationGenericFilter::SetTransfo( itk::MultiResolutionImageRegistrationMethod<itk::Image<float,2>,itk::Image<float,2> >::Pointer & registration  )
//{
//  typedef itk::Euler2DTransform < double > Transform2DType;
//  Transform2DType::Pointer t2 =Transform2DType::New();
//
//  //Initializing the transform
//  Transform2DType::OutputVectorType translation;
//  translation[0] = m_ArgsInfo.transX_arg;
//  translation[1] = m_ArgsInfo.transY_arg;
//  t2->SetTranslation(translation);
//  t2->SetRotation(m_ArgsInfo.rotX_arg);
//
//  //For simplicity we set the center to the top left corner
//  Transform2DType::InputPointType center;
//  center[0] = 0.;
//  center[1] = 0.;
//  t2->SetCenter(center);
//  registration->SetTransform(t2);
//  registration->SetInitialTransformParameters(t2->GetParameters());
//}

//void clitk::AffineRegistrationGenericFilter::SetTransfo( itk::MultiResolutionImageRegistrationMethod<itk::Image<float,3>,itk::Image<float,3> >::Pointer & registration )
//{
//  typedef itk::Euler3DTransform < double > Transform3DType;
//  Transform3DType::Pointer t3 = Transform3DType::New();
//
//  t3->SetComputeZYX(true);
//  //Initializing the transform
//  Transform3DType::OutputVectorType translation;
//  translation[0] = m_ArgsInfo.transX_arg;
//  translation[1] = m_ArgsInfo.transY_arg;
//  translation[2] = m_ArgsInfo.transZ_arg;
//  t3->SetTranslation(translation);
//  t3->SetRotation(m_ArgsInfo.rotX_arg, m_ArgsInfo.rotY_arg, m_ArgsInfo.rotZ_arg);
//
//  //For simplicity we set the center to the top left corner
//  Transform3DType::InputPointType center;
//  center[0] = 0.;
//  center[1] = 0.;
//  center[2] = 0.;
//  t3->SetCenter(center);
//  registration->SetTransform(t3);
//  registration->SetInitialTransformParameters( t3->GetParameters());
//}

}
//====================================================================

#endif  //#define CLITKAFFINEREGISTRATIONCGENERICFILTER_CXX
