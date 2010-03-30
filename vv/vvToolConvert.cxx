/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/

#include "vvToolConvert.h"
#include "vvSlicer.h"
#include "clitkImageConvertGenericFilter.h"
#include <QMenu>
#include <QApplication>
#include <QMessageBox>
#include <vtkImageData.h>

//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolConvert);
//------------------------------------------------------------------------------

QAction * vvToolConvert::a = NULL;
QAction * vvToolConvert::b = NULL;
std::vector<std::string> vvToolConvert::mListOfPixelTypeNames;
std::vector<std::string> vvToolConvert::mListOfPixelTypeIcons;
std::map<std::string, QAction*> vvToolConvert::mMapOfPixelType;

//------------------------------------------------------------------------------
vvToolConvert::vvToolConvert(vvMainWindowBase * parent, Qt::WindowFlags f):
  vvToolBase<vvToolConvert>(parent)
{
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolConvert::~vvToolConvert() {
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolConvert::Initialize() {
  SetToolName("Convert");
  SetToolMenuName("Convert with WidgetBase");
  SetToolIconFilename(":/common/icons/ducky.png");
  SetToolTip("Make 'foo' on an image.");
  
  // Create a menu to choose the convert image
  QMenu * m = new QMenu();
  m->setTitle("Convert to ");
  m->setIcon(QIcon(QString::fromUtf8(":/common/icons/green-arrow.png")));

  mListOfPixelTypeNames.push_back("char");
  mListOfPixelTypeNames.push_back("unsigned_char");
  mListOfPixelTypeNames.push_back("short");
  mListOfPixelTypeNames.push_back("unsigned_short");
  mListOfPixelTypeNames.push_back("int");
  mListOfPixelTypeNames.push_back("float");
  mListOfPixelTypeNames.push_back("double");

  mListOfPixelTypeIcons.push_back(":/common/icons/1b.png");
  mListOfPixelTypeIcons.push_back(":/common/icons/1b.png");
  mListOfPixelTypeIcons.push_back(":/common/icons/2b.png");
  mListOfPixelTypeIcons.push_back(":/common/icons/2b.png");
  mListOfPixelTypeIcons.push_back(":/common/icons/4b.png");
  mListOfPixelTypeIcons.push_back(":/common/icons/4b.png");
  mListOfPixelTypeIcons.push_back(":/common/icons/8b.png");

  for(unsigned int i=0; i<mListOfPixelTypeNames.size(); i++) {
    std::string & s = mListOfPixelTypeNames[i];
    mMapOfPixelType[s] = m->addAction(QIcon(QString::fromUtf8(mListOfPixelTypeIcons[i].c_str())), 
				      tr(s.c_str()));
    m->addAction(mMapOfPixelType[s]);
  }

  CREATOR(vvToolConvert)->addMenuToContextMenu(m);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolConvert::show() {
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  // Get action menu name
  QAction * cc = dynamic_cast<QAction*>(mSender);
  std::string type = cc->text().toStdString();
  // Get current image
  int index = mMainWindowBase->GetSlicerManagerCurrentIndex();
  vvSlicerManager * m = mMainWindowBase->GetSlicerManagers()[index];
  assert(m != NULL); // Should no occur
  
  // Create filter and run !
  clitk::ImageConvertGenericFilter * filter = new clitk::ImageConvertGenericFilter;
  filter->SetInputVVImage(m->GetImage());
  filter->SetOutputPixelType(type);
  filter->EnableDisplayWarning(false);
  filter->Update();
      
  // Manage warning
  if (filter->IsWarningOccur()) {
    QApplication::restoreOverrideCursor();
    QMessageBox::warning(mMainWindowBase, "Warning", filter->GetWarning().c_str());	
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  }
  
  // Output
  vvImage::Pointer output = filter->GetOutputVVImage();
  std::ostringstream osstream;
  osstream << "Convert_" << type << "_" << m->GetSlicer(0)->GetFileName() << ".mhd";
  AddImage(output,osstream.str()); 
  QApplication::restoreOverrideCursor();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolConvert::apply() {
  // nothing !!
}
//------------------------------------------------------------------------------


