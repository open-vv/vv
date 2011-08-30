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
#ifndef vvXXX_h
#define vvXXX_h
#include <list>
#include <string>
#include <QSettings>

///Returns the last images opened by the user
std::list<std::string> GetRecentlyOpenedImages();

///Adds an image to the list of recently opened images
void AddToRecentlyOpenedImages(std::string filename);

///path to user settings
QString getVVSettingsPath();

///by default nativeFormat
QSettings::Format getSettingsOptionFormat();

#endif
