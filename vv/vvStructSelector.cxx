/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/
#include "vvStructSelector.h"
#include <iostream>
#include <sstream>

void vvStructSelector::SetStructures(StructureType structures)
{
  mStructures=structures;
  for (StructureType::iterator i=structures.begin(); i!=structures.end(); i++) {
    std::ostringstream ss;
    ss << (*i).first << ":" << (*i).second;
    this->structSelectionWidget->addItem(ss.str().c_str());
  }
}

std::vector<int> vvStructSelector::getSelectedItems()
{
  std::vector<int> result;
  for (int i=0; i<structSelectionWidget->count(); i++) {
    if (structSelectionWidget->item(i)->isSelected())
      result.push_back(mStructures[i].first);
  }
  return result;
}


vvStructSelector::vvStructSelector()
{
  setupUi(this);
}
