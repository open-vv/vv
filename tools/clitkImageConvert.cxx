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
int main(int argc, char * argv[])
{

  // init command line
  GGO(clitkImageConvert, args_info);
  CLITK_INIT;

  // Get list of filenames
  std::vector<std::string> l;
  unsigned int nCmdLineInputs = args_info.inputs_num;
  std::string output;
  if(args_info.output_given)
    output = args_info.output_arg;
  else if(nCmdLineInputs--)
    output = args_info.inputs[nCmdLineInputs];
  else {
    std::cerr << "Error, you should give the --output option or one image filename on the commande line." << std::endl;
    exit(0);
  }

  if(args_info.input_given) {
    l.push_back(args_info.input_arg);
  }
  if(nCmdLineInputs==1) {
    l.push_back(args_info.inputs[0]);
  }
  if (l.size() < 1) {
    std::cerr << "Error, you should give the --input option or one image filename on the commande line." << std::endl;
    exit(0);
  }

  // Create filter
  clitk::ImageConvertGenericFilter::Pointer filter = clitk::ImageConvertGenericFilter::New();
  filter->SetInputFilenames(l);
  filter->SetIOVerbose(args_info.verbose_flag);
  filter->SetOutputFilename(output);
  filter->EnableWriteCompression(args_info.compression_flag);
  if (args_info.type_given) filter->SetOutputPixelType(args_info.type_arg);

  // Go !
  CLITK_TRY_CATCH_EXIT(filter->Update());

  // this is the end my friend
  return 0;
}
//-------------------------------------------------------------------=

#endif /* end #define CLITKIMAGECONVERT_CXX */

