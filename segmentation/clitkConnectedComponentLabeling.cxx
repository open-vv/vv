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
#include "clitkConnectedComponentLabeling_ggo.h"
#include "clitkConnectedComponentLabelingGenericFilter.h"
#include "clitkMemoryUsage.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[])
{
  //  clitk::PrintMemory(true, "start");

  // Init command line
  GGO(clitkConnectedComponentLabeling, args_info);
  CLITK_INIT;

  // Filter
  //clitk::PrintMemory(true, "before filter");
  typedef clitk::ConnectedComponentLabelingGenericFilter<args_info_clitkConnectedComponentLabeling> FilterType;
  FilterType::Pointer filter = FilterType::New();

  filter->SetArgsInfo(args_info);
  //clitk::PrintMemory(true, "before update");
  try {
    filter->Update();
  } catch(std::runtime_error e) {
    std::cout << e.what() << std::endl;
  }

  //clitk::PrintMemory(true, "after filter");

  return EXIT_SUCCESS;
} // This is the end, my friend
//--------------------------------------------------------------------
