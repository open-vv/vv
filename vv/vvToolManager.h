#ifndef VVTOOLMANAGER_H
#define VVTOOLMANAGER_H
#include "clitkCommon.h"

class vvToolCreatorBase;
class vvMainWindowBase;

//------------------------------------------------------------------------------
// Manage a list of ToolCreator. Each tool will be automagically
// inserted into a list at construction time, before main. Then the
// MainWindow will call the 'Initialize' to insert the tool in the
// menu bar.
class vvToolManager {

public:

  /// Get or build unique instance with this method
  static vvToolManager * GetInstance();  

  /// Add a tool creator in the list (called before main, via static member initialization)
  static void AddTool(vvToolCreatorBase * v);

  /// Called in MainWindow, insert all tools into the menu
  static void InsertToolsInMenu(vvMainWindowBase * m);
  
protected:

  /// Singleton object pointer
  static vvToolManager * mSingleton;

  /// list of all tool creators
  std::vector<vvToolCreatorBase *> mListOfTools;
};
//------------------------------------------------------------------------------

#endif

