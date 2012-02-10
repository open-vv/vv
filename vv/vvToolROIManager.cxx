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

// vv
#include "vvToolROIManager.h"
#include "vvImageReader.h"
#include "vvROIActor.h"
#include "vvSlicer.h"
#include "vvROIActor.h"

// Qt
#include <QFileDialog>
#include <QMessageBox>
#include <QColorDialog>
#include <QAbstractEventDispatcher>
 
// vtk
#include <vtkLookupTable.h>
#include <vtkRenderWindow.h>

//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolROIManager);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vvToolROIManager::vvToolROIManager(vvMainWindowBase * parent, Qt::WindowFlags f):
  // vvToolWidgetBase(parent, f), 
  // if Qt::Widget -> No dialog in this case (in tab) ; PB = "invisible widget on menu" !
  // if "f" normal widget
  QWidget(parent->GetTab()->widget(4)), 
  vvToolBase<vvToolROIManager>(parent),
  Ui::vvToolROIManager()
{
  //  Insert the current QWidget into the tab layout (required)
  QWidget * mother = qFindChild<QWidget*>(parent->GetTab(), "ROItab");
  mother->layout()->addWidget(this);
  
  // Build the UI
  Ui_vvToolROIManager::setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);

  // Select the current image as the target
  int i = parent->GetSlicerManagerCurrentIndex();
  InputIsSelected(parent->GetSlicerManagers()[i]);

  // Connect event from mainwindow to this widget
  connect(parent, SIGNAL(AnImageIsBeingClosed(vvSlicerManager *)), 
          this, SLOT(AnImageIsBeingClosed(vvSlicerManager *)));
  connect(parent, SIGNAL(SelectedImageHasChanged(vvSlicerManager *)), 
          this, SLOT(SelectedImageHasChanged(vvSlicerManager *)));

  // mMainWindowBase->GetTab()->setTabIcon(mTabNumber, GetToolIcon());
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolROIManager::~vvToolROIManager()
{
  std::cout << "vvToolROIManager::~vvToolROIManager()" << std::endl;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// STATIC
void vvToolROIManager::Initialize() {
  SetToolName("ROIManager");
  SetToolMenuName("Display ROI (binary image)");
  SetToolIconFilename(":/common/icons/tool-roi.png");
  SetToolTip("Display ROI from a binary image.");
  SetToolExperimental(true);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::InputIsSelected(vvSlicerManager *m)
{
  std::cout << "vvToolROIManager::InputIsSelected()" << std::endl;
  mSlicerManager = m;

  // Signal/slot
  connect(mCloseButton, SIGNAL(clicked()), this, SLOT(close()));

  // Initialization

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::AnImageIsBeingClosed(vvSlicerManager * m)
{
  if (m == mSlicerManager) close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::SelectedImageHasChanged(vvSlicerManager * m) {
  if (m != mSlicerManager) hide();
  else show();
}
//------------------------------------------------------------------------------
