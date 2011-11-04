#ifndef VVSAVESTATE_H
#define VVSAVESTATE_H

#include <string>
#include <memory>

class vvMainWindow;
class QXmlStreamWriter;
class QFile;
class QTreeWidgetItem;

class vvSaveState
{
public:
  vvSaveState();
  virtual ~vvSaveState();
  
  virtual void Run(vvMainWindow* vvWindow, const std::string& file);
    
protected:
  
  void SaveGlobals();
  void SaveGUI();
  void SaveTree();
  void SaveImage(QTreeWidgetItem* item, int index);
  void SaveFusion(QTreeWidgetItem* item);
  void SaveOverlay(QTreeWidgetItem* item);
  void SaveVector(QTreeWidgetItem* item);

  std::auto_ptr<QXmlStreamWriter> m_XmlWriter;
  std::auto_ptr<QFile> m_File;
  vvMainWindow* m_Window;
};

#endif // VVSAVESTATE_H
