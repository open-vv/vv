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
#include "vvRegisterForm.h"
#include <QUrl>
#include <QSettings>
#include "gtest/gtest.h"
#include "vvConfiguration.h"
#include <stdio.h>
#include <fstream>
const std::string data_path = "/home/mpech/workspace/vvSource/tests/data/";
TEST(vvRegisterForm, canPush){
  QUrl url;
  //the file exists with some old lines
  vvRegisterForm* v=new vvRegisterForm(url, QString::fromStdString(data_path+".settingsCanPush.txt"), QSettings::NativeFormat);
  std::ifstream inFile((data_path+".settingsCanPush.txt").c_str(), std::ifstream::in);
  ASSERT_FALSE(inFile.fail());
  EXPECT_TRUE(v->canPush());
  
  //the file doesn't exist
  vvRegisterForm* v2=new vvRegisterForm(url, QString::fromStdString(data_path+".settings_notFound.txt"), QSettings::NativeFormat);
  EXPECT_TRUE(v2->canPush());
  
  //the current version is the same
  QString strSettings=QString::fromStdString(data_path+".settings2.txt");
  QSettings settings(strSettings, QSettings::NativeFormat);
  settings.setValue("vvVersion", VV_VERSION);
  vvRegisterForm* v3=new vvRegisterForm(url, strSettings, QSettings::NativeFormat);
  EXPECT_FALSE(v3->canPush());
}
TEST(vvRegisterForm, acquitPushed){
  QUrl url;
  vvRegisterForm* v=new vvRegisterForm(url, QString::fromStdString(data_path+".settingsAcquit.txt"), QSettings::NativeFormat);
  ASSERT_TRUE(v->canPush());
  v->acquitPushed();
  EXPECT_FALSE(v->canPush());
  remove((data_path+".settingsAcquit.txt").c_str());
}
