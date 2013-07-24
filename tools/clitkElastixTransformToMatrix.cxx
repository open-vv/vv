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
#include "clitkElastixTransformToMatrix_ggo.h"
#include "clitkAffineTransformGenericFilter.h"
#include "clitkElastix.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[])
{

  // Init command line
  GGO(clitkElastixTransformToMatrix, args_info);
  CLITK_INIT;

  // Use static fct of AffineTransformGenericFilter
  std::vector<std::string> l;
  l.push_back(args_info.input_arg);
  itk::Matrix<double, 4, 4> m = clitk::createMatrixFromElastixFile<3>(l, args_info.verbose_flag);

  // Print matrix
  std::ofstream os;
  clitk::openFileForWriting(os, args_info.output_arg);
  for(unsigned int i=0; i<4; i++) {
    for(unsigned int j=0; j<4; j++)
      os << m[i][j] << " ";
    os << std::endl;
  }
  os.close();  

  return EXIT_SUCCESS;
}// end main

//--------------------------------------------------------------------
