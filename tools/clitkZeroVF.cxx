/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
===========================================================================**/
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
#include "clitkCommon.h"
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

