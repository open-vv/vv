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
  mMainWindow = parent;
  
  // Build the UI
  Ui_vvToolROIManager::setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
  mTree->clear();
  mTree->header()->resizeSection(0, 30);
  parent->GetTab()->setCurrentIndex(2);

  // Set default LUT
  mDefaultLUTColor = vtkSmartPointer<vtkLookupTable>::New();
  DD(mDefaultLUTColor->GetNumberOfTableValues());
  for(int i=0; i<mDefaultLUTColor->GetNumberOfTableValues(); i++) {
    double r = (rand()/(RAND_MAX+1.0));
    double v = (rand()/(RAND_MAX+1.0));
    double b = (rand()/(RAND_MAX+1.0));
    mDefaultLUTColor->SetTableValue(i, r, v, b);
    //    std::cout << "mDefaultLUTColor->SetTableValue(" << i << ", " << r << ", " << v << ", " << b << ");" << std::endl;
  }
#include "vvDefaultLut.h"

  // Initialization
  mNumberOfVisibleROI = 0;
  mNumberOfVisibleContourROI = 0;

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
  mSlicerManager = m;
  mCurrentImage = mSlicerManager->GetImage();

  // Refuse if 4D
  if (mCurrentImage->GetNumberOfDimensions() != 3) {
    QMessageBox::information(this,tr("Sorry only 3D yet"), tr("Sorry only 3D yet"));
    close();
    return;
  }

  // Auto diusplay browser to select new contours 
  OpenBinaryImage();
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


//------------------------------------------------------------------------------
void vvToolROIManager::OpenBinaryImage() 
{
  // Open images
  QString Extensions = "Images files ( *.mha *.mhd *.hdr *.his)";
  Extensions += ";;All Files (*)";
  QStringList filename =
    QFileDialog::getOpenFileNames(this,tr("Open binary image"),
				  mMainWindowBase->GetInputPathName(),Extensions);
  if (filename.size() == 0) return;
  
  // For each selected file, open the image
  for(int i=0; i<filename.size(); i++) {
    // Open Image
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    vvImageReader::Pointer reader = vvImageReader::New();
    std::vector<std::string> filenames;
    filenames.push_back(filename[i].toStdString());
    reader->SetInputFilenames(filenames);
    reader->Update(vvImageReader::IMAGE);
    QApplication::restoreOverrideCursor();

    if (reader->GetLastError().size() != 0) {
      std::cerr << "Error while reading " << filename[i].toStdString() << std::endl;
      QString error = "Cannot open file \n";
      error += reader->GetLastError().c_str();
      QMessageBox::information(this,tr("Reading problem"),error);
      return;
    }
    vvImage::Pointer binaryImage = reader->GetOutput();
    AddImage(binaryImage, filename[i].toStdString(), mBackgroundValueSpinBox->value(),
             (!mBGModeCheckBox->isChecked()));
    mOpenedBinaryImage.push_back(binaryImage);
  }

  // Update the contours
  UpdateAllContours(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::AddImage(vvImage * binaryImage, std::string filename, 
                                double BG, bool modeBG) 
{
  DD(modeBG);

  // Check Dimension
  int dim = mCurrentImage->GetNumberOfDimensions();
  int bin_dim = binaryImage->GetNumberOfDimensions();
  if (dim < bin_dim) {
    std::ostringstream os;
    os << "Error. Loaded binary image is " << bin_dim
       << "D while selected image is " << dim << "D" << std::endl;
    QMessageBox::information(this,tr("Reading problem"),os.str().c_str());
    return;
  }
  
  // Compute roi index
  int n = mROIList.size();
  DD(n);
  
  // Compute the name of the new ROI
  std::ostringstream oss;
  oss << vtksys::SystemTools::GetFilenameName(vtksys::SystemTools::GetFilenameWithoutLastExtension(filename));
  std::string name = oss.str();
  DD(name);
  
  // Set color
  std::vector<double> color;
  color.push_back(1);
  color.push_back(0);
  color.push_back(0);

  // Create ROI
  clitk::DicomRT_ROI::Pointer roi = clitk::DicomRT_ROI::New();
  roi->SetFromBinaryImage(binaryImage, n, name, color, filename);

  // Add a new roi to the list
  mROIList.push_back(roi);
 
  // Set BG or FG mode
  if (modeBG) 
    roi->SetBackgroundValueLabelImage(BG);
  else 
    roi->SetForegroundValueLabelImage(BG);
  
  // Change color
  DD("color");
  if (n<mDefaultLUTColor->GetNumberOfTableValues ()) {
    double * color = mDefaultLUTColor->GetTableValue(n % mDefaultLUTColor->GetNumberOfTableValues ());
    roi->SetDisplayColor(color[0], color[1], color[2]);
  }
  
  // Add a new roi actor
  QSharedPointer<vvROIActor> actor = QSharedPointer<vvROIActor>(new vvROIActor);
  actor->SetBGMode(modeBG);
  actor->SetROI(roi);
  actor->SetSlicerManager(mSlicerManager);
  actor->Initialize(n+1); // depth is n+1 to start at 1
  mROIActorsList.push_back(actor);
  
  // CheckBox for "All"
  if (actor->IsVisible()) mNumberOfVisibleROI++;
  if (actor->IsContourVisible()) mNumberOfVisibleContourROI++;
  
  // Add ROI in tree
  mTreeWidgetList.push_back(QSharedPointer<QTreeWidgetItem>(new QTreeWidgetItem(mTree)));
  QTreeWidgetItem * w = mTreeWidgetList.back().data();
  w->setText(0, QString("%1").arg(roi->GetROINumber()));
  w->setText(1, QString("%1").arg(roi->GetName().c_str()));
  w->setText(3, QString("%1").arg(actor->GetDepth()));  
  QBrush brush(QColor(roi->GetDisplayColor()[0]*255, 
                      roi->GetDisplayColor()[1]*255, 
                      roi->GetDisplayColor()[2]*255));
  brush.setStyle(Qt::SolidPattern);
  w->setBackground(2, brush);
  mMapROIToTreeWidget[roi] = w;
  mMapTreeWidgetToROI[w] = roi;
  mTree->resizeColumnToContents(0);
  mTree->resizeColumnToContents(1);

  // Update 
  UpdateAllROIStatus(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::UpdateAllContours() 
{
  // Render loaded ROIs (the first is sufficient)
  for(unsigned int i=0; i<mROIList.size(); i++) {
    mROIActorsList[i]->Update();
  }
  for(int i=0; i<mSlicerManager->GetNumberOfSlicers(); i++) {
    mSlicerManager->GetSlicer(i)->Render();
  }  
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::UpdateAllROIStatus() {
  int nbVisible = 0;
  int nb = mROIList.size();
  for(int i=0; i<nb; i++) {
    if (mROIActorsList[i]->IsVisible()) {
      nbVisible++;
    }
  }
  DD(nbVisible);

  // change the states
  disconnect(mCheckBoxShowAll, SIGNAL(stateChanged(int)), this, SLOT(AllVisibleROIToggled(int)));  
  disconnect(mContourCheckBoxShowAll, SIGNAL(toggled(bool)), this, SLOT(AllVisibleContourROIToggled(bool)));
  if (nbVisible == nb) mCheckBoxShowAll->setCheckState(Qt::Checked);
  else {
    if (nbVisible == 0) mCheckBoxShowAll->setCheckState(Qt::Unchecked);
    else mCheckBoxShowAll->setCheckState(Qt::PartiallyChecked);
  }
  connect(mContourCheckBoxShowAll, SIGNAL(toggled(bool)), this, SLOT(AllVisibleContourROIToggled(bool)));
  connect(mCheckBoxShowAll, SIGNAL(stateChanged(int)), this, SLOT(AllVisibleROIToggled(int)));
}
//------------------------------------------------------------------------------

