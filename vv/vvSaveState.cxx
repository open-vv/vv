#include "vvSaveState.h"
#include "vvMainWindow.h"

#include <QDir>
#include <QFile>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QXmlStreamWriter>

#include <cassert>
#include <string>

vvSaveState::vvSaveState() : m_XmlWriter(new QXmlStreamWriter), m_File(new QFile)
{
}

vvSaveState::~vvSaveState()
{
}

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
  m_XmlWriter->writeEndDocument();
  m_XmlWriter->writeEndElement();
}

void vvSaveState::SaveGlobals()
{
  m_XmlWriter->writeStartElement("Globals");
  m_XmlWriter->writeEndElement();
}

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
}

void vvSaveState::SaveImage(QTreeWidgetItem* item, int index)
{
  m_XmlWriter->writeStartElement("Image");
  
  std::ostringstream indexStr;
  indexStr.str("");
  indexStr << index;
  m_XmlWriter->writeAttribute("Index", indexStr.str().c_str());

  std::string filename = item->data(0, Qt::UserRole).toString().toStdString();
  m_XmlWriter->writeTextElement("FileName", QDir::current().absoluteFilePath(filename.c_str()));
  
  QTreeWidgetItem* item_child;
  std::string role;
  for (int i = 0; i < item->childCount(); i++) {
    item_child = item->child(i);
    role = item_child->data(1,Qt::UserRole).toString().toStdString();
    if (role == "fusion")
      SaveFusion(item_child);
    else if (role == "overlay")
      SaveOverlay(item_child);
    else if (role == "vector")
      SaveVector(item_child);
  }
  
  m_XmlWriter->writeEndElement();
}

void vvSaveState::SaveFusion(QTreeWidgetItem* item)
{
  m_XmlWriter->writeStartElement("Fusion");
  std::string filename = item->data(0, Qt::UserRole).toString().toStdString();
  m_XmlWriter->writeTextElement("FileName", QDir::current().absoluteFilePath(filename.c_str()));
  m_XmlWriter->writeEndElement();
}

void vvSaveState::SaveOverlay(QTreeWidgetItem* item)
{
  m_XmlWriter->writeStartElement("Overlay");
  std::string filename = item->data(0, Qt::UserRole).toString().toStdString();
  m_XmlWriter->writeTextElement("FileName", QDir::current().absoluteFilePath(filename.c_str()));
  m_XmlWriter->writeEndElement();
}

void vvSaveState::SaveVector(QTreeWidgetItem* item)
{
  m_XmlWriter->writeStartElement("Vector");
  std::string filename = item->data(0, Qt::UserRole).toString().toStdString();
  m_XmlWriter->writeTextElement("FileName", QDir::current().absoluteFilePath(filename.c_str()));
  m_XmlWriter->writeEndElement();
}

void vvSaveState::SaveGUI()
{
  m_XmlWriter->writeStartElement("GUI");
  m_XmlWriter->writeEndElement();
}

