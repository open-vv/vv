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

#include "clitkGenericAffineTransform.h"
#include "clitkAffineRegistration_ggo.h"

#include <itkEuler2DTransform.h>
#include <itkEuler3DTransform.h>

namespace clitk
{
//=========================================================================================================================
template<>
itk::MatrixOffsetTransformBase<double, 2, 2>::Pointer
clitk::GenericAffineTransform<args_info_clitkAffineRegistration, double, 2>::GetNewEulerTransform()
{
  typedef itk::Euler2DTransform< double > Transform2DType;
  Transform2DType::Pointer t2 = Transform2DType::New();
  itk::MatrixOffsetTransformBase<double, 2, 2>::Pointer pp;
  pp = t2;
  return pp;
}

//=========================================================================================================================
template<>
itk::MatrixOffsetTransformBase<double, 3, 3>::Pointer
clitk::GenericAffineTransform<args_info_clitkAffineRegistration, double, 3>::GetNewEulerTransform()
{
  typedef itk::Euler3DTransform < double > Transform3DType;
  Transform3DType::Pointer t3 = Transform3DType::New();
  t3->SetComputeZYX(true); //SR: required?
  itk::MatrixOffsetTransformBase<double, 3, 3>::Pointer pp;
  pp = t3;
  return pp;
}
}
