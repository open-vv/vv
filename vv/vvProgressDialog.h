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

#ifndef VVPROGRESSDIALOG_H
#define VVPROGRESSDIALOG_H

// vv
#include "ui_vvProgressDialog.h"

//------------------------------------------------------------------------------
class vvProgressDialog : 
  public QDialog, 
  private Ui::vvProgressDialog
{
  Q_OBJECT

  public:
  vvProgressDialog(std::string message,bool show_progress=false);
  ~vvProgressDialog() {}
  void SetCancelButtonEnabled(bool b);
  void SetText(std::string message);
  void AddToText(std::string message);
  void SetProgress(unsigned int current,unsigned int max);
};
//------------------------------------------------------------------------------

#endif
