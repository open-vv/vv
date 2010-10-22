/*------------------------------------------------------------------------
                                                                                 
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
                                                                             
  ------------------------------------------------------------------------*/

// clitk
#include "clitkFillMask_ggo.h"
#include "clitkIO.h"
#include "clitkCommon.h"
#include "clitkFillMaskGenericFilter.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkFillMask,args_info);
  CLITK_INIT;

  // Filter
  typedef clitk::FillMaskGenericFilter<args_info_clitkFillMask> FilterType;
  FilterType::Pointer filter = FilterType::New();
  
  filter->SetArgsInfo(args_info);
  
  try {
    filter->Update();
  } catch(std::runtime_error e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
} // This is the end, my friend
//--------------------------------------------------------------------
