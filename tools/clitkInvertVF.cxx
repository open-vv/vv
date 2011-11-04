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
int main(int argc, char * argv[])
{

  // Init command line
  GGO(clitkInvertVF, args_info);
  CLITK_INIT;

  // Filter
  typedef clitk::InvertVFGenericFilter<args_info_clitkInvertVF> FilterType;
  FilterType::Pointer genericFilter = FilterType::New();

  genericFilter->SetArgsInfo(args_info);
  CLITK_TRY_CATCH_EXIT(genericFilter->Update());

  return EXIT_SUCCESS;
}// end main

//--------------------------------------------------------------------
