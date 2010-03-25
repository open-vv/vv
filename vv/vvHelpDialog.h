#ifndef vvHelpDialog_h
#define vvHelpDialog_h
#include <iostream>
#include <vector>

#include "ui_vvHelpDialog.h"

class vvHelpDialog : public QDialog, private Ui::vvHelpDialog
{
    Q_OBJECT

public:
    vvHelpDialog() {
        setupUi(this);
    }
    ~vvHelpDialog() {}

public slots:

private:

};

#endif
