/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/
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
int main(int argc, char * argv[])
{

  // Init command line
  GGO(clitkSplitImage, args_info);
  CLITK_INIT;

  // Read image dimension
  itk::ImageIOBase::Pointer header = clitk::readImageHeader(args_info.input_arg);
  if (header.IsNull()) {
    std::cerr << "Unable to read image file " << args_info.input_arg << std::endl;
    std::exit(1);
  }
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
  filter.SetPng(args_info.png_flag);
  filter.SetWindowLevel(args_info.window_arg, args_info.level_arg);
  filter.SetVerbose(args_info.verbose_flag);
  filter.Update();

  // this is the end my friend
  return 0;
} // end main
