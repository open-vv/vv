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

#include "clitkTransformUtilities.h"

namespace clitk
{

//--------------------------------------------------------------------
template < >
itk::Matrix<double, 3, 3>
GetBackwardAffineMatrix<2>(itk::Array<double> transformParameters)
{
  return GetBackwardAffineMatrix2D(transformParameters);
}

//--------------------------------------------------------------------
template < >
itk::Matrix<double, 4, 4>
GetBackwardAffineMatrix<3>(itk::Array<double> transformParameters)
{
  return GetBackwardAffineMatrix3D(transformParameters);
}

}
