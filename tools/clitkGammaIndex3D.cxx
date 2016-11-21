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
#ifndef CLITKGAMMAINDEX3D_CXX
#define CLITKGAMMAINDEX3D_CXX

// clitk include
#include "clitkGammaIndex3D_ggo.h"
#include "clitkGammaIndex3DGenericFilter.h"
#include "clitkIO.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[])
{

  // Init command line
  GGO(clitkGammaIndex3D, args_info);
  CLITK_INIT;

  // Creation of a generic filter
  typedef clitk::GammaIndex3DGenericFilter<args_info_clitkGammaIndex3D> FilterType;
  FilterType::Pointer filter = FilterType::New();

  // Go !
  filter->SetArgsInfo(args_info);
  CLITK_TRY_CATCH_EXIT(filter->Update());

  // this is the end my friend
  return EXIT_SUCCESS;
} // end main

#endif //define CLITKGAMMAINDEX3D_CXX
