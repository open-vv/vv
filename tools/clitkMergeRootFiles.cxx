/**
   =================================================
   * @file   clitkMergeRootFiles.cxx
   * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
   * @author Brent Huisman <brent.huisman@insa-lyon.fr>
   * @date   06 May 2014
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
#include <TApplication.h>

using std::endl;
using std::cout;

struct PetInputFile {
    string filename;
    double mean_time;
};

bool sort_pet_input_file(const PetInputFile &a, const PetInputFile &b) {
    return a.mean_time < b.mean_time;
};

typedef std::vector<PetInputFile> PetInputFiles;

//-----------------------------------------------------------------------------
int main(int argc, char *argv[]) {
	//this fixes a bug in TFileMerger. See http://root.cern.ch/phpBB3/viewtopic.php?t=18016.
    TApplication app("app", 0, 0);

    gROOT->GetPluginManager()->AddHandler("TVirtualStreamerInfo", "*",
                                          "TStreamerInfo", "RIO", "TStreamerInfo()");

    // init command line
    GGO(clitkMergeRootFiles, args_info);

    // Check parameters
    if (args_info.input_given < 2) {
        FATAL("Error, please provide at least two inputs files");
    }

    // Set the tree maximum size to 1TB instead of 100GB
    TTree::SetMaxTreeSize( 1000000000000LL );

    /* The following block does some bookkeeping necesary for files originating from a pet simulation.
    Seems fixing some timing info, for coincidences between files perhaps.
    It seems the files are later on reopened and merged, if the conditions were met.
    It's not required for merging other .root files.
    GateMergeManager reportedly exists specifically for the purpose of merging pet simulations. */
    {
        // Detect Pet output
        bool all_pet_output = true;
        PetInputFiles pet_input_files;
        for (uint i = 0; i < args_info.input_given; i++) {
            const char *filename = args_info.input_arg[i];
            PetInputFile input_file;
            input_file.filename = filename;
            TFile *handle = TFile::Open(filename, "READ");
            assert(handle);
            TTree *hits = dynamic_cast<TTree *>(handle->Get("Hits"));
            TTree *singles = dynamic_cast<TTree *>(handle->Get("Singles"));
            const bool is_pet_output = (hits != NULL) && (singles != NULL);
            cout << "testing " << filename << " is_pet_output " << is_pet_output;

	        //TTree *histos = dynamic_cast<TH1F *>(handle->Get("histo;1"));
	        //const bool is_hist_output = (histos != NULL);


            if (is_pet_output) {
                double time;
                double time_accum = 0;
                singles->SetBranchAddress("time", &time);
                size_t total = singles->GetEntries();
                for (size_t kk = 0; kk < total; kk++) {
                    singles->GetEntry(kk);
                    time_accum += time;
                }

                input_file.mean_time = time_accum / total;
                pet_input_files.push_back(input_file);
                cout << " mean_time " << input_file.mean_time;
            }

            cout << endl;

            handle->Close();
            delete handle;
            //bitwise on booleans?
            all_pet_output &= is_pet_output;
        }
        cout << "all_pet_output " << all_pet_output << endl;

        if (all_pet_output) {
            GateMergeManager manager(args_info.fastmerge_given, args_info.verbose_arg, true, 0, "");

            cout << "sorting input file using singles time" << endl;
            std::sort(pet_input_files.begin(), pet_input_files.end(), sort_pet_input_file);

            Strings input_filenames;
            for (PetInputFiles::const_iterator iter = pet_input_files.begin(); iter != pet_input_files.end(); iter++)
                input_filenames.push_back(iter->filename);

            manager.StartMergingFromFilenames(input_filenames, args_info.output_arg);
            //if the file was PET, then we're done.
            return 0;
        }
    }

    //if the file was not PET, but a generic Rootfile, use TFileMerger.

    // Merge
    TFileMerger *merger = new TFileMerger;
    for (uint i = 0; i < args_info.input_given; i++) merger->AddFile(args_info.input_arg[i]);
    merger->OutputFile(args_info.output_arg);
    bool whatbool = merger->Merge();

    cout << "whatbool: " << whatbool << " no more whatbool" << endl;
    // this is the end my friend
    return 0;
    
}
//-----------------------------------------------------------------------------
