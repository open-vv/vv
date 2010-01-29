/*------------------------------------------------------------------------
                                                                                 
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
                                                                             
  ------------------------------------------------------------------------*/

/* =================================================
 * @file   clitkBinarizeImageGenericFilter.txx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   29 June 2009
 * 
 * @brief Binarize an image
 * 
 ===================================================*/

// clitk
#include "clitkBinarizeImage_ggo.h"
#include "clitkBinarizeImageGenericFilter.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkBinarizeImage, args_info);
  CLITK_INIT;

  // Filter
  clitk::BinarizeImageGenericFilter<args_info_clitkBinarizeImage>::Pointer filter = 
    clitk::BinarizeImageGenericFilter<args_info_clitkBinarizeImage>::New();
  
  filter->SetArgsInfo(args_info);
  filter->Update();

  return EXIT_SUCCESS;
}// end main

//--------------------------------------------------------------------
