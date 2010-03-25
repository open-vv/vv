#ifndef VVTOOLBASE_H
#define VVTOOLBASE_H
#include "vvToolBaseBase.h"
#include "vvToolCreator.h"

//------------------------------------------------------------------------------
template<class ToolType>
class vvToolBase : public vvToolBaseBase {
public:
  vvToolBase(vvMainWindowBase * m);
  static void Initialize();  // can't be virtual, must be overwritten

  static void SetToolName(QString n) { vvToolCreator<ToolType>::mSingleton->mToolName = n; }
  static void SetToolMenuName(QString n) { vvToolCreator<ToolType>::mSingleton->mToolMenuName = n; }
  static void SetToolIconFilename(QString n) { vvToolCreator<ToolType>::mSingleton->mToolIconFilename = n; }
  static void SetToolTip(QString n) { vvToolCreator<ToolType>::mSingleton->mToolTip = n; }
  static void SetToolExperimental(bool exp) { vvToolCreator<ToolType>::mSingleton->mExperimental = exp; }

  void AddImage(vvImage::Pointer image,std::string filename) {
    CREATOR(ToolType)->GetMainWindow()->AddImage(image,filename); 
  }

};
//------------------------------------------------------------------------------

#include "vvToolBase.txx"

#endif

