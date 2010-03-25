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
