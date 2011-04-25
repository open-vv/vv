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
#ifndef CLITKMERGESEQUENCE_CXX
#define CLITKMERGESEQUENCE_CXX

/**
 * @file   clitkMergeSequence.cxx
 * @author Jef Vandemeulebroucke <jefvdmb@gmail.com>
 * @date   December 2  10:14:53 2008
 * 
 * @brief  Read in one VF (ex mhd, vf) invert it using a splat with linear kernels to the target. 
 * 
 */

// clitk include
#include "clitkMergeSequence_ggo.h"
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkCommon.h"
#include "clitkMergeSequenceGenericFilter.h"

int main( int argc, char *argv[] )
{
  
  // Init command line
  GGO(clitkMergeSequence, args_info);
  CLITK_INIT;
  
  //Creation of the generic filter
  clitk::MergeSequenceGenericFilter::Pointer MergeSequenceGenericFilter= clitk::MergeSequenceGenericFilter::New();

  //Pass the parameters
  std::vector<std::string> names;
  for(unsigned int i=0; i<args_info.inputs_num;i++)names.push_back(args_info.inputs[i]);
  MergeSequenceGenericFilter->SetInputs(names);
  MergeSequenceGenericFilter->SetSpacing(args_info.spacing_arg);
  MergeSequenceGenericFilter->SetOutput(args_info.output_arg);
  MergeSequenceGenericFilter->SetVerbose(args_info.verbose_flag);

  //update
  MergeSequenceGenericFilter->Update();  
  return EXIT_SUCCESS;
}
#endif


