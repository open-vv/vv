#include "vvReadState.h"
#include "vvMainWindow.h"

#include <qtreewidget.h>

#include <QFile>
#include <QXmlStreamReader>

#include <cassert>
#include <string>

vvReadState::vvReadState() : m_XmlReader(new QXmlStreamReader), m_File(new QFile)
{
}

vvReadState::~vvReadState()
{
}

void vvReadState::Run(vvMainWindow* vvWindow, const std::string& file)
{
  assert(vvWindow);

  m_File->setFileName(file.c_str());
  m_File->open(QIODevice::ReadOnly);
  m_XmlReader->setDevice(m_File.get());
  m_Window = vvWindow;
  QTreeWidget* tree = m_Window->GetTree();
  m_TreeItemCount = tree->topLevelItemCount();

  ReadGUI();
  ReadTree();
}

void vvReadState::ReadTree()
{
  std::string value;
  
  while (!m_XmlReader->atEnd()) {
    m_XmlReader->readNext();
    value = m_XmlReader->qualifiedName().toString().toStdString();
    if (m_XmlReader->isStartElement()) {
      if (value == "Image") 
        value = ReadImage();
    } 
  }
  
  if (m_XmlReader->hasError())
    std::cout << "Error " << m_XmlReader->error() << " XML " << std::endl;
}

std::string  vvReadState::ReadImage()
{
  std::string value;
  int current_index = -1;
  std::vector<std::string> files(1);

  QXmlStreamAttributes attributes = m_XmlReader->attributes();
  if (!m_XmlReader->hasError())
    current_index = attributes.value("Index").toString().toInt();

  current_index += m_TreeItemCount;
  
  while (!m_XmlReader->isEndElement() || value != "Image") { 
    m_XmlReader->readNext();
    value = m_XmlReader->qualifiedName().toString().toStdString();
    //std::cout << "Value = " << value << std::endl;
    if (m_XmlReader->isStartElement()) {
      if (value == "FileName") {
        files[0] = m_XmlReader->readElementText().toStdString();
        if (!m_XmlReader->hasError()) {
          m_Window->LoadImages(files, vvImageReader::IMAGE);
        }
      }
      else if (current_index >= 0) {
        if (value == "Fusion") 
          value = ReadFusion(current_index);
        else if (value == "Overlay")
          value = ReadOverlay(current_index);
        else if (value == "Vector")
          value = ReadVector(current_index);
      }
    }
  }

  return value;
}

std::string vvReadState::ReadFusion(int index)
{
  std::string file, value;
  while (!m_XmlReader->isEndElement() || value != "Fusion") {
    m_XmlReader->readNext();
    value = m_XmlReader->qualifiedName().toString().toStdString();
    if (m_XmlReader->isStartElement()) {
      if (value == "FileName") {
        file = m_XmlReader->readElementText().toStdString();
        if (!m_XmlReader->hasError())
          m_Window->AddFusionImage(index, file.c_str());
      }
    }
  }
  return value;
}

std::string vvReadState::ReadOverlay(int index)
{
  std::string file, value;
  while (!m_XmlReader->isEndElement() || value != "Overlay") {
    m_XmlReader->readNext();
    value = m_XmlReader->qualifiedName().toString().toStdString();
    if (m_XmlReader->isStartElement()) {
      if (value == "FileName") {
        file = m_XmlReader->readElementText().toStdString();
        if (!m_XmlReader->hasError())
          m_Window->AddOverlayImage(index, file.c_str());
      }
    }
  }
  return value;
}

std::string vvReadState::ReadVector(int index)
{
  std::string file, value;
  while (!m_XmlReader->isEndElement() || value != "Vector") {
    m_XmlReader->readNext();
    value = m_XmlReader->qualifiedName().toString().toStdString();
    if (m_XmlReader->isStartElement()) {
      if (value == "FileName") {
        file = m_XmlReader->readElementText().toStdString();
        if (!m_XmlReader->hasError())
          m_Window->AddField(file.c_str(), index);
      }
    }
  }
  return value;
}

void vvReadState::ReadGUI()
{

}

