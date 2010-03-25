
/* =================================================
 * @file   clitkAverageTemporalDimension.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk
#include "clitkAverageTemporalDimension_ggo.h"
#include "clitkIO.h"
#include "clitkAverageTemporalDimensionGenericFilter.h"


//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkAverageTemporalDimension, args_info);
  CLITK_INIT;

  // Filter
  typedef clitk::AverageTemporalDimensionGenericFilter<args_info_clitkAverageTemporalDimension> FilterType;
  FilterType::Pointer genericFilter = FilterType::New();
  
  genericFilter->SetArgsInfo(args_info);
  genericFilter->Update();

  return EXIT_SUCCESS;
}// end main

//--------------------------------------------------------------------
