#ifndef VVREADSTATE_H
#define VVREADSTATE_H

#include <string>
#include <memory>

class vvMainWindow;
class QXmlStreamReader;
class QFile;

class vvReadState
{
public:
  vvReadState();
  virtual ~vvReadState();
  
  virtual void Run(vvMainWindow* vvWindow, const std::string& file);
  int GetNumberOfImages() { return m_NumImages; }
    
protected:
  
  void ReadGUI();
  void ReadTree();
  void ReadTools();
  void ReadTool(const std::string & toolname);
  std::string ReadImage();
  std::string ReadFusion(int index);
  std::string ReadOverlay(int index);
  std::string ReadVector(int index);
  std::string ReadLink();

#if __cplusplus > 199711L
  std::shared_ptr<QXmlStreamReader> m_XmlReader;
  std::shared_ptr<QFile> m_File;
#else
  std::auto_ptr<QXmlStreamReader> m_XmlReader;
  std::auto_ptr<QFile> m_File;
#endif
  vvMainWindow* m_Window;
  int m_TreeItemCount;
  int m_NumImages;
};

#endif // VVREADSTATE_H
