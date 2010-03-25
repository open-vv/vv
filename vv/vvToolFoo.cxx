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



