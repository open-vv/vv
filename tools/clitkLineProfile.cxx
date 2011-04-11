#include "clitkLineProfile_ggo.h"
#include "clitkLineProfileGenericFilter.h"


bool verbose = false;

template <unsigned int Dimension>
void run(const args_info_clitkLineProfile& argsInfo);

int main(int argc, char** argv)
{
  GGO(clitkLineProfile, args_info);

  clitk::LineProfileGenericFilter::Pointer filter = clitk::LineProfileGenericFilter::New();
  filter->SetArgsInfo(args_info);
  filter->Update();
  
  return EXIT_SUCCESS;
}


