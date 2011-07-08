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
  cmd_line << CLITK_TEST_TOOLS_PATH<<"clitkImageInfo " << mhd << " > clitkImageInfoTest.out";
  system(cmd_line.str().c_str());
  
  EXPECT_FALSE( itksys::SystemTools::FilesDiffer("clitkImageInfoTest.out", ref.c_str()) );
  remove("clitkImageInfoTest.out");
}
TEST(clitkImageInfoTest, main){
  std::string mhd1=std::string(CLITK_TEST_DATA_PATH)+"Deformation4D.mhd";
  ASSERT_TRUE(itksys::SystemTools::FileExists(mhd1.c_str(), true));
  
  std::string mhd2=std::string(CLITK_TEST_DATA_PATH)+"Lung3D.mhd";
  ASSERT_TRUE(itksys::SystemTools::FileExists(mhd2.c_str(), true));
  
  mhd1+=" "+mhd2;
  std::string ref1=std::string(CLITK_TEST_DATA_PATH)+"clitkImageInfoTestRef.out";
  ASSERT_TRUE(itksys::SystemTools::FileExists(ref1.c_str(), true));
  compare(mhd1, ref1);
}