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
#include "gtest/gtest.h"
#include <itksys/SystemTools.hxx>
TEST(clitkWriteDicomSeriesTest, main){
  
  std::string fIn=std::string(CLITK_DATA_PATH)+"/3d/mhd/00.mhd";
  std::string fOut=std::string(CLITK_DATA_PATH)+"/3d/dcm/1.2.840.113704.1.111.536.1248695032.26";
  ASSERT_TRUE(itksys::SystemTools::FileExists(fIn.c_str(), true));
  std::ostringstream cmd_line;
  cmd_line << TOOLS_PATH <<"clitkWriteDicomSeries -i " << fIn << " -d " << fOut << " -o dcm --verbose";
  EXPECT_TRUE(itksys::SystemTools::FileExists(fOut.c_str(), false));
  EXPECT_TRUE(itksys::SystemTools::FileExists("dcm", false));
  EXPECT_EQ(0, system(cmd_line.str().c_str()));
  //does directory exists
  itksys::SystemTools::RemoveADirectory("dcm");
}