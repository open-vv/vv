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
#ifndef CLITKIMAGEREGIONFILL_CXX
#define CLITKIMAGEREGIONFILL_CXX
/**
   ------------------------------------------------=
   * @file   clitkImageFillRegion.cxx
   * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
   * @date   23 Feb 2008
   ------------------------------------------------=*/

// clitk include
#include "clitkImageFillRegion_ggo.h"
#include "clitkImageFillRegionGenericFilter.h"
#include "clitkIO.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[])
{

  // Init command line
  GGO(clitkImageFillRegion, args_info);
  CLITK_INIT;

  // Read image dimension
  itk::ImageIOBase::Pointer header = clitk::readImageHeader(args_info.input_arg);
  unsigned int dim = header->GetNumberOfDimensions();

  // Main filter
  clitk::ImageFillRegionGenericFilter filter;
  filter.SetInputFilename(args_info.input_arg);
  filter.SetOutputFilename(args_info.output_arg);
  filter.SetFillPixelValue(args_info.value_arg);

  if(!args_info.ellips_flag && !args_info.rect_flag) {
    std::cerr << "ERROR : No type of region specified!"<< std::endl;
    exit(-1);
  }

  if(args_info.ellips_flag && args_info.rect_flag) {
    std::cerr << "ERROR : Multiple types of regions specified!"<< std::endl;
    exit(-1);
  }

  if(args_info.rect_flag) {
    if (args_info.size_given && args_info.start_given) {
      // Check parameters
      if (args_info.size_given != dim) {
        std::cerr << "ERROR : image has " << dim << "dimensions, --size should have the same number of values." << std::endl;
        exit(-1);
      }
      if (args_info.start_given != dim) {
        std::cerr << "ERROR : image has " << dim << "dimensions, --size should have the same number of values." << std::endl;
        exit(-1);
      }
      filter.SetRegion(args_info.size_arg, args_info.start_arg);
    } else {
      std::cerr << "ERROR : both size and start should be given!"<< std::endl;
      exit(-1);
    }
  }

  if(args_info.ellips_flag) {

    std::vector<double> c, a;
    if (args_info.center_given) {
      if (args_info.center_given != dim) {
        std::cerr << "ERROR : image has " << dim << "dimensions, --center should have the same number of values." << std::endl;
        exit(-1);
      }
      for(unsigned int i=0; i<dim; i++)
        c.push_back(args_info.center_arg[i]);
    }

    if (args_info.axes_given) {
      if (args_info.axes_given != dim) {
        std::cerr << "ERROR : image has " << dim << "dimensions, --axes should have the same number of values." << std::endl;
        exit(-1);
      }
      for(unsigned int i=0; i<dim; i++)
        a.push_back(args_info.axes_arg[i]);

    } else
      for(unsigned int i=0; i<dim; i++)
        a.push_back(10.0);

    if ((args_info.center_given))
      filter.SetSphericRegion(a,c);
    else
      filter.SetSphericRegion(a);
  }

  filter.Update();

  // this is the end my friend
  return 0;
} // end main

#endif //define CLITKIMAGEREGIONFILL
