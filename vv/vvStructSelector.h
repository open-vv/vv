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
