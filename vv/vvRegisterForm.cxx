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
#include <QDir>
#include <QPalette>
#include "common/globals.h"
#include "vvCommon.h"
#include "vvUtils.h"

vvRegisterForm::vvRegisterForm(QUrl url, QString path, QSettings::Format format):url(url), settings(path, format){ 
  manager = new QNetworkAccessManager(this);
  setupUi(this);
  textBrowser->viewport()->setAutoFillBackground(false);
}

void vvRegisterForm::sendData(){
  QUrl url2(url);
  url2.addQueryItem("name", firstName->text().toUtf8());
  url2.addQueryItem("lastName", lastName->text().toUtf8());
  url2.addQueryItem("email", email->text().toUtf8());
  url2.addQueryItem("group", group->text().toUtf8());
  url2.addQueryItem("os", osName->text().toUtf8());
  url2.addQueryItem("vvVersion", VV_VERSION);
  url2.addQueryItem("architecture", ARCHITECTURE);
  manager->get(QNetworkRequest(url2));
}
void vvRegisterForm::accept(){
  sendData();
  settings.setValue("name", firstName->text().toUtf8());
  settings.setValue("lastName", lastName->text().toUtf8());
  settings.setValue("email", email->text().toUtf8());
  settings.setValue("group", group->text().toUtf8());
  settings.setValue("os", osName->text().toUtf8());
  QDialog::accept();
}
bool vvRegisterForm::canPush(){
  ///maybe we show this dialog only for new major release, not for any patches?
  return settings.value("vvVersion").toString().toStdString()<VV_VERSION;
}
void vvRegisterForm::acquitPushed(){
  settings.setValue("vvVersion", VV_VERSION);
}
void vvRegisterForm::show(){
 firstName->setText(settings.value("name").toString());
 lastName->setText(settings.value("lastName").toString()); 
 email->setText(settings.value("email").toString()); 
 group->setText(settings.value("group").toString()); 
  
 osName->setText(QString::fromStdString(OS_NAME));
 QDialog::show();
}