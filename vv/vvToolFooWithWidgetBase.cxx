/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolFooWithWidgetBase.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/01 08:37:19 $
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

#include "vvToolFooWithWidgetBase.h"

//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolFooWithWidgetBase);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vvToolFooWithWidgetBase::vvToolFooWithWidgetBase(vvMainWindowBase * parent, Qt::WindowFlags f)
  :vvToolWidgetBase(parent, f), 
   vvToolBase<vvToolFooWithWidgetBase>(parent), 
   Ui::vvToolFoo() {
  // Setup the UI
  Ui_vvToolFoo::setupUi(mToolWidget);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolFooWithWidgetBase::~vvToolFooWithWidgetBase() {
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolFooWithWidgetBase::Initialize() {
  SetToolName("Foo");
  SetToolMenuName("Foo with WidgetBase");
  SetToolIconFilename(":/new/prefix1/icons/ducky.png");
  SetToolTip("Make 'foo' on an image.");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolFooWithWidgetBase::InputIsSelected(vvSlicerManager *m) {
  mButton->setText(m->GetFileName().c_str());
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolFooWithWidgetBase::apply() {
  close();
}
//------------------------------------------------------------------------------
