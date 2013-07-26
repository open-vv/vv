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

struct PetInputFile
{
	string filename;
	double mean_time;
};

bool sort_pet_input_file(const PetInputFile& a, const PetInputFile& b)
{
	return a.mean_time<b.mean_time;
};

typedef std::vector<PetInputFile> PetInputFiles;

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
	  PetInputFiles pet_input_files;
	  for (uint i=0; i<args_info.input_given; i++) 
	  {
		  const char* filename = args_info.input_arg[i];
		  PetInputFile input_file;
		  input_file.filename = filename;
		  TFile* handle = TFile::Open(filename,"READ");
		  assert(handle);
		  TTree* hits = dynamic_cast<TTree*>(handle->Get("Hits"));
		  TTree* singles = dynamic_cast<TTree*>(handle->Get("Singles"));
		  const bool is_pet_output = (hits!=NULL) && (singles!=NULL);
		  cout << "testing " << filename << " is_pet_output " << is_pet_output;

		  if (is_pet_output)
		  {
			  double time;
			  double time_accum = 0;
			  singles->SetBranchAddress("time",&time);
			  size_t total = singles->GetEntries();
			  for (size_t kk=0; kk<total; kk++)
			  {
				  singles->GetEntry(kk);
				  time_accum += time;
			  }
				  
			  input_file.mean_time = time_accum/total;
			  pet_input_files.push_back(input_file);
			  cout << " mean_time " << input_file.mean_time;
		  }

		  cout << endl;

		  handle->Close();
		  delete handle;
		  all_pet_output &= is_pet_output;
	  }
	  cout << "all_pet_output " << all_pet_output << endl;

	  if (all_pet_output)
	  {
		  GateMergeManager manager(args_info.fastmerge_given,args_info.verbose_arg,true,0,"");

		  cout << "sorting input file using singles time" << endl;
		  std::sort(pet_input_files.begin(),pet_input_files.end(),sort_pet_input_file);

		  Strings input_filenames;
		  for (PetInputFiles::const_iterator iter=pet_input_files.begin(); iter!=pet_input_files.end(); iter++)
			  input_filenames.push_back(iter->filename);

		  manager.StartMergingFromFilenames(input_filenames,args_info.output_arg);
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
