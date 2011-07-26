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
const int TEST_EXITED=1;
int getOutputOptionIndex(int argc, char** argv){
  for(int i=1; i<argc; i++){
      std::string s = argv[i];
      if(s=="-o"){
	 return i+1;
      }
  }
  return NO_OUTPUT_OPTION;
}

std::string getTmpFileName(){
  char fileName[] = "/tmp/vvTempXXXXXX";
  
  #ifdef _WIN32
    errno_t err = tmpfile_s(&fileName, strlen(fileName));
  #else
    int err=0;
    int fd = mkstemp(fileName);
    if(fd==-1) err=1;
  #endif
  if(err){
   std::cout<<"couldnot create file. Exiting"<<std::endl;
   exit(TEST_EXITED);
  }
  return std::string(fileName);
}


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
  //reference file must exist or we fail
  char* refFile = argv[argc-1];
  if(!(itksys::SystemTools::FileExists(refFile, true))){
    std::cout<<"refFile "<<refFile<<" doesn't exist"<<std::endl; 
    return 1;
  } 
  
  std::ostringstream cmd_line;
  cmd_line<<CLITK_TEST_TOOLS_PATH;
  for(int i=1; i<argc-1; i++){
      //we should ensure the file exists, find an -i index or a long file name maybe?
      cmd_line<<argv[i]<<" ";
  }

  //look for the need of generating an output file
  int outputOptionIndex = getOutputOptionIndex(argc, argv);
  std::string outFile;
  if(NO_OUTPUT_OPTION==outputOptionIndex){
    outFile = getTmpFileName();
    std::cout<<outFile<<std::endl;
    cmd_line<<">"<<outFile;
  }else{
    //todo test this else branch
    outFile =  std::string(CLITK_TEST_DATA_PATH);
    outFile += argv[outputOptionIndex];
  }
  //run the command line
  system(cmd_line.str().c_str());
  
  //files should be equal, so if this is the case return success=0
  int fail = (itksys::SystemTools::FilesDiffer(outFile.c_str(), refFile))?1:0;
  remove(outFile.c_str());
  return fail;
}
