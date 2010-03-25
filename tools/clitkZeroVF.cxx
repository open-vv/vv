#ifndef clitkZeroVF_cxx
#define clitkZeroVF_cxx

/**
 * @file   clitkZeroVF.cxx
 * @author Joel Schaerer
 * @date   July 20  10:14:53 2007
 * 
 * @brief  Creates a VF filled with zeros the size of the input VF
 * 
 */

// clitk include
#include "clitkZeroVF_ggo.h"
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkZeroVFGenericFilter.h"

int main( int argc, char *argv[] )
{
  
  // Init command line
  GGO(clitkZeroVF, args_info);
  CLITK_INIT;
  
  //Creation of the generic filter
  clitk::ZeroVFGenericFilter::Pointer zeroVFGenericFilter= clitk::ZeroVFGenericFilter::New();

  //Pass the parameters
  zeroVFGenericFilter->SetInput(args_info.input_arg);
  zeroVFGenericFilter->SetOutput(args_info.output_arg);
  zeroVFGenericFilter->SetVerbose(args_info.verbose_flag);

  //update
  zeroVFGenericFilter->Update();  
  return EXIT_SUCCESS;
}
#endif

