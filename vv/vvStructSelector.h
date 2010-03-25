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
