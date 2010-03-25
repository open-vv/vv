#ifndef VVTOOLCREATOR_H
#define VVTOOLCREATOR_H
#include "vvToolCreatorBase.h"

//------------------------------------------------------------------------------
template<class ToolType>
class vvToolCreator: public vvToolCreatorBase {
public:
  vvToolCreator(QString name):vvToolCreatorBase(name) {;}
  virtual void InsertToolInMenu(vvMainWindowBase * m);
  virtual void MenuSpecificToolSlot() { CreateTool<ToolType>(); }
  static vvToolCreator<ToolType> * mSingleton;
};
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#define CREATOR(CLASSNAME) vvToolCreator<CLASSNAME>::mSingleton
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#define ADD_TOOL(NAME)                                          \
  template<>                                                    \
  vvToolCreator<NAME> * vvToolCreator<NAME>::mSingleton =       \
    new vvToolCreator<NAME>(#NAME);
//------------------------------------------------------------------------------

#include "vvToolCreator.txx"

#endif

