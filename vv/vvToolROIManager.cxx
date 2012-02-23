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
  QWidget(parent->GetTab()), 
  vvToolBase<vvToolROIManager>(parent),
  Ui::vvToolROIManager()
{
  // Store parent
  mMainWindow = parent;
  
  // Assume the initial tab ROI index is 2
  mIndexFirstTab = 2;

  // Get the ROI Tab
  QWidget * tab = qFindChild<QWidget*>(parent->GetTab(), "ROItab");
  
  // Set it as current
  parent->GetTab()->setCurrentIndex(mIndexFirstTab);
  
  // Check if widget already used
  if (tab->layout()->isEmpty()) {
    tab->layout()->addWidget(this);
  }
  else {
    close();
    return;
  }
  
  // Build the UI
  Ui_vvToolROIManager::setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
  mTree->clear();
  mTree->header()->resizeSection(0, 30);

  // Set default LUT
  mDefaultLUTColor = vtkSmartPointer<vtkLookupTable>::New();
  for(int i=0; i<mDefaultLUTColor->GetNumberOfTableValues(); i++) {
    double r = (rand()/(RAND_MAX+1.0));
    double v = (rand()/(RAND_MAX+1.0));
    double b = (rand()/(RAND_MAX+1.0));
    mDefaultLUTColor->SetTableValue(i, r, v, b);
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
  connect(mOpenBinaryButton, SIGNAL(clicked()), this, SLOT(OpenBinaryImage()));
  connect(mTree, SIGNAL(itemSelectionChanged()), this, SLOT(SelectedItemChangedInTree()));
  connect(mCheckBoxShow, SIGNAL(toggled(bool)), this, SLOT(VisibleROIToggled(bool)));
  connect(mOpacitySlider, SIGNAL(valueChanged(int)), this, SLOT(OpacityChanged(int)));
  connect(mChangeColorButton, SIGNAL(clicked()), this, SLOT(ChangeColor()));
  connect(mContourCheckBoxShow, SIGNAL(toggled(bool)), this, SLOT(VisibleContourROIToggled(bool)));  
  connect(mChangeContourColorButton, SIGNAL(clicked()), this, SLOT(ChangeContourColor()));
  connect(mContourWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(ChangeContourWidth(int)));
  connect(mDepthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(ChangeDepth(int)));
  connect(mReloadButton, SIGNAL(clicked()), this, SLOT(ReloadCurrentROI()));
  connect(mCheckBoxShowAll, SIGNAL(stateChanged(int)), this, SLOT(AllVisibleROIToggled(int)));
  connect(mContourCheckBoxShowAll, SIGNAL(toggled(bool)), this, SLOT(AllVisibleContourROIToggled(bool)));
  connect(mCloseButton, SIGNAL(clicked()), this, SLOT(close()));
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
  mSlicerManager = m;

  // Initialization
  mSlicerManager = m;
  mCurrentImage = mSlicerManager->GetImage();

  // Refuse if 4D
  if (mCurrentImage->GetNumberOfDimensions() != 3) {
    QMessageBox::information(this,tr("Sorry only 3D yet"), tr("Sorry only 3D yet"));
    close();
    return;
  }

  // Change gui
  mLabelInputInfo->setText(QString("%1").arg(m->GetFileName().c_str()));

  // Auto display browser to select new contours 
  OpenBinaryImage();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::AnImageIsBeingClosed(vvSlicerManager * m)
{
  DD("AnImageIsBeingClosed");
  if (m == mSlicerManager) { 
    close();
    return;
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::close()
{
  DD("close");
  QWidget::close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::SelectedImageHasChanged(vvSlicerManager * m) {
  DD("SelectedImageHasChanged");
  if (m != mSlicerManager) hide(); 
  else {
    show();
  }
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
  
  // Compute the name of the new ROI
  std::ostringstream oss;
  oss << vtksys::SystemTools::GetFilenameName(vtksys::SystemTools::GetFilenameWithoutLastExtension(filename));
  std::string name = oss.str();
  
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


//------------------------------------------------------------------------------
void vvToolROIManager::SelectedItemChangedInTree() {
  
  // Search which roi is selected
  QList<QTreeWidgetItem *> l = mTree->selectedItems();
  if (l.size() == 0) {
    //    mCurrentROIActor = 0;
    mCurrentROI = NULL;
    mGroupBoxROI->setEnabled(false);
    return;
  }
  QTreeWidgetItem * w = l[0];
  if (mMapTreeWidgetToROI.find(w) == mMapTreeWidgetToROI.end()) {
    //    mCurrentROIActor = 0;
    mCurrentROI = NULL;
    mGroupBoxROI->setEnabled(false);
    return;
  }
  clitk::DicomRT_ROI * roi = mMapTreeWidgetToROI[w];
  // Get selected roi actor
  QSharedPointer<vvROIActor> actor = mROIActorsList[roi->GetROINumber()];
  mCurrentROI = roi;
  mCurrentROIActor = actor;

  // Warning -> avoid unuseful Render here by disconnect slider 
  // Update GUI
  disconnect(mTree, SIGNAL(itemSelectionChanged()), this, SLOT(SelectedItemChangedInTree()));
  disconnect(mCheckBoxShow, SIGNAL(toggled(bool)), this, SLOT(VisibleROIToggled(bool)));
  disconnect(mOpacitySlider, SIGNAL(valueChanged(int)), this, SLOT(OpacityChanged(int)));
  disconnect(mChangeColorButton, SIGNAL(clicked()), this, SLOT(ChangeColor()));
  disconnect(mContourCheckBoxShow, SIGNAL(toggled(bool)), this, SLOT(VisibleContourROIToggled(bool)));  
  disconnect(mChangeContourColorButton, SIGNAL(clicked()), this, SLOT(ChangeContourColor()));
  disconnect(mContourWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(ChangeContourWidth(int)));
  disconnect(mDepthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(ChangeDepth(int)));

  mGroupBoxROI->setEnabled(true);
  mROInameLabel->setText(roi->GetName().c_str());
  mCheckBoxShow->setChecked(actor->IsVisible());
  mContourCheckBoxShow->setChecked(actor->IsContourVisible());
  mContourWidthSpinBox->setValue(actor->GetContourWidth());
  mDepthSpinBox->setValue(actor->GetDepth());
  w->setText(3, QString("%1").arg(actor->GetDepth()));
  mOpacitySlider->setValue((int)lrint(actor->GetOpacity()*100));
  mOpacitySpinBox->setValue((int)lrint(actor->GetOpacity()*100));

  connect(mTree, SIGNAL(itemSelectionChanged()), this, SLOT(SelectedItemChangedInTree()));
  connect(mCheckBoxShow, SIGNAL(toggled(bool)), this, SLOT(VisibleROIToggled(bool)));
  connect(mOpacitySlider, SIGNAL(valueChanged(int)), this, SLOT(OpacityChanged(int)));
  connect(mChangeColorButton, SIGNAL(clicked()), this, SLOT(ChangeColor()));
  connect(mContourCheckBoxShow, SIGNAL(toggled(bool)), this, SLOT(VisibleContourROIToggled(bool)));  
  connect(mChangeContourColorButton, SIGNAL(clicked()), this, SLOT(ChangeContourColor()));
  connect(mContourWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(ChangeContourWidth(int)));
  connect(mDepthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(ChangeDepth(int)));

  
  // Set the current color to the selected ROI name
  mROInameLabel->setAutoFillBackground(true);// # This is important!!
  mROInameLabel->setStyleSheet("QLabel { background-color : red; color : blue; }");
  QColor color = QColor(mCurrentROI->GetDisplayColor()[0]*255,
                        mCurrentROI->GetDisplayColor()[1]*255,
                        mCurrentROI->GetDisplayColor()[2]*255);
  QString values = QString("%1, %2, %3").arg(color.red()).arg(color.green()).arg(color.blue());
  mROInameLabel->setStyleSheet("QLabel { background-color: rgb("+values+"); }");

  // is this needed ?
  //  actor->Update(); 
  // Final rendering
  // mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::VisibleROIToggled(bool b) {
  if (mCurrentROIActor == NULL) return;
  if (b == mCurrentROIActor->IsVisible()) return; // nothing to do
  mCurrentROIActor->SetVisible(b);
  UpdateAllROIStatus();
  mSlicerManager->Render(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::VisibleContourROIToggled(bool b) {
  if (mCurrentROIActor == NULL) return;
  if (mCurrentROIActor->IsContourVisible() == b) return; // nothing to do
  mCurrentROIActor->SetContourVisible(b);
  mCurrentROIActor->UpdateColor();
  mSlicerManager->Render(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::OpacityChanged(int v) {
  if (mCurrentROIActor == NULL) return;
  mCurrentROIActor->SetOpacity((double)v/100.0);
  mCurrentROIActor->UpdateColor();
  mSlicerManager->Render(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::AllVisibleROIToggled(int b) {
  bool status = false;
  if ((mCheckBoxShowAll->checkState() == Qt::Checked) ||
      (mCheckBoxShowAll->checkState() == Qt::PartiallyChecked))  status = true;

  for(uint i=0; i<mROIList.size(); i++) {
    mROIActorsList[i]->SetVisible(status);
  }
  if (status) mCheckBoxShowAll->setCheckState(Qt::Checked);
  else  mCheckBoxShowAll->setCheckState(Qt::Unchecked);
  mCheckBoxShow->setChecked(status);
  mSlicerManager->Render(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::AllVisibleContourROIToggled(bool b) {
  bool status = false;
  if ((mContourCheckBoxShowAll->checkState() == Qt::Checked) ||
      (mContourCheckBoxShowAll->checkState() == Qt::PartiallyChecked))  status = true;
  // Update current 
  for(uint i=0; i<mROIActorsList.size(); i++) {
    mROIActorsList[i]->SetContourVisible(status);
  }
  // Update current selection
  if (status) mContourCheckBoxShowAll->setCheckState(Qt::Checked);
  else  mContourCheckBoxShowAll->setCheckState(Qt::Unchecked);
  mContourCheckBoxShow->setChecked(status);
  mSlicerManager->Render(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::ChangeColor() {
  QColor color;
  color.setRgbF(mCurrentROIActor->GetROI()->GetDisplayColor()[0],
                mCurrentROIActor->GetROI()->GetDisplayColor()[1],
                mCurrentROIActor->GetROI()->GetDisplayColor()[2]);
  QColor c = QColorDialog::getColor(color, this, "Choose the ROI color");
  mCurrentROIActor->GetROI()->SetDisplayColor(c.redF(), c.greenF(), c.blueF());
  mCurrentROIActor->UpdateColor();

  QTreeWidgetItem * w = mMapROIToTreeWidget[mCurrentROI];
  QBrush brush(QColor(mCurrentROI->GetDisplayColor()[0]*255,
                      mCurrentROI->GetDisplayColor()[1]*255,
                      mCurrentROI->GetDisplayColor()[2]*255));
  brush.setStyle(Qt::SolidPattern);
  w->setBackground(2, brush);
  // Render
  mSlicerManager->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::ChangeContourColor() {
  QColor color;
  color.setRgbF(mCurrentROIActor->GetContourColor()[0], 
		mCurrentROIActor->GetContourColor()[1], 
		mCurrentROIActor->GetContourColor()[2]);
  QColor c = QColorDialog::getColor(color, this, "Choose the contour color");
  mCurrentROIActor->SetContourColor(c.redF(), c.greenF(), c.blueF());
  mCurrentROIActor->UpdateColor();
  mSlicerManager->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::ChangeContourWidth(int n) {
  mCurrentROIActor->SetContourWidth(n);
  mCurrentROIActor->UpdateColor();
  mSlicerManager->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::ChangeDepth(int n) {
  mCurrentROIActor->SetDepth(n);
  mCurrentROIActor->UpdateImage();
  mSlicerManager->Render();
  QList<QTreeWidgetItem *> l = mTree->selectedItems();
  QTreeWidgetItem * w = l[0];
  w->setText(3, QString("%1").arg(mCurrentROIActor->GetDepth()));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolROIManager::ReloadCurrentROI() {
  // Reload image
  vvImageReader::Pointer reader = vvImageReader::New();
  reader->SetInputFilename(mCurrentROI->GetFilename());
  reader->Update(vvImageReader::IMAGE);
  if (reader->GetLastError() != "") {
    QMessageBox::information(mMainWindowBase, tr("Sorry, error. Could not reload"), 
                             reader->GetLastError().c_str());
    return;
  }
  mCurrentROI->GetImage()->GetFirstVTKImageData()->ReleaseData();
  mCurrentROI->SetImage(reader->GetOutput());
  
  // Update visu"
  mCurrentROIActor->UpdateImage();
  mSlicerManager->Render();    
}
//------------------------------------------------------------------------------
