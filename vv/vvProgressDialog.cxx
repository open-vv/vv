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

// vv
#include "vvProgressDialog.h"

//------------------------------------------------------------------------------
vvProgressDialog::vvProgressDialog(std::string message, bool show_progress) 
{
  setupUi(this);
  connect(mCancelButton, SIGNAL(rejected()), this, SLOT(reject()));
  SetCancelButtonEnabled(false);
  textLabel->setText(message.c_str());
  if (show_progress) 
    progressBar->show();
  else 
    progressBar->hide();
  this->show();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvProgressDialog::SetCancelButtonEnabled(bool b)
{
  if (b) {
    mCancelButton->show();
    //setMaximumHeight(500);
    //setMinimumHeight(80);
    //QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    // sizePolicy.setHorizontalStretch(0);
    // sizePolicy.setVerticalStretch(0);
    // sizePolicy.setHeightForWidth(sizePolicy().hasHeightForWidth());
    //setSizePolicy(sizePolicy);
  }
  else {
    mCancelButton->hide();
    //    setFixedHeight(80); // fixe the size when no cancel button
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvProgressDialog::SetText(std::string message)
{
  textLabel->setText(message.c_str());
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvProgressDialog::AddToText(std::string message)
{
  textLabel->setText(QString("%1\n%2").arg(textLabel->text()).arg(message.c_str()));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvProgressDialog::SetProgress(unsigned int current,unsigned int max)
{
  progressBar->setMaximum(max);
  progressBar->setValue(current);
}
//------------------------------------------------------------------------------

