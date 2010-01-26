/*-------------------------------------------------------------------------
                                                                                
Program:   clitk
Language:  C++
                                                                                
Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
l'Image). All rights reserved. See Doc/License.txt or
http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.
                                                                             
-------------------------------------------------------------------------*/

#ifndef CLITKIMAGECONVERT_CXX
#define CLITKIMAGECONVERT_CXX

/**
   -------------------------------------------------
   * @file   clitkImageConvert.cxx
   * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
   * @date   03 Jul 2006 10:28:32
   -------------------------------------------------*/

// clitk include
#include "clitkImageConvert_ggo.h"
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkImageConvertGenericFilter.h"

//-------------------------------------------------------------------=
int main(int argc, char * argv[]) {

  // init command line
  GGO(clitkImageConvert, args_info);
  CLITK_INIT;

  // Get list of filenames
  std::vector<std::string> l;
  for(unsigned int i=0; i<args_info.input_given; i++) {
    l.push_back(args_info.input_arg[i]);
  }
  for(unsigned int i=0; i<args_info.inputs_num; i++) {
    l.push_back(args_info.inputs[i]);
  }
  if (l.size() < 1) {
    std::cerr << "Error, you should give at least one --input option or one image filename on the commande line." << std::endl;
    exit(0);
  }  

  // Create filter
  clitk::ImageConvertGenericFilter::Pointer filter = clitk::ImageConvertGenericFilter::New();
  filter->SetInputFilenames(l);
  filter->SetIOVerbose(args_info.verbose_flag);
  filter->SetOutputFilename(args_info.output_arg);
  if (args_info.type_given) filter->SetOutputPixelType(args_info.type_arg);

  // Go !
  filter->Update();  

  // this is the end my friend  
  return 0;
}
//-------------------------------------------------------------------=

#endif /* end #define CLITKIMAGECONVERT_CXX */

