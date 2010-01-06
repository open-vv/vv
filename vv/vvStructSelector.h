/*=========================================================================

 Program:   vv
 Language:  C++
 Author :   Joel Schaerer (joel.schaerer@insa-lyon.fr)
 Program:   vv

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
#ifndef vvStructSelector_h
#define vvStructSelector_h

#include "ui_vvStructSelector.h"

class vvStructSelector : public QDialog, private Ui::vvStructSelector
{
    Q_OBJECT

public:
    vvStructSelector();
    typedef std::vector<std::pair<int, std::string> > StructureType;
    ///Sets the different structures and displays them
    void SetStructures(StructureType structures);
    ///Returns the indexes of selected ROIs
    std::vector<int> getSelectedItems();
    ///Enables the propagation checkbox
    void EnablePropagationCheckBox() {this->propagateCheckBox->setEnabled(true);
        this->propagateCheckBox->setChecked(true);}
    ///Returns true if contours should be propagated over the sequence using the vf
    bool PropagationEnabled() {return this->propagateCheckBox->isChecked();}

protected:
    StructureType mStructures;

};

#endif
