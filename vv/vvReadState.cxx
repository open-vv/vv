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

#include "vvReadState.h"
#include "vvMainWindow.h"
#include "vvSlicerManager.h"
#include "vvToolCreatorBase.h"
#include "vvToolBaseBase.h"

#include <qtreewidget.h>

#include <QFile>
#include <QXmlStreamReader>

#include <cassert>
#include <string>

//------------------------------------------------------------------------------
vvReadState::vvReadState() : m_XmlReader(new QXmlStreamReader), m_File(new QFile)
{
  m_NumImages = 0;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvReadState::~vvReadState()
{
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvReadState::Run(vvMainWindow* vvWindow, const std::string& file)
{
  assert(vvWindow);

  m_NumImages = 0;
  m_File->setFileName(file.c_str());
  m_File->open(QIODevice::ReadOnly);
  m_XmlReader->setDevice(m_File.get());
  m_Window = vvWindow;
  QTreeWidget* tree = m_Window->GetTree();

  // Get the number of images already loaded
  m_TreeItemCount = tree->topLevelItemCount();

  // Read elements
  ReadGUI();
  ReadTree();
  ReadTools();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvReadState::ReadTree()
{
  std::string value;
  
  while (!m_XmlReader->isEndElement() || value != "Images") { 
    m_XmlReader->readNext();
    value = m_XmlReader->qualifiedName().toString().toStdString();
    if (m_XmlReader->isStartElement()) {
      if (value == "Image") value = ReadImage();
    } 
  }
  
  if (m_XmlReader->hasError())
    std::cout << "Error " << m_XmlReader->error() << " XML " << std::endl;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
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
  
  if (!m_XmlReader->hasError())
    m_NumImages++;

  return value;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
std::string vvReadState::ReadFusion(int index)
{
  std::string file, value;
  int vali;
  double vald;
  vvSlicerManager* slicerManager = m_Window->GetSlicerManagers()[index];
  while (!m_XmlReader->isEndElement() || value != "Fusion") {
    m_XmlReader->readNext();
    value = m_XmlReader->qualifiedName().toString().toStdString();
    if (m_XmlReader->isStartElement()) {
      if (value == "FileName") {
        file = m_XmlReader->readElementText().toStdString();
        if (!m_XmlReader->hasError())
          m_Window->AddFusionImage(index, file.c_str());
      }
      if (value == "FusionOpacity") {
        vali = m_XmlReader->readElementText().toInt();
        if (!m_XmlReader->hasError())
          slicerManager->SetFusionOpacity(vali);
      }
      if (value == "FusionThresholdOpacity") {
        vali = m_XmlReader->readElementText().toInt();
        if (!m_XmlReader->hasError())
          slicerManager->SetFusionThresholdOpacity(vali);
      }
      if (value == "FusionColorMap") {
        vali = m_XmlReader->readElementText().toInt();
        if (!m_XmlReader->hasError())
          slicerManager->SetFusionColorMap(vali);
      }
      if (value == "FusionWindow") {
        vald = m_XmlReader->readElementText().toDouble();
        if (!m_XmlReader->hasError())
          slicerManager->SetFusionWindow(vald);
      }
      if (value == "FusionLevel") {
        vald = m_XmlReader->readElementText().toDouble();
        if (!m_XmlReader->hasError())
          slicerManager->SetFusionLevel(vald);
      }
    }
  }
  m_Window->ImageInfoChanged();
  return value;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
std::string vvReadState::ReadOverlay(int index)
{
  std::string file, value;
  int vali;
  double vald;
  vvSlicerManager* slicerManager = m_Window->GetSlicerManagers()[index];
  while (!m_XmlReader->isEndElement() || value != "Overlay") {
    m_XmlReader->readNext();
    value = m_XmlReader->qualifiedName().toString().toStdString();
    if (m_XmlReader->isStartElement()) {
      if (value == "FileName") {
        file = m_XmlReader->readElementText().toStdString();
        if (!m_XmlReader->hasError()) {
          // TODO: manage sequence of images
          std::vector<std::string> vec;
          vec.push_back(file.c_str());
          m_Window->AddOverlayImage(index, vec, vvImageReader::IMAGE);
        }
      }
      if (value == "OverlayColorWindow") {
        vald = m_XmlReader->readElementText().toDouble();
        if (!m_XmlReader->hasError())
          slicerManager->SetOverlayColorWindow(vald);
      }
      if (value == "OverlayColorLevel") {
        vald = m_XmlReader->readElementText().toDouble();
        if (!m_XmlReader->hasError())
          slicerManager->SetOverlayColorLevel(vald);
      }
      if (value == "LinkOverlayWindowLevel") {
        vali = m_XmlReader->readElementText().toInt();
        if (!m_XmlReader->hasError())
          slicerManager->SetLinkOverlayWindowLevel(vali);
      }
      if (value == "OverlayColor") {
        vali = m_XmlReader->readElementText().toInt();
        if (!m_XmlReader->hasError())
          slicerManager->SetOverlayColor(vali);
      }
    }
  }
  m_Window->ImageInfoChanged();
  return value;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvReadState::ReadGUI()
{

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvReadState::ReadTools()
{
  std::string value;
  
  while ((!m_XmlReader->hasError()) && (!m_XmlReader->isEndElement() || value != "Tools")) { 
    m_XmlReader->readNext();
    value = m_XmlReader->qualifiedName().toString().toStdString();
    if (value != "Tools") {
      if (m_XmlReader->isStartElement()) {
        ReadTool(value);
      } 
    }
  }
  
  if (m_XmlReader->hasError())
    std::cout << "Error " << m_XmlReader->error() << " XML " << std::endl;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvReadState::ReadTool(const std::string & toolname)
{
  // Find name into vvToolManager::GetInstance()->GetListOfTools();
  vvToolCreatorBase * v = vvToolManager::GetInstance()->GetToolCreatorFromName(toolname.c_str());
  if (v == NULL) {
    std::cerr << "Error, I do not know the tool named '" << toolname << "' ; ignored." << std::endl;
    std::string value="";
    while (!m_XmlReader->isEndElement() || value != toolname) { 
      m_XmlReader->readNext();
      value = m_XmlReader->qualifiedName().toString().toStdString();
      if (m_XmlReader->hasError()) {
        std::cout << "Error " << m_XmlReader->error() << " XML " << std::endl;
        return;
      }
    }
    return;
  }

  // CreateTool
  // std::vector<vvToolBaseBase*> & tools = v->GetListOfTool();
  v->m_XmlReader = m_XmlReader;
  v->mReadStateFlag = true;
  v->mImageIndex = m_TreeItemCount;
  v->MenuSpecificToolSlot();
  v->mReadStateFlag = false;
  m_XmlReader = v->m_XmlReader; // Need because auto_ptr operator= release on the right.
}
//------------------------------------------------------------------------------
