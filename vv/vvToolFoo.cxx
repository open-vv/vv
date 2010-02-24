/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolFoo.cxx,v $
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

#include "vvToolFoo.h"

//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolFoo);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vvToolFoo::vvToolFoo(vvMainWindowBase * parent, Qt::WindowFlags f)
  :QDialog(parent), 
   vvToolBase<vvToolFoo>(parent), 
   Ui::vvToolFoo() {
  // Setup the UI
  Ui_vvToolFoo::setupUi(this);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolFoo::~vvToolFoo() {
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolFoo::Initialize() {
  SetToolName("Foo");
  SetToolMenuName("Foo");
  SetToolIconFilename(":/new/prefix1/icons/ducky.png");
  SetToolTip("Make 'foo' on an image.");
}
//------------------------------------------------------------------------------



