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

// clitk include
#include "clitkIO.h"
#include "clitkAffineRegistration_ggo.h"
#include "clitkAffineRegistrationGenericFilter.h"

int main( int argc, char *argv[] )
{
  //init command line and check options
  GGO(clitkAffineRegistration, args_info);
  CLITK_INIT;

  //===========================================================================
  //Set all the options passed through the commandline
  typedef clitk::AffineRegistrationGenericFilter FilterType;
  FilterType::Pointer genericFilter = FilterType::New();
  genericFilter->SetArgsInfo(args_info);
  genericFilter->Update();

  return 1;
}
