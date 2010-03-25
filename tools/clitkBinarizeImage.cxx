
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
  typedef clitk::BinarizeImageGenericFilter<args_info_clitkBinarizeImage> FilterType;
  FilterType::Pointer filter = FilterType::New();
  
  filter->SetArgsInfo(args_info);
  filter->Update();

  return EXIT_SUCCESS;
}// end main

//--------------------------------------------------------------------
