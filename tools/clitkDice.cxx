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
#include "clitkIO.h"
#include "clitkDice_ggo.h"

#include "clitkLabelImageOverlapMeasureGenericFilter.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkDice, args_info);
  CLITK_INIT;

  // Filter
  typedef clitk::LabelImageOverlapMeasureGenericFilter<args_info_clitkDice> FilterType;
  FilterType::Pointer filter = FilterType::New();

  filter->SetArgsInfo(args_info);

  try {
    filter->Update();
  } catch(clitk::ExceptionObject e) {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
} // This is the end, my friend
//--------------------------------------------------------------------
