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

// Set parameters for VV (AETITLE, port-scu) to allow c-move.
void setDicomClient()
{
	QSettings settings(getVVSettingsPath(), getSettingsOptionFormat());
	if (! settings.childGroups().contains("DICOMCLIENT"))
	{
		 settings.beginGroup(QString::fromStdString("DICOMCLIENT"));
		 settings.setValue("AETITLE",QString::fromStdString("VVDICOMSCU"));
		 settings.setValue("port",QString::number(1234));
		 settings.setValue("directory",QDir::homePath() +QString::fromStdString("/.move"));
		 settings.endGroup();
	}
}

// get VV-AETITLE  for c-move. parameters
std::string getDicomClientAETitle()
{	
	 std::string result ="";
	QSettings settings(getVVSettingsPath(), getSettingsOptionFormat());
	 settings.beginGroup(QString::fromStdString("DICOMCLIENT"));
	 result = settings.value("AETITLE").toString().toStdString();
	 settings.endGroup();
	 return result;
}

// get the directory where the dicom files will be stored during c-move action.
std::string getCMoveDirectory()
{
	 std::string result ="";
	QSettings settings(getVVSettingsPath(), getSettingsOptionFormat());
	 settings.beginGroup(QString::fromStdString("DICOMCLIENT"));
	 result = settings.value("directory").toString().toStdString();
	 settings.endGroup();
	 return result;
}

// get VV-PORT-SCU  for c-move. parameters
int getDicomClientPort()
{	
	int result;
	QSettings settings(getVVSettingsPath(), getSettingsOptionFormat());
	 settings.beginGroup(QString::fromStdString("DICOMCLIENT"));
	 result = settings.value("port").toString().toInt();
	 settings.endGroup();
	 return result;
}

// get interpolation option for vv
int getInterpolationFavoriteStatus()
{
	int result;
	QSettings settings(getVVSettingsPath(), getSettingsOptionFormat());
	 settings.beginGroup(QString::fromStdString("VVSETTINGS"));
	 result = settings.value("interpolation").toString().toInt();
	 settings.endGroup();
	 return result;
}

// set VV-settings at first use
void setVVSettings()
{
	QSettings settings(getVVSettingsPath(), getSettingsOptionFormat());
	if (! settings.childGroups().contains("VVSETTINGS"))
	{
		 settings.beginGroup(QString::fromStdString("VVSETTINGS"));
		 settings.setValue("interpolation",QString::number(1));
		 settings.endGroup();
	}
}

/// Add a new Dicom Server
void AddDicomServer(std::string nickname, std::string aetitle, std::string adress, std::string port)
{
	QSettings settings(getVVSettingsPath(), getSettingsOptionFormat());
	 settings.beginGroup(QString::fromStdString("DICOMSERVER"));
	 settings.beginWriteArray(QString::fromStdString(nickname));
	 settings.setValue("nickname",QString::fromStdString(nickname));
	 settings.setValue("AETITLE",QString::fromStdString(aetitle));
	 settings.setValue("ADRESS",QString::fromStdString(adress));
	 settings.setValue("PORT",QString::fromStdString(port));
	 settings.endGroup();
}

/// Remove a Dicom Server
void removeDicomServer(std::string nickname)
{
	QSettings settings(getVVSettingsPath(), getSettingsOptionFormat());
	 settings.beginGroup(QString::fromStdString("DICOMSERVER"));
	 std::string temp = nickname + "//";
	 settings.remove(QString::fromStdString(temp + "nickname"));
	 settings.remove(QString::fromStdString(temp + "AETITLE"));
	  settings.remove(QString::fromStdString(temp + "ADRESS"));
	   settings.remove(QString::fromStdString(temp + "PORT"));
	 settings.endGroup();
}




/// get the list of a Dicom Server
QStringList getDicomServers()
{

	QStringList list;
	QSettings settings(getVVSettingsPath(), getSettingsOptionFormat());
	 settings.beginGroup(QString::fromStdString("DICOMSERVER"));
	 QStringList  keys = settings.allKeys();
	 for(int i =0; i < keys.size(); i++)
	 {
		 QString val = keys.at(i);
		 if (val.contains("nickname"))
			 list.push_back(settings.value(val).toString());
	 }
	 return list;
}

/// get needed Infos for a Dicom Server
std::map <std::string,std::string > getDicomServer(QString nickname)
{
		std::map< std::string, std::string  >results;
		QSettings settings(getVVSettingsPath(), getSettingsOptionFormat());
	 settings.beginGroup(QString::fromStdString("DICOMSERVER"));
	 settings.beginReadArray(nickname);
	   QStringList keys = settings.childKeys();
	   for (int i = 0; i <keys.size(); i++)
		   results[keys.at(i).toStdString()] = settings.value(keys.at(i)).toString().toStdString();
	   return results;

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

