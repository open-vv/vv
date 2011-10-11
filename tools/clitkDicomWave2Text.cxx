#include "clitkDicomWave2Text.h"
#include "clitkDicomWave2Text_ggo.h"

#include "clitkCommon.h"

#include "gdcmFile.h"
#if GDCM_MAJOR_VERSION < 2
  #include "gdcmBinEntry.h"
#else
  #include "gdcmReader.h"
  #include "gdcmPrivateTag.h"
#endif

#include <iostream>
#include <fstream>


bool wave2stream( std::ostream &text_file, const short *buffer, int length )
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
  return true;
}


int main(int argc, char * argv[])
{
  GGO(clitkDicomWave2Text, args_info);
  
  std::ofstream os(args_info.OutputFile_arg, std::ios::out | std::ios::trunc);
  
  #if GDCM_MAJOR_VERSION < 2
  
    gdcm::File * mDCMFile = new gdcm::File();
    mDCMFile->SetFileName(args_info.InputFile_arg);
    mDCMFile->AddForceLoadElement(0x01e1,0x1018); //Load wave data no matter its size
    if ( !mDCMFile->OpenFile() )
      {
      std::cerr << "Failed to read: " << args_info.InputFile_arg << std::endl;
      return 1;
      }
    mDCMFile->Load();
    gdcm::DocEntrySet* item = mDCMFile;
    #define T short
    gdcm::BinEntry * entry = item->GetBinEntry(0x01e1,0x1018);
    T* buffer = reinterpret_cast<T*>(entry->GetBinArea());
    int length=item->GetEntryLength(0x01e1,0x1018)/sizeof(T);
    
  #else
  
    gdcm::Reader reader;
    reader.SetFileName(args_info.InputFile_arg);
    if( !reader.Read() )
      {
      std::cerr << "Failed to read: " << args_info.InputFile_arg << std::endl;
      return 1;
      }
    const gdcm::DataSet& ds = reader.GetFile().GetDataSet();
    const gdcm::PrivateTag twave(0x01e1,0x18,"ELSCINT1");
    if( !ds.FindDataElement( twave ) ) return 1;
    const gdcm::DataElement& wave = ds.GetDataElement( twave );
    if ( wave.IsEmpty() ) return 1;
    const gdcm::ByteValue * bv = wave.GetByteValue();
    short * buffer = (short*)bv->GetPointer();
    int length = bv->GetLength() / sizeof( short );
    
  #endif
  
  // Dump that to a CSV file:
  wave2stream( os, buffer, length );
  os.close();
  return 0;
}
