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
#include <sstream>
#include <algorithm>
#include <QDir>
#include "clitkCommon.h"
#include "vvUtils.h"

const std::string vv_user_file=".vv_settings.txt";
const std::string recentFileList="recentFiles";
typedef std::list<std::string> FileListType;

QString getVVSettingsPath(){
  return QDir::homePath()+QString::fromStdString("/"+vv_user_file);
}

QSettings::Format getSettingsOptionFormat(){
  return QSettings::IniFormat;
}

///Returns the last images opened by the user
FileListType GetRecentlyOpenedImages()
{
  QSettings settings(getVVSettingsPath(), getSettingsOptionFormat());
  FileListType result;
  settings.beginGroup(QString::fromStdString(recentFileList));
    QStringList keys = settings.childKeys();
    for(int i=0; i<keys.size(); i++){
      std::string value=settings.value(QString::fromStdString (keys[i].toStdString())).toString().toStdString();
      result.push_back(value);
    }
  settings.endGroup();
  return result;
}

///Adds an image to the list of recently opened images
void AddToRecentlyOpenedImages(std::string filename)
{
  QSettings settings(getVVSettingsPath(), getSettingsOptionFormat());
  FileListType file_list = GetRecentlyOpenedImages();
  
  FileListType::iterator i = std::find(file_list.begin(),file_list.end(),filename);
  if (i != file_list.end()) // avoid dupes
    file_list.erase(i);
  while (file_list.size() >= 6) //keep list to a reasonable size
    file_list.pop_back();
  file_list.push_front(filename);
  
  settings.beginGroup(QString::fromStdString(recentFileList));
    int index=0;
    for (FileListType::iterator j = file_list.begin() ; j != file_list.end() ; j++){
      QString s=QString(index++);
      settings.setValue(s, QString::fromStdString ( *j ));
    }
  settings.endGroup();
}

