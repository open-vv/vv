template<class ToolType>
void vvToolCreator<ToolType>::InsertToolInMenu(vvMainWindowBase * m) { 
  mMainWindow = m;

  // Default Initialization
  mToolMenuName = mToolName;
  mToolIconFilename = "noicon";
  mToolTip = mToolName;

  // User Tool Initialization
  ToolType::Initialize();

  // Common Initialization (insertion into menu)
  vvToolCreatorBase::InsertToolInMenu(mMainWindow); 
}
//------------------------------------------------------------------------------
