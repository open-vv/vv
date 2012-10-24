#include "vvSaveState.h"
#include "vvMainWindow.h"
#include "vvToolCreatorBase.h"
#include "vvToolBaseBase.h"

#include <QDir>
#include <QFile>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QXmlStreamWriter>

#include <cassert>
#include <string>

//------------------------------------------------------------------------------
vvSaveState::vvSaveState() : m_XmlWriter(new QXmlStreamWriter), m_File(new QFile)
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vvSaveState::~vvSaveState()
{
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSaveState::Run(vvMainWindow* vvWindow, const std::string& file)
{
  assert(vvWindow);

  m_File->setFileName(file.c_str());
  m_File->remove();
  m_File->open(QIODevice::Text | QIODevice::ReadWrite);
  
  m_Window = vvWindow;

  m_XmlWriter->setDevice(m_File.get());
  m_XmlWriter->setAutoFormatting(true);
  m_XmlWriter->setAutoFormattingIndent(2);
  m_XmlWriter->writeStartDocument();
  m_XmlWriter->writeStartElement("VVState");
  SaveGlobals();
  SaveGUI();
  SaveTree();
  SaveTools();
  m_XmlWriter->writeEndDocument();
  m_XmlWriter->writeEndElement();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSaveState::SaveGlobals()
{
  m_XmlWriter->writeStartElement("Globals");
  m_XmlWriter->writeEndElement();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSaveState::SaveTree()
{
  QTreeWidget* tree = m_Window->GetTree();
  QTreeWidgetItem* item;
  
  m_XmlWriter->writeStartElement("Images");
  for (int i = 0; i < tree->topLevelItemCount(); i++) {
    item = tree->topLevelItem(i);
    SaveImage(item, i);
  }
  m_XmlWriter->writeEndElement();

  m_XmlWriter->writeStartElement("Links");
  for (int i = 0; i < tree->topLevelItemCount(); i++) {
    const vvSlicerManager * slicerManager = m_Window->GetSlicerManagers()[i];
    SaveLink(slicerManager);
  }
  m_XmlWriter->writeEndElement();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSaveState::SaveImage(const QTreeWidgetItem* item, int index)
{
  const vvSlicerManager * slicerManager = m_Window->GetSlicerManagers()[index];
  m_XmlWriter->writeStartElement("Image");

  std::ostringstream valueStr;
  valueStr.str("");
  valueStr << index;
  m_XmlWriter->writeAttribute("Index", valueStr.str().c_str());

  std::string filename = item->data(0, Qt::UserRole).toString().toStdString();
  m_XmlWriter->writeTextElement("FileName", QDir::current().absoluteFilePath(filename.c_str()));
  int preset = slicerManager->GetPreset();
  m_XmlWriter->writeTextElement("Preset", QString::number(preset));
  if (preset == 6) {
    m_XmlWriter->writeTextElement("Window", QString::number(slicerManager->GetColorWindow()));
    m_XmlWriter->writeTextElement("Level", QString::number(slicerManager->GetColorLevel()));
  }
  
  QTreeWidgetItem* item_child;
  std::string role;
  for (int i = 0; i < item->childCount(); i++) {
    item_child = item->child(i);
    role = item_child->data(1,Qt::UserRole).toString().toStdString();
    if (role == "fusion")
      SaveFusion(item_child, slicerManager);
    else if (role == "overlay")
      SaveOverlay(item_child, slicerManager);
    else if (role == "vector")
      SaveVector(item_child);
  }

  // End
  m_XmlWriter->writeEndElement();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSaveState::SaveFusion(const QTreeWidgetItem* item, const vvSlicerManager* vvManager)
{
  m_XmlWriter->writeStartElement("Fusion");
  std::string filename = item->data(0, Qt::UserRole).toString().toStdString();
  m_XmlWriter->writeTextElement("FileName", QDir::current().absoluteFilePath(filename.c_str()));
  m_XmlWriter->writeTextElement("FusionOpacity", QString::number(vvManager->GetFusionOpacity()));
  m_XmlWriter->writeTextElement("FusionThresholdOpacity", QString::number(vvManager->GetFusionThresholdOpacity()));
  m_XmlWriter->writeTextElement("FusionColorMap", QString::number(vvManager->GetFusionColorMap()));
  m_XmlWriter->writeTextElement("FusionWindow", QString::number(vvManager->GetFusionWindow()));
  m_XmlWriter->writeTextElement("FusionLevel", QString::number(vvManager->GetFusionLevel()));
  m_XmlWriter->writeEndElement();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSaveState::SaveOverlay(const QTreeWidgetItem* item, const vvSlicerManager* vvManager)
{
  m_XmlWriter->writeStartElement("Overlay");
  std::string filename = item->data(0, Qt::UserRole).toString().toStdString();
  m_XmlWriter->writeTextElement("FileName", QDir::current().absoluteFilePath(filename.c_str()));
  m_XmlWriter->writeTextElement("OverlayColorWindow", QString::number(vvManager->GetOverlayColorWindow()));
  m_XmlWriter->writeTextElement("OverlayColorLevel", QString::number(vvManager->GetOverlayColorLevel()));
  m_XmlWriter->writeTextElement("LinkOverlayWindowLevel", QString::number(vvManager->GetLinkOverlayWindowLevel()));
  m_XmlWriter->writeTextElement("OverlayColor", QString::number(vvManager->GetOverlayColor()));
  m_XmlWriter->writeEndElement();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSaveState::SaveVector(const QTreeWidgetItem* item)
{
  m_XmlWriter->writeStartElement("Vector");
  std::string filename = item->data(0, Qt::UserRole).toString().toStdString();
  m_XmlWriter->writeTextElement("FileName", QDir::current().absoluteFilePath(filename.c_str()));
  m_XmlWriter->writeEndElement();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvSaveState::SaveLink(const vvSlicerManager* vvManager)
{
  typedef std::list<std::string> LinkListType;
  LinkListType links = vvManager->GetLinks();
  if (!links.empty()) {
    std::string my_id = vvManager->GetId();
    m_XmlWriter->writeStartElement("LinkedFrom");
    m_XmlWriter->writeAttribute("Id", my_id.c_str());
    LinkListType::iterator i;
    for (i = links.begin(); i != links.end(); i++) {
      std::string link_id = *i;
      m_XmlWriter->writeTextElement("LinkedTo", link_id.c_str());
    }
    m_XmlWriter->writeEndElement();
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvSaveState::SaveGUI()
{
  m_XmlWriter->writeStartElement("GUI");
  m_XmlWriter->writeEndElement();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSaveState::SaveTools()
{

  m_XmlWriter->writeStartElement("Tools");
  // Check if a tool is open and need to be store
  std::vector<vvToolCreatorBase *> & v = vvToolManager::GetInstance()->GetListOfTools();
  for(uint i=0; i<v.size(); i++) {
    std::vector<vvToolBaseBase*> & t = v[i]->GetListOfTool();
    for(uint j=0; j<t.size(); j++) {
      std::string name = v[i]->mToolName.toStdString();
      m_XmlWriter->writeStartElement(name.c_str());
      m_XmlWriter->writeTextElement("ToolType_Index", QString::number(i));
      m_XmlWriter->writeTextElement("Tool_Index", QString::number(j));
      t[j]->SaveState(m_XmlWriter);
      m_XmlWriter->writeEndElement();
    }
  }
  m_XmlWriter->writeEndElement();
}
//------------------------------------------------------------------------------
