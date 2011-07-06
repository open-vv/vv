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
===========================================================================*/
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <itksys/SystemTools.hxx>
int main(int argc, char** argv){
  int fail = 1;
  std::string mhd = std::string(CLITK_TEST_DATA_PATH);
  std::string ref = std::string(CLITK_TEST_DATA_PATH);
  
  //We select which image we use to test clitkImageInfo
  int idImg = atoi(argv[1]);
  switch(idImg){
     case '0':
	mhd += "Deformation4D.mhd";
	ref += "clitkImageInfoTestRef";
      break;
     case '1':
	mhd += "Lung3D.mhd";
	ref += "clitkImageInfoTestRef";
      break;
    default:
      return fail;
  }  
  //end of selection
  
  //ensure mhd do exist
  if(!itksys::SystemTools::FileExists(mhd.c_str(), true)){
      return fail;
  }
  //ensure ref do exist
  if(!itksys::SystemTools::FileExists(ref.c_str(), true)){
      return fail;
  }
  
  std::ostringstream cmd_line;
  cmd_line << TOOLS_PATH <<"clitkImageInfo " << mhd << " > clitkImageInfoTest.out";
  system(cmd_line.str().c_str());
  
  //files should be equal, so if this is the case return success=0
  fail = (itksys::SystemTools::FilesDiffer("clitkImageInfoTest.out", ref.c_str()))?0:1;
  remove("clitkImageInfoTest.out");
  return fail;
}
