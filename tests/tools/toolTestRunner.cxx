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
const int NO_OUTPUT_OPTION=-1;
int getOutputOptionIndex(int argc, char** argv){
  for(int i=1; i<argc; i++){
      std::string s = argv[i];
      if(s=="-o"){
	 return i+1;
      }
  }
  return NO_OUTPUT_OPTION;
}
char* getTmpFileName(){
  char buffer [L_tmpnam];
  char * pointer;

  char* back = tmpnam (buffer);
  if(back==NULL){
    std::cout<<"fail to get temporary file name"<<std::endl;
    exit(1);
  }
  return pointer;
}
//todo, check if files exist
/**
 * argv
 * [1] executable
 * [2] random options
 * [2.x] -o
 * [2.x+1] outFileName
 * [3] reference file
 * 
 * [2.x] and [2.x+1] are optional
 */
int main(int argc, char** argv){
  std::ostringstream cmd_line;
  for(int i=1; i<argc; i++){
      cmd_line<<argv[i]<<" ";
  }
  int outputOptionIndex = getOutputOptionIndex(argc, argv);
  char* outFile;
  if(NO_OUTPUT_OPTION==outputOptionIndex){
    outFile = getTmpFileName();
    cmd_line<<" "<<outFile;
  }else{
    outFile = argv[outputOptionIndex];
  }
  
  std::cout<<"running "<<cmd_line.str()<<std::endl;
  system(cmd_line.str().c_str());
  //files should be equal, so if this is the case return success=0
  char* refFile = argv[argc-1];
  std::cout<<"comapring "<<outFile<<" to "<<refFile<<std::endl;
  bool fail = (itksys::SystemTools::FilesDiffer(outFile, refFile))?0:1;
  remove(outFile);
  return fail;
}
