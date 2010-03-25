
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
