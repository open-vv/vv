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

#include "clitkMatrix.h"
#include <algorithm>

//--------------------------------------------------------------------
namespace clitk {

//-------------------------------------------------------------------
std::string
Get4x4MatrixDoubleAsString(vtkMatrix4x4 *matrix,
                           const int precision)
{
  std::ostringstream strmatrix;

  // Figure out the number of digits of the integer part of the largest absolute value
  // for each column
  unsigned width[4];
  for (unsigned int j = 0; j < 4; j++){
    double absmax = 0.;
    for (unsigned int i = 0; i < 4; i++)
      absmax = std::max(absmax, std::abs(matrix->GetElement(i, j)));
    unsigned ndigits = (unsigned)std::max(0.,std::log10(absmax))+1;
    width[j] = precision+ndigits+3;
  }

  // Output with correct width, aligned to the right
  for (unsigned int i = 0; i < 4; i++) {
    for (unsigned int j = 0; j < 4; j++) {
      strmatrix.setf(ios::fixed,ios::floatfield);
      strmatrix.precision(precision);
      strmatrix.fill(' ');
      strmatrix.width(width[j]);
      strmatrix << std::right << matrix->GetElement(i, j);
    }
    strmatrix << std::endl;
  }
  std::string result = strmatrix.str().c_str();
  return result;
}
//-------------------------------------------------------------------

//-------------------------------------------------------------------
std::string
Get4x4MatrixDoubleAsString(itk::Matrix<double, 4, 4> m,
                           const int precision)
{
  vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
  for (unsigned int j = 0; j < 4; j++)
    for (unsigned int i = 0; i < 4; i++)
      matrix->SetElement(j,i,m[j][i]);
  return Get4x4MatrixDoubleAsString(matrix, precision);
}
//-------------------------------------------------------------------
}
//-------------------------------------------------------------------
