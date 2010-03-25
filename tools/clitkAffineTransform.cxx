
/* =================================================
 * @file   clitkAffineTransform.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk
#include "clitkAffineTransform_ggo.h"
#include "clitkIO.h"
#include "clitkAffineTransformGenericFilter.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkAffineTransform, args_info);
  CLITK_INIT;

  // Filter
  typedef clitk::AffineTransformGenericFilter<args_info_clitkAffineTransform> FilterType;
  FilterType::Pointer genericFilter = FilterType::New();
  
  genericFilter->SetArgsInfo(args_info);
  genericFilter->Update();

  return EXIT_SUCCESS;
}// end main

//--------------------------------------------------------------------
