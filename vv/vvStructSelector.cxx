/*=========================================================================

 Program:   vv
 Language:  C++
 Author :   Joel Schaerer (joel.schaerer@insa-lyon.fr)

Copyright (C) 2008
Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
CREATIS-LRMN http://www.creatis.insa-lyon.fr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================*/

#include "vvStructSelector.h"
#include <iostream>
#include <sstream>

void vvStructSelector::SetStructures(StructureType structures)
{
    mStructures=structures;
    for (StructureType::iterator i=structures.begin();i!=structures.end();i++)
    {
        std::ostringstream ss;
        ss << (*i).first << ":" << (*i).second;
        this->structSelectionWidget->addItem(ss.str().c_str());
    }
}

std::vector<int> vvStructSelector::getSelectedItems()
{
    std::vector<int> result;
    for (int i=0;i<structSelectionWidget->count();i++)
    {
        if (structSelectionWidget->item(i)->isSelected())
            result.push_back(mStructures[i].first);
    }
    return result;
}


vvStructSelector::vvStructSelector()
{
    setupUi(this);
}
