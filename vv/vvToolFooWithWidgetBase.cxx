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
