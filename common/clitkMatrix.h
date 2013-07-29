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

#ifndef clitkMatrix_h
#define clitkMatrix_h

#include <itkMatrix.h>
#define VTK_EXCLUDE_STRSTREAM_HEADERS
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>

//--------------------------------------------------------------------
namespace clitk {
std::string
Get4x4MatrixDoubleAsString(vtkMatrix4x4 *matrix,
                           const int precision=3);

std::string
Get4x4MatrixDoubleAsString(itk::Matrix<double, 4, 4> m,
                           const int precision=3);
}
//-------------------------------------------------------------------

#endif
