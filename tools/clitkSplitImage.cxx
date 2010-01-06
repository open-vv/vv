/*------------------------------------------------------------------------=
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
                                                                             
  ------------------------------------------------------------------------=*/

/**
   ------------------------------------------------=
   * @file   clitkSplitImage.cxx
   * @author Joël Schaerer
   * @date   20 April 2009
   ------------------------------------------------=*/

// clitk include
#include "clitkSplitImage_ggo.h"
#include "clitkSplitImageGenericFilter.h"
#include "clitkIO.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkSplitImage, args_info);
  CLITK_INIT;

  // Read image dimension
  itk::ImageIOBase::Pointer header = clitk::readImageHeader(args_info.input_arg);
  unsigned int dim = header->GetNumberOfDimensions();
  
  // Check parameters
  if (args_info.dimension_given >= dim) {
    std::cerr << "ERROR : image has " << dim << "dimensions, split dimension should be between 0 and " << dim-1 << "." << std::endl;
    exit(-1);
  }
  // Main filter
  clitk::SplitImageGenericFilter filter;
  filter.SetInputFilename(args_info.input_arg);
  filter.SetOutputFilename(args_info.output_arg);
  filter.SetSplitDimension(args_info.dimension_arg);
  filter.SetVerbose(args_info.verbose_flag);
  filter.Update();

  // this is the end my friend  
  return 0;
} // end main
