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

#include "clitkTransformUtilities.h"

namespace clitk
{

//--------------------------------------------------------------------
template < >
itk::Matrix<double, 3, 3>
GetForwardAffineMatrix<2>(itk::Array<double> transformParameters)
{
  return GetForwardAffineMatrix2D(transformParameters);
}

//--------------------------------------------------------------------
template < >
itk::Matrix<double, 4, 4>
GetForwardAffineMatrix<3>(itk::Array<double> transformParameters)
{
  return GetForwardAffineMatrix3D(transformParameters);
}

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

//--------------------------------------------------------------------
template <>
itk::Matrix<double, 2, 2> GetRotationMatrix<2>(itk::Array<double> rotationParameters)
{
  return GetRotationMatrix2D(rotationParameters);
}

//--------------------------------------------------------------------
template <>
itk::Matrix<double, 3, 3> GetRotationMatrix<3>(itk::Array<double> rotationParameters)
{
  return GetRotationMatrix3D(rotationParameters);
}

//--------------------------------------------------------------------
itk::Matrix<double, 4, 4> ReadMatrix3D(std::string fileName)
{
  // read input matrix
  std::ifstream is;
  openFileForReading(is, fileName);
  std::vector<double> nb;
  double x;
  skipComment(is);
  is >> x;
  while (is && !is.eof()) {
    nb.push_back(x);
    skipComment(is);
    is >> x;
  }

  if(nb.size() != 16)
    itkGenericExceptionMacro(<< "Could not read 4x4 matrix in " << fileName);

  //copy it to the matrix
  itk::Matrix<double, 4, 4> matrix;
  unsigned int index=0;
  for (unsigned int i=0;i<4;i++)
    for (unsigned int j=0;j<4;j++)
      matrix[i][j]=nb[index++];
  return matrix;
}

}
