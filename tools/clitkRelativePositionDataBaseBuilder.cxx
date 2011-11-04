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
#include "clitkRelativePositionDataBaseBuilder_ggo.h"
#include "clitkRelativePositionDataBaseBuilderGenericFilter.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkRelativePositionDataBaseBuilder, args_info);
  CLITK_INIT;

  // Filter
  typedef clitk::RelativePositionDataBaseBuilderGenericFilter<args_info_clitkRelativePositionDataBaseBuilder> FilterType;
  FilterType::Pointer filter = FilterType::New();
  
  // Set options
  filter->SetArgsInfo(args_info);
  
  // Add an input to determine the type of image
  NewAFDB(afdb, args_info.afdb_arg);
  std::string f = afdb->GetTagValue(args_info.supportName_arg);
  f = std::string(args_info.afdb_path_arg)+"/"+f;
  filter->AddInputFilename(f);
  
  try {
    filter->Update();
  } catch(std::runtime_error e) {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
} // This is the end, my friend
//--------------------------------------------------------------------
