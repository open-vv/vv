/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolFoo.h,v $
  Language:  C++
  Date:      $Date: 2010/02/24 11:43:37 $
  Version:   $Revision: 1.1 $
  Author :   David Sarrut (david.sarrut@creatis.insa-lyon.fr)

  Copyright (C) 2010
  Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
  CREATIS                   http://www.creatis.insa-lyon.fr

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

