/*=========================================================================
  Program:         vv http://www.creatis.insa-lyon.fr/rio/vv
  Main authors :   XX XX XX

  Authors belongs to: 
  - University of LYON           http://www.universite-lyon.fr/
  - Léon Bérard cancer center    http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory      http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence
  - BSD       http://www.opensource.org/licenses/bsd-license.php
  - CeCILL-B  http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html

  =========================================================================*/

#include <fstream>
#include "clitkCommon.h"
#include "clitkMergeAsciiDoseActor_ggo.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkMergeAsciiDoseActor, args_info);

  // Read input 1
  std::vector<double> input1;
  clitk::readDoubleFromFile(args_info.input1_arg, input1);
  DD(input1.size());

  // Read input 2
  std::vector<double> input2;
  clitk::readDoubleFromFile(args_info.input2_arg, input2);
  DD(input2.size());

  // Check 
  if (input1.size() != input2.size()) {
      std::cerr << "Error. Please provide input file with the same number of values. Read " 
		<< input1.size()  << " in " << args_info.input1_arg 
		<< " and " << input2.size() << " in " << args_info.input2_arg << std::endl;
      exit(0);
  }
  
  // Add
  for(unsigned int i=0; i<input1.size(); i++) {
    input1[i] += input2[i];
  }

  // Write output
  std::ofstream os;
  clitk::openFileForWriting(os, args_info.output_arg);
  os << "## Merge " << input1.size() << " values" << std::endl;
  for(unsigned int i=0; i<input1.size(); i++) {
    os << input1[i] << std::endl;
  }  
  os.close();

  // This is the end my friend 
  return 0;
}
