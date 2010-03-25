/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/
/**
   =================================================
   * @file   clitkImageInfo.cxx
   * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
   * @date   02 Jul 2006
   =================================================*/

// itk include
#include "itkImageIOBase.h"

// clitk include
#include "clitkImageInfo_ggo.h"
#include "clitkIO.h"
#include "clitkImageCommon.h"

//====================================================================
int main(int argc, char * argv[]) {

  // init command line
  GGO(clitkImageInfo, args_info);
  CLITK_INIT;

  // check arg
  if (args_info.inputs_num == 0) return 0;

  // read Header 
  for(unsigned int i=0; i<args_info.inputs_num; i++) {
    itk::ImageIOBase::Pointer header = clitk::readImageHeader(args_info.inputs[i]);
    if (header) {
      if (args_info.name_flag) std::cout << "[" << args_info.inputs[i] << "]\t ";
      if (args_info.long_given) {
	//	std::cout << std::endl;
	clitk::printImageHeader(header, std::cout, args_info.long_arg);
      }
      else {
	if (args_info.verbose_flag) clitk::printImageHeader(header, std::cout, 1);
	else {
	  clitk::printImageHeader(header, std::cout, 0);
	  std::cout << std::endl;
	}
      }
    }  // heade null ; non fatal error
      else {
	std::cerr << "*** Warning : I could not read '" << args_info.inputs[i] << "' ***" << std::endl;
      }
  }

  // this is the end my friend  
  return 0;
}
//====================================================================
