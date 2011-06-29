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
static inline void compare(std::string mhd, std::string refMhd, std::string refRaw){
  std::ostringstream cmd_line;
  
  //Ensure the description file exists
  ASSERT_TRUE(itksys::SystemTools::FileExists(mhd.c_str(), true));
  
  //Ensure the description reference file exists
  ASSERT_TRUE(itksys::SystemTools::FileExists(refMhd.c_str(), true));
  
  //Ensure our reference raw exists
  ASSERT_TRUE(itksys::SystemTools::FileExists(refRaw.c_str(), true));
//todo : test this on windows  
  //Generate the new mhd and the new raw
  cmd_line << TOOLS_PATH<<"clitkAffineTransform -i " << mhd << " -o "<<"out.mhd >/dev/null";
  system(cmd_line.str().c_str());
  
  //Check the new file has been created
  EXPECT_TRUE(itksys::SystemTools::FileExists("out.mhd", true));
  
  //Check the mhd and the produced mhd are still equal
  EXPECT_FALSE( itksys::SystemTools::FilesDiffer("out.mhd", refMhd.c_str())  );
  
  //Check the raw and the produced raw are different
  EXPECT_TRUE( itksys::SystemTools::FilesDiffer("out.raw", refRaw.c_str())  );
  
  //clean the produced junk
  remove("out.mhd");
  remove("out.raw");
}
TEST(identity, clitkAffineTransformTest){
  std::string mhd1=std::string(CLITK_DATA_PATH)+"Deformation4D.mhd";
  std::string refMhd1=std::string(CLITK_DATA_PATH)+"Deformation4DRef.mhd";
  std::string refRaw1=std::string(CLITK_DATA_PATH)+"Deformation4DRef.raw";
  compare(mhd1, refMhd1, refRaw1);
}