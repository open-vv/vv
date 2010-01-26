/*=========================================================================

 Program:   vv
 Language:  C++
 Author :   Pierre Seroul (pierre.seroul@gmail.com)

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
