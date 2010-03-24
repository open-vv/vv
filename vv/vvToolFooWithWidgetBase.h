/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolFooWithWidgetBase.h,v $
  Language:  C++
  Date:      $Date: 2010/03/24 10:48:18 $
  Version:   $Revision: 1.2 $
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

#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "ui_vvToolFoo.h"

//------------------------------------------------------------------------------
class vvToolFooWithWidgetBase:
  public vvToolWidgetBase,
  public vvToolBase<vvToolFooWithWidgetBase>, 
  private Ui::vvToolFoo
{
  Q_OBJECT
    public:
  vvToolFooWithWidgetBase(vvMainWindowBase* parent=0, Qt::WindowFlags f=0);
  ~vvToolFooWithWidgetBase();

  static void Initialize();
  virtual void InputIsSelected(vvSlicerManager *m);
  //  OR ===> virtual void InputIsSelected(std::vector<vvSlicerManager *> & m);

public slots:
  virtual void apply();

 protected:
  Ui::vvToolFoo ui;

}; // end class vvToolFooWithWidgetBase
//------------------------------------------------------------------------------

#endif

