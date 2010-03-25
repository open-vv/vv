#ifndef vvDocumentation_h
#define vvDocumentation_h
#include <iostream>
#include <vector>

#include "ui_vvDocumentation.h"

class vvDocumentation : public QDialog, private Ui::vvDocumentation
{
    Q_OBJECT

public:
    vvDocumentation() {
        setupUi(this);
    }
    ~vvDocumentation() {}

public slots:

private:

};

#endif
