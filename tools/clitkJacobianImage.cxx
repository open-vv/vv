#include "clitkJacobianImage_ggo.h"
#include "clitkJacobianImageGenericFilter.h"

int main(int argc, char** argv) {
  // Init command line
  GGO(clitkJacobianImage, args_info);

  // Filter
  typedef clitk::JacobianImageGenericFilter<args_info_clitkJacobianImage> FilterType;
  FilterType::Pointer genericFilter = FilterType::New();

  genericFilter->SetArgsInfo(args_info);
  genericFilter->Update();

  return EXIT_SUCCESS;
}