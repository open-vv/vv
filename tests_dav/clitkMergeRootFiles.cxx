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
#include "GateMergeManager.hh"
#include <string> 
#include <TROOT.h>
#include <TPluginManager.h>
#include <TFile.h>
#include <TKey.h>
#include <TFileMerger.h>
#include <TTree.h>
#include <TChain.h>
#include <TH1.h>
#include <TH2.h>
#include <iostream>

using std::endl;
using std::cout;

typedef std::list<std::string> Strings;


//-----------------------------------------------------------------------------
int main(int argc, char * argv[]) {

  gROOT->GetPluginManager()->AddHandler("TVirtualStreamerInfo", "*",
      "TStreamerInfo", "RIO", "TStreamerInfo()");

  // init command line
  GGO(clitkMergeRootFiles, args_info);

  // Check parameters
  if (args_info.input_given < 2) {
    FATAL("Error, please provide at least two inputs files");
  }

  { // Detect Pet output
	  bool all_pet_output = true;
	  Strings input_filenames;
	  for (uint i=0; i<args_info.input_given; i++) 
	  {
		  const char* filename = args_info.input_arg[i];
		  input_filenames.push_back(filename);
		  TFile* handle = TFile::Open(filename,"READ");
		  assert(handle);
		  TTree* hits = dynamic_cast<TTree*>(handle->Get("Hits"));
		  TTree* singles = dynamic_cast<TTree*>(handle->Get("Singles"));
		  const bool is_pet_output = (hits!=NULL) && (singles!=NULL);
		  cout << "testing " << filename << " is_pet_output " << is_pet_output << endl;
		  handle->Close();
		  delete handle;
		  all_pet_output &= is_pet_output;
	  }
	  cout << "all_pet_output " << all_pet_output << endl;

	  if (all_pet_output)
	  {
		  GateMergeManager* manager = new GateMergeManager(args_info.fastmerge_given,args_info.verbose_arg,true,0,".");
		  delete manager;
		  return 0;
	  }
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
