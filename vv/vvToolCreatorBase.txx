template<class ToolType>
void vvToolCreatorBase::CreateTool() {
  // Get information on the current image
  mMainWindow->UpdateCurrentSlicer();
  // Create the tool
  ToolType * tool = new ToolType(mMainWindow);
  // Put it in the list of open tools
  mListOfTool.push_back(tool);
  // Go !
  tool->show();
}
//------------------------------------------------------------------------------

