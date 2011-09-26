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
#ifdef _WIN32
  char fileName[L_tmpnam_s];
  errno_t err = tmpnam_s(fileName);
#else
  char fileName[] = "/tmp/vvTempXXXXXX";
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

void assertFalse(int fail, const std::string &message=""){
  if(fail){
    std::cout<<message<<std::endl; 
    exit(1);
  }
}

#ifdef _WIN32
void dosToUnixFile(std::string dosFile, std::string unixedFile){
		
	std::ifstream ifile(dosFile.c_str(),std::ios::binary);
	ifile.seekg(0,std::ios_base::end);
	long s=ifile.tellg();
	char *buffer=new char[s];
	ifile.seekg(0);
	ifile.read(buffer,s);
	ifile.close();
	std::string txt(buffer,s);
	delete[] buffer;
	size_t off=0;
	while ((off=txt.find("\r\n",off))!=std::string::npos)
		txt.replace(off,sizeof("\r\n")-1,"\n");
	std::ofstream ofile(unixedFile.c_str());
	ofile.write(txt.c_str(),txt.size());
	
}
#endif

std::string mhdToRawName(const std::string &mhdName){
	int found = mhdName.find_last_of(".");
  return mhdName.substr(0, found)+".raw";
}
/**
 * argv
 * [1] executable
 * [2] random options
 * [2.x] -o
 * [3] reference file
 * 
 * [2.x] is optional. If set a temporary file will be generated. So NO need to pass a random outputFileName
 */
int main(int argc, char** argv){
  //reference file must exist or we fail
  char* refFile = argv[argc-1];
	std::string strRefFile = std::string(refFile);
  assertFalse(!(itksys::SystemTools::FileExists(refFile, true)), "refFile "+strRefFile+" doesn't exist");
  
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
     cmd_line<<">"<<outFile;
  }else{
     outFile = argv[argc-2];
  }
  std::cout<<cmd_line.str()<<std::endl;;
  //run the command line
  system(cmd_line.str().c_str());
  
  
	//compare source files
#ifdef _WIN32
	std::string unixedOutFile= getTmpFileName();
	//replace \r\n
	dosToUnixFile(outFile, unixedOutFile);
	assertFalse((itksys::SystemTools::FilesDiffer(unixedOutFile.c_str(), refFile)), "Generated mhd file != ref File");
  remove(unixedOutFile.c_str());
#else
  assertFalse((itksys::SystemTools::FilesDiffer(outFile.c_str(), refFile)), "Generated mhd file != ref File");
#endif
  
  std::string refRawFile = mhdToRawName(strRefFile);
  std::string rawFile = mhdToRawName(outFile);
  
  if((itksys::SystemTools::FileExists(refRawFile.c_str(), true))){
    //compare the raw stuff
    if((itksys::SystemTools::FileExists(rawFile.c_str(), true))){
       std::cout<<"Checking raws"<<std::endl;
       assertFalse(itksys::SystemTools::FilesDiffer(refRawFile.c_str(), rawFile.c_str()), "Raws are different");
    }
    //file is not removed if there is a fail
    remove(rawFile.c_str());
  }
  //neither the mhd is
  remove(outFile.c_str());
  
  //success
  return 0;
}


