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
===========================================================================**/
#include <QApplication>

#include "vvRegisterForm.h"
#include <QNetworkRequest>
#include <QSettings>
#include <QDir>
#include <iostream>
#include "common/globals.h"
#include "vvCommon.h"
#include "vvUtils.h"

vvRegisterForm::vvRegisterForm(QUrl url):url(url) { 
  manager = new QNetworkAccessManager(this);
  setupUi(this);
}

void vvRegisterForm::sendData(){
  QUrl url2(url);
  url2.addQueryItem("name", firstName->text().toUtf8());
  url2.addQueryItem("lastName", lastName->text().toUtf8());
  url2.addQueryItem("email", email->text().toUtf8());
  url2.addQueryItem("group", group->text().toUtf8());
  url2.addQueryItem("os", QString::fromUtf8(OS_NAME));
  url2.addQueryItem("vvVersion", QString::fromUtf8(VV_VERSION));
  manager->get(QNetworkRequest(url2));
}
void vvRegisterForm::accept(){
  sendData();
  QDialog::accept();
}
bool vvRegisterForm::canPush(){
  QSettings settings(getVVSettingsPath(), getSettingsOptionFormat());
  return settings.value("vvVersion").toString().toStdString()<VV_VERSION;
}
void vvRegisterForm::acquitPushed(){
  QSettings settings(getVVSettingsPath(), getSettingsOptionFormat());
  settings.setValue("vvVersion", VV_VERSION);
}