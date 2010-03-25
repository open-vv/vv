/**
   ------------------------------------------------=
   * @file   clitkGuerreroVentilation.cxx
   * @author Joël Schaerer
   * @date   20 April 2009
   ------------------------------------------------=*/

// clitk include
#include "clitkGuerreroVentilation_ggo.h"
#include "clitkGuerreroVentilationGenericFilter.h"
#include "clitkIO.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkGuerreroVentilation, args_info);
  CLITK_INIT;

  // Read image dimension
  itk::ImageIOBase::Pointer header = clitk::readImageHeader(args_info.input_arg);
  //unsigned int dim = header->GetNumberOfDimensions();
  
  // Check parameters
  // Main filter
  clitk::GuerreroVentilationGenericFilter filter;
  filter.SetInputFilename(args_info.input_arg);
  filter.AddInputFilename(args_info.ref_arg);
  filter.SetOutputFilename(args_info.output_arg);
  filter.SetBloodCorrectionFactor(args_info.factor_arg);
  filter.SetUseCorrectFormula(args_info.correct_flag);
  filter.Update();

  // this is the end my friend  
  return 0;
} // end main
