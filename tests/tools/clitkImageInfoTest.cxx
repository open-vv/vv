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
===========================================================================*/
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include "gtest/gtest.h"
#include "stdio.h"
#include <itksys/SystemTools.hxx>
static inline void compare(std::string mhd, std::string ref){
  std::ostringstream cmd_line;
  ASSERT_TRUE(itksys::SystemTools::FileExists(mhd.c_str(), true));
  cmd_line << TOOLS_PATH<<"clitkImageInfo " << mhd << " > clitkImageInfoTest.out";
  std::cout << "Executing " << cmd_line.str() << std::endl;
  system(cmd_line.str().c_str());
  
  bool differ = itksys::SystemTools::FilesDiffer("clitkImageInfoTest.out", ref.c_str());
  remove("clitkImageInfoTest.out");
  EXPECT_FALSE(differ);
}
TEST(clitkImageInfoTest, main){
  std::string mhd1=std::string(CLITK_DATA_PATH)+"4d/mhd/00.mhd";
  std::string ref1=std::string(CLITK_DATA_PATH)+"tools/clitkImageInfoTestValidate3D.out";
  compare(mhd1, ref1);
  
  std::string mhd2=std::string(CLITK_DATA_PATH)+"4d/mhd/bh.mhd";
  std::string ref2=std::string(CLITK_DATA_PATH)+"tools/clitkImageInfoTestValidate4D.out";
  compare(mhd2, ref2);
}