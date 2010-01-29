/*------------------------------------------------------------------------
                                                                                 
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
                                                                             
  ------------------------------------------------------------------------*/

/* =================================================
 * @file   clitkInvertVF.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk
#include "clitkInvertVF_ggo.h"
#include "clitkIO.h"
#include "clitkInvertVFGenericFilter.h"


//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkInvertVF, args_info);
  CLITK_INIT;

  // Filter
  typedef clitk::InvertVFGenericFilter<args_info_clitkInvertVF> FilterType;
  FilterType::Pointer genericFilter = FilterType::New();
  
  genericFilter->SetArgsInfo(args_info);
  genericFilter->Update();

  return EXIT_SUCCESS;
}// end main

//--------------------------------------------------------------------
