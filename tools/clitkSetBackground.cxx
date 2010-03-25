
/* =================================================
 * @file   clitkSetBackground.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk
#include "clitkSetBackground_ggo.h"
#include "clitkIO.h"
#include "clitkSetBackgroundGenericFilter.h"


//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkSetBackground,args_info);
  CLITK_INIT;

  // Filter
  clitk::SetBackgroundGenericFilter::Pointer genericFilter=clitk::SetBackgroundGenericFilter::New();
  
  genericFilter->SetArgsInfo(args_info);
  genericFilter->Update();

  return EXIT_SUCCESS;
}// end main

//--------------------------------------------------------------------
