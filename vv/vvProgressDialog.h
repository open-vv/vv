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
