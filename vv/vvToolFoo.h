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
#ifndef VVTOOLFOO_H
#define VVTOOLFOO_H
#include <QtDesigner/QDesignerExportWidget>
#include <QDialog>

#include "vvToolBase.h"
#include "ui_vvToolFoo.h"

//------------------------------------------------------------------------------
class vvToolFoo:
  public QDialog,
  public vvToolBase<vvToolFoo>, 
  private Ui::vvToolFoo
{
  Q_OBJECT
    public:
  vvToolFoo(vvMainWindowBase* parent=0, Qt::WindowFlags f=0);
  ~vvToolFoo();

  static void Initialize();

 protected:
  Ui::vvToolFoo ui;

}; // end class vvToolFoo
//------------------------------------------------------------------------------

#endif

