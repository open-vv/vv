#ifndef VVSAVESTATE_H
#define VVSAVESTATE_H

#include <string>
#include <memory>
#include "vvSlicerManager.h"

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
  void SaveTools();
  void SaveImage(const QTreeWidgetItem* item, int index);
  void SaveFusion(const QTreeWidgetItem* item, const vvSlicerManager* slicerManager);
  void SaveOverlay(const QTreeWidgetItem* item, const vvSlicerManager* slicerManager);
  void SaveVector(const QTreeWidgetItem* item);
  void SaveLink(const vvSlicerManager* slicerManager);

#if __cplusplus > 199711L
  std::shared_ptr<QXmlStreamWriter> m_XmlWriter;
  std::shared_ptr<QFile> m_File;
#else
  std::auto_ptr<QXmlStreamWriter> m_XmlWriter;
  std::auto_ptr<QFile> m_File;
#endif
  vvMainWindow* m_Window;
};

#endif // VVSAVESTATE_H
