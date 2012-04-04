/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
===========================================================================**/
#ifndef VVTOOLMANAGER_H
#define VVTOOLMANAGER_H
#include "clitkCommon.h"

class vvToolCreatorBase;
class vvMainWindowBase;
class QString;
class vvToolBaseBase;

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
  
  /// Called in MainWindow, make the tools enabled/disabled
  static void EnableToolsInMenu(vvMainWindowBase * m, bool enable);

  std::vector<vvToolCreatorBase *> & GetListOfTools() { return mListOfTools; }

  vvToolCreatorBase * GetToolCreatorFromName(QString toolTypeName);

protected:

  /// Singleton object pointer
  static vvToolManager * mSingleton;

  /// list of all tool creators
  std::vector<vvToolCreatorBase *> mListOfTools;
};
//------------------------------------------------------------------------------

#endif

