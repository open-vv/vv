#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <sstream>
#include <cassert>

#include <itksys/SystemTools.hxx>

const size_t NUMTESTS=2;

// test files
const char mhd_files[NUMTESTS][128] = {
  "data/4d/mhd/00.mhd",
  "data/4d/mhd/bh.mhd"
};

// pre-written validation files. the idea
// is that the output generated from the test
// files match the verification files
const char validation_files[NUMTESTS][128] = {
  "data/tools/clitkImageInfoTestValidate3D.out",
  "data/tools/clitkImageInfoTestValidate4D.out"
};

int main(int argc, char** argv)
{
  bool failed = false;
  for (size_t i = 0; i < NUMTESTS; i++) {
    std::ostringstream cmd_line;
    cmd_line << "clitkImageInfo " << mhd_files[i] << " > clitkImageInfoTest.out";

    std::cout << "Executing " << cmd_line.str() << std::endl;
    system(cmd_line.str().c_str());
    
    // compare output with validation file
    std::cout << "Validating output against " << validation_files[i] << std::endl;
    bool differ = itksys::SystemTools::FilesDiffer("clitkImageInfoTest.out", validation_files[i]);
    if (differ)
    {
      failed = true;
      std::cout << "FAILED: Program output and reference do not match." << std::endl;
    }
    else
    {
      itksys::SystemTools::RemoveFile("clitkImageInfoTest.out");
      std::cout << "PASSED" << std::endl;
    }
  }
  return failed ? -1 : 0;
}