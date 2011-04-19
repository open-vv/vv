#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <sstream>
#include <cassert>

#include <itksys/SystemTools.hxx>

const size_t NUMTESTS=1;

const char mhd_files[NUMTESTS][128] = {
  CLITK_DATA_PATH"/3d/mhd/00.mhd"
};

const char dcm_dirs[NUMTESTS][128] = {
  CLITK_DATA_PATH"/3d/dcm/1.2.840.113704.1.111.536.1248695032.26"
};

int main(int argc, char** argv)
{
  bool failed = false;
  for (size_t i = 0; i < NUMTESTS; i++) {
    std::ostringstream cmd_line;
    cmd_line << "clitkWriteDicomSeries -i " << mhd_files[i] << " -d " << dcm_dirs[i] << " -o dcm --verbose";

    std::cout << "Executing " << cmd_line.str() << std::endl;
    int err = system(cmd_line.str().c_str());
    if (err != 0)
    {
      failed = true;
      std::cout << "FAILED (errno = " << err << ")" << std::endl;
    }
    else
    {
      itksys::SystemTools::RemoveADirectory("dcm");
      std::cout << "PASSED" << std::endl;
    }
  }
  return failed ? -1 : 0;
}