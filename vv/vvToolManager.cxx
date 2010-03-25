#include "vvToolManager.h"
#include "vvToolCreatorBase.h"
#include "vvMainWindowBase.h"

//------------------------------------------------------------------------------
/// Unique static instance 
vvToolManager* vvToolManager::mSingleton=0; 
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolManager * vvToolManager::GetInstance() {
  if (mSingleton == 0) {
    mSingleton = new vvToolManager;
  }
  return mSingleton;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolManager::AddTool(vvToolCreatorBase * v)  {
  std::cout << "Adding the tool <" << v->mToolName.toStdString() << ">." << std::endl;
  GetInstance()->mListOfTools.push_back(v);
}  
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolManager::InsertToolsInMenu(vvMainWindowBase * m) {
  for(unsigned int i=0; i<GetInstance()->mListOfTools.size(); i++) {
    GetInstance()->mListOfTools[i]->InsertToolInMenu(m);
  }
}
//------------------------------------------------------------------------------

