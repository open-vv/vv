template<class ToolType>
vvToolBase<ToolType>::vvToolBase(vvMainWindowBase * m) { 
  mMainWindowBase = m; 
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------  
// Default value (to be overwritten)
template<class ToolType>
void vvToolBase<ToolType>::Initialize() {
  SetToolName("Unnamed tool (use SetToolName in Initialize())");
  SetToolMenuName("Unnamed tool");
  SetToolIconFilename("");
  SetToolTip("Unamed tool.");
}
//------------------------------------------------------------------------------    

