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
  SetToolIconFilename(":/common/icons/ducky.png");
  SetToolTip("Make 'foo' on an image.");
}
//------------------------------------------------------------------------------



