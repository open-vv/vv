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
#include "clitkRelativePositionAnalyzer_ggo.h"
#include "clitkRelativePositionAnalyzerGenericFilter.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkRelativePositionAnalyzer, args_info);
  CLITK_INIT;

  // Filter
  typedef clitk::RelativePositionAnalyzerGenericFilter<args_info_clitkRelativePositionAnalyzer> FilterType;
  FilterType::Pointer filter = FilterType::New();

  // Set filename from the AFDB if needed
#define SetOptionFromAFDBMacro(ARGS, OPTIONNAME, OPTION)                \
  if (ARGS.OPTIONNAME##_given) {                                        \
    if (ARGS.OPTION##_given) {                                          \
      std::cerr << "Warning --"#OPTION" is ignored" << std::endl;       \
    }                                                                   \
    std::string f = afdb->GetTagValue(ARGS.OPTIONNAME##_arg);           \
    f = std::string(args_info.afdb_path_arg)+"/"+f;                     \
    filter->AddInputFilename(f);                                        \
  }                                                                     \
  else if (!ARGS.OPTION##_given) {                                      \
    std::cerr << "Error on the command line please provide --"#OPTION" or --"#OPTIONNAME"." << std::endl; \
    return EXIT_FAILURE;                                                \
  }

  // Set options
  filter->SetArgsInfo(args_info);
  
  NewAFDB(afdb, args_info.afdb_arg);
  // The order is important. If --supportName and --support are
  // given, the first is set before, so the second ignored
  SetOptionFromAFDBMacro(args_info, supportName, support);
  SetOptionFromAFDBMacro(args_info, objectName, object);
  SetOptionFromAFDBMacro(args_info, targetName, target);

  // Go !
  try {
    filter->Update();
  } catch(std::runtime_error e) {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
} // This is the end, my friend
//--------------------------------------------------------------------
