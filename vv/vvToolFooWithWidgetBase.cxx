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
  
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolFooWithWidgetBase::apply() {
  close();
}
//------------------------------------------------------------------------------
