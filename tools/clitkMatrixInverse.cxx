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

// clitk
#include "clitkMatrixInverse_ggo.h"
#include "clitkTransformUtilities.h"
#include "clitkIO.h"
#include "clitkMatrix.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[])
{
  // Init command line
  GGO(clitkMatrixInverse, args_info);
  CLITK_INIT;

  // Read matrix
  itk::Matrix<double, 4, 4> matrix;
  try {
    matrix = clitk::ReadMatrix3D(args_info.input_arg);
  }
  catch (itk::ExceptionObject & err) {
    std::cerr << "Error reading " << args_info.input_arg << std::endl;
    std::cerr << err.GetDescription() << std::endl;
    exit(-1);
  }

  matrix = matrix.GetInverse();

  // Print matrix
  std::ofstream os;
  clitk::openFileForWriting(os, args_info.output_arg);
  os << clitk::Get4x4MatrixDoubleAsString(matrix, 16);
  os.close();

  return EXIT_SUCCESS;
}// end main

//--------------------------------------------------------------------
