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
#include <TROOT.h>
#include <TPluginManager.h>
#include <TFile.h>
#include <TKey.h>
#include <TFileMerger.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <iostream>
#include <set>

using std::endl;
using std::cout;

typedef std::list<std::string> Strings;
typedef std::list<TFile*> Handles;
typedef std::set<std::string> StringsSet;

void mergePetRoot(const Strings& input_filenames, const std::string& output_filename)
{
	TFile* output_handle = TFile::Open(output_filename.c_str(),"RECREATE");

	Handles input_handles;
	StringsSet tree_names;
	StringsSet h1_names;
	StringsSet h2_names;
	for (Strings::const_iterator iter=input_filenames.begin(); iter!=input_filenames.end(); iter++)
	{
		TFile* handle = TFile::Open(iter->c_str(),"READ");
		assert(handle);

		TIter key_next(handle->GetListOfKeys());
		while (TKey* key = static_cast<TKey*>(key_next()))
		{
			const std::string name = key->GetName();
			const bool is_tree = key->ReadObj()->InheritsFrom("TTree");
			const bool is_h1 = key->ReadObj()->InheritsFrom("TH1");
			const bool is_h2 = key->ReadObj()->InheritsFrom("TH2");
			if (is_tree) tree_names.insert(name);
			if (is_h1) h1_names.insert(name);
			if (is_h2) h2_names.insert(name);
		}

		input_handles.push_back(handle);
	}

	cout << "found " << tree_names.size() << " tree "<< h1_names.size() << " histo1d " << h2_names.size() << " histo2d" << endl;


	for (Handles::const_iterator iter=input_handles.begin(); iter!=input_handles.end(); iter++)
	{
		(*iter)->Close();
		delete *iter;
	}
}


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
		  mergePetRoot(input_filenames,args_info.output_arg);
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
