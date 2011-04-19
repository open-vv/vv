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

#ifndef VVTOOLCONVERT_H
#define VVTOOLCONVERT_H

#include "vvToolBase.h"

//------------------------------------------------------------------------------
class vvToolConvert: public QObject, public vvToolBase<vvToolConvert>
{
  Q_OBJECT
    public:
  vvToolConvert(vvMainWindowBase* parent, Qt::WindowFlags f=0);
  ~vvToolConvert();

  static void Initialize();

public slots:
  virtual void apply();
  void show();
  
 protected:
  static std::vector<std::string> mListOfPixelTypeNames;
  static std::vector<std::string> mListOfPixelTypeIcons;
  static std::map<std::string, QAction*> mMapOfPixelType;
  static QAction * a;
  static QAction * b;

}; // end class vvToolConvert
//------------------------------------------------------------------------------

#endif

