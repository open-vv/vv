#ifndef vvProgressDialog_h
#define vvProgressDialog_h
#include "ui_vvProgressDialog.h"
#include <string>

class vvProgressDialog : public QDialog, private Ui::vvProgressDialog
{
    Q_OBJECT

public:
    vvProgressDialog(std::string message,bool show_progress=false) {
        setupUi(this);
        textLabel->setText(message.c_str());
        if (show_progress) 
            progressBar->show();
        else 
            progressBar->hide();
        this->show();
    }
    void Update(std::string message)
    {
        textLabel->setText(message.c_str());
    }
    void SetProgress(unsigned int current,unsigned int max)
    {
        progressBar->setMaximum(max);
        progressBar->setValue(current);
    }
    ~vvProgressDialog() {}

public slots:

};

#endif
