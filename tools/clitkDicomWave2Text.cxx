#include "clitkDicomWave2Text.h"
#include "clitkDicomWave2Text_ggo.h"

#include "clitkCommon.h"
//gdcm include
#include "gdcmFile.h"

#if GDCM_MAJOR_VERSION < 2
  #include "gdcmUtil.h"
  #include "gdcmBinEntry.h"
  #include "gdcmValEntry.h"
  #include "gdcmSeqEntry.h"
  #include "gdcmSQItem.h"
#endif

#include "gdcmSerieHelper.h"

#include <iostream>
#include <fstream>

//==========================================================================================================================

int main(int argc, char * argv[]) {

//-----------------------------------------------------------------------------
// init command line
GGO(clitkDicomWave2Text, args_info);
//-----------------------------------------------------------------------------

#if GDCM_MAJOR_VERSION < 2
//-----------------------------------------------------------------------
// opening dicom input file
gdcm::File * mDCMFile = new gdcm::File();
mDCMFile->SetFileName(args_info.InputFile_arg);
mDCMFile->AddForceLoadElement(0x01e1,0x1018); //Load wave data no matter its size

if (!mDCMFile->OpenFile ()) {
	std::cerr << "Sorry, the file does not exist or does not appear to be a DICOM file. Abort." << std::endl;
    exit(0);
}
mDCMFile->Load();
std::cout << "File:   "<< args_info.InputFile_arg << "   loaded !"<< std::endl;


//-----------------------------------------------------------------------
// read data
gdcm::DocEntrySet* item = mDCMFile;
#define T short

gdcm::BinEntry* entry = item->GetBinEntry(0x01e1,0x1018);
DD(entry);
T* buffer = reinterpret_cast<T*>(entry->GetBinArea());
int length=item->GetEntryLength(0x01e1,0x1018)/sizeof(T);
std::ofstream text_file(args_info.OutputFile_arg, std::ios::out | std::ios::trunc);
if(text_file)
{
    std::string stat="";
    text_file << "COMPLETE_WAVE" << '\t' << "MASK"       << '\t' << "AQUISITION_PROFIL" << '\t' << "END-INHALE" << '\t' << "END-EXHALE" << '\t' << "AQUISITION_WAVE" << '\t' << "WAVE_STATISTICS" << '\t' << "MASK"        << std::endl;
    for (int i=0;i<length-76;i+=2)
    {
	    if ( i < 74 )
	    {
		    switch(i)
		    {
		      case 68 :
			stat="Total points";
			break;
		      case 70 :
			stat="Sampling rate (Hz)";
			break;
		      default :
			stat="";
			break;
		    }
		      
		    if (buffer[i+75] == 0)
			text_file << buffer[i+74]    << '\t' << buffer[i+75] << '\t' << 0                   << '\t' << "  "         << '\t' << "  "         << '\t' << "  "              << '\t' << buffer[i]         << '\t' << buffer[i+1] << '\t' << stat << std::endl;
		    if (buffer[i+75] == 16384)
			text_file << buffer[i+74]    << '\t' << buffer[i+75] << '\t' << 0                   << '\t' << buffer[i+74] << '\t' << "  "         << '\t' << "  "              << '\t' << buffer[i]         << '\t' << buffer[i+1] << '\t' << stat << std::endl;
		    if (buffer[i+75] == 256)
			text_file << buffer[i+74]    << '\t' << buffer[i+75] << '\t' << 0                   << '\t' << "  "         << '\t' << buffer[i+74] << '\t' << "  "              << '\t' << buffer[i]         << '\t' << buffer[i+1] << '\t' << stat << std::endl;
		    if (buffer[i+75] == -32768)
			text_file << buffer[i+74]    << '\t' << buffer[i+75] << '\t' << 1                   << '\t' << "  "         << '\t' << "  "         << '\t' << buffer[i+74]      << '\t' << buffer[i]         << '\t' << buffer[i+1] << '\t' << stat << std::endl;
		    if (buffer[i+75] == -16384)
			text_file << buffer[i+74]    << '\t' << buffer[i+75] << '\t' << 1                   << '\t' << buffer[i+74] << '\t' << "  "         << '\t' << buffer[i+74]      << '\t' << buffer[i]         << '\t' << buffer[i+1] << '\t' << stat << std::endl;
		    if (buffer[i+75] == -32512)
			text_file << buffer[i+74]    << '\t' << buffer[i+75] << '\t' << 1                   << '\t' << "  "         << '\t' << buffer[i+74] << '\t' << buffer[i+74]      << '\t' << buffer[i]         << '\t' << buffer[i+1] << '\t' << stat << std::endl;
	    }
	    else
	    {
		    if (buffer[i+75] == 0)
			text_file << buffer[i+74]    << '\t' << buffer[i+75] << '\t' << 0                   << '\t' << "  "         << '\t' << "  "         << '\t' << "  "              << '\t' << "  "              << '\t' << "  "        << std::endl;
		    if (buffer[i+75] == 16384)
			text_file << buffer[i+74]    << '\t' << buffer[i+75] << '\t' << 0                   << '\t' << buffer[i+74] << '\t' << "  "         << '\t' << "  "              << '\t' << "  "              << '\t' << "  "        << std::endl;
		    if (buffer[i+75] == 256)
			text_file << buffer[i+74]    << '\t' << buffer[i+75] << '\t' << 0                   << '\t' << "  "         << '\t' << buffer[i+74] << '\t' << "  "              << '\t' << "  "              << '\t' << "  "        << std::endl;
		    if (buffer[i+75] == -32768)
			text_file << buffer[i+74]    << '\t' << buffer[i+75] << '\t' << 1                   << '\t' << "  "         << '\t' << "  "         << '\t' << buffer[i+74]      << '\t' << "  "              << '\t' << "  "        << std::endl;
		    if (buffer[i+75] == -16384)
			text_file << buffer[i+74]    << '\t' << buffer[i+75] << '\t' << 1                   << '\t' << buffer[i+74] << '\t' << "  "         << '\t' << buffer[i+74]      << '\t' << "  "              << '\t' << "  "        << std::endl;
		    if (buffer[i+75] == -32512)
			text_file << buffer[i+74]    << '\t' << buffer[i+75] << '\t' << 1                   << '\t' << "  "         << '\t' << buffer[i+74] << '\t' << buffer[i+74]      << '\t' << "  "              << '\t' << "  "        << std::endl;
	    }
    }
    text_file.close();
}
else
    std::cerr << "Error openning "<< args_info.OutputFile_arg << std::endl;
#else
    std::cerr << argv[0] << " is not compatible with GDCM 2.x you should use GDCM 1.x" << std::endl;
    return 1;
#endif
}
