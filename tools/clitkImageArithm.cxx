#ifndef CLITKIMAGEARITHM_CXX
#define CLITKIMAGEARITHM_CXX
/**
   -------------------------------------------------
   * @file   clitkImageArithm.cxx
   * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
   * @date   23 Feb 2008 08:37:53
   -------------------------------------------------*/

// clitk include
#include "clitkImageArithm_ggo.h"
#include "clitkImageArithmGenericFilter.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkImageArithm, args_info);

  // Creation of a generic filter
  typedef clitk::ImageArithmGenericFilter<args_info_clitkImageArithm> FilterType;
  FilterType::Pointer filter = FilterType::New();

  // Go !
  filter->SetArgsInfo(args_info);
  filter->Update();
  
  // this is the end my friend  
  return EXIT_SUCCESS;
} // end main

#endif //define CLITKIMAGEARITHM_CXX
