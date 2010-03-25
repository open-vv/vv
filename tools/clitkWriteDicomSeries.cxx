
/* =================================================
 * @file   clitkWriteDicomSeries.cxx
 * @author Jef Vandemeulebroucke
 * @date   4th of August
 * 
 * @brief Write a volume into a series with the header of another series
 * 
 ===================================================*/


// clitk
#include "clitkWriteDicomSeries_ggo.h"
#include "clitkIO.h"
#include "clitkWriteDicomSeriesGenericFilter.h"


//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkWriteDicomSeries, args_info);
  CLITK_INIT;

  // Filter
  typedef clitk::WriteDicomSeriesGenericFilter<args_info_clitkWriteDicomSeries> FilterType;
  FilterType::Pointer genericFilter = FilterType::New();
  
  genericFilter->SetArgsInfo(args_info);
  genericFilter->Update();

  return EXIT_SUCCESS;
}// end main

//--------------------------------------------------------------------
