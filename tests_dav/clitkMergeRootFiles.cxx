/**
   =================================================
   * @file   clitkMergeRootFiles.cxx
   * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
   * @date   01 Apr 2009
   * 
   * @brief  
   * 
   =================================================*/

#include "clitkMergeRootFiles_ggo.h"
#include "clitkCommon.h"
#include <string> 
#include "TROOT.h"
#include "TFile.h"
#include "TFileMerger.h"

//-----------------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // init command line
  GGO(clitkMergeRootFiles, args_info);

  // Check parameters
  if (args_info.input_given < 2) {
    FATAL("Error, please provide at least two inputs files");
  }

  // Merge
  TFileMerger * merger = new TFileMerger;
  for (uint i=0; i<args_info.input_given; i++) merger->AddFile(args_info.input_arg[i]);
  merger->OutputFile(args_info.output_arg);
  merger->Merge();

  // this is the end my friend  
  return 0;
}
//-----------------------------------------------------------------------------
