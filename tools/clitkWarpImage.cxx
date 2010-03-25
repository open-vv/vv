
/* =================================================
 * @file   clitkWarpImage.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk
#include "clitkWarpImage_ggo.h"
#include "clitkIO.h"
#include "clitkWarpImageGenericFilter.h"


//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkWarpImage, args_info);
  CLITK_INIT;

  // Filter
  clitk::WarpImageGenericFilter::Pointer genericFilter=clitk::WarpImageGenericFilter::New();
  
  genericFilter->SetArgsInfo(args_info);
  genericFilter->Update();

  return EXIT_SUCCESS;
}// end main

//--------------------------------------------------------------------
