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

// clitk include
#include "clitkComposeVF_ggo.h"
#include "clitkCommon.h"
#include "clitkComposeVFGenericFilter.h"

//--------------------------------------------------------------------
int main( int argc, char *argv[] )
{
  
  // Init command line
  GGO(clitkComposeVF, args_info);
  CLITK_INIT;
  
  //Creation of the generic filter
  clitk::ComposeVFGenericFilter::Pointer ComposeVFGenericFilter= clitk::ComposeVFGenericFilter::New();

  //Pass the parameters
  ComposeVFGenericFilter->SetInput1(args_info.input1_arg);
  ComposeVFGenericFilter->SetInput2(args_info.input2_arg);
  ComposeVFGenericFilter->SetOutput(args_info.output_arg);
  //JV how to pass for different dims?
  //ComposeVFGenericFilter->SetEdgePaddingValue(args_info.pad_arg);
  ComposeVFGenericFilter->SetVerbose(args_info.verbose_flag);
  CLITK_TRY_CATCH_EXIT(ComposeVFGenericFilter->Update());  

  return EXIT_SUCCESS;
}
//--------------------------------------------------------------------


