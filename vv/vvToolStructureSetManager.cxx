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

// vv
#include "vvToolStructureSetManager.h"
#include "vvImageReader.h"
#include "vvStructureSetActor.h"
#include "vvSlicer.h"
#include "vvROIActor.h"

// Qt
#include <QFileDialog>
#include <QMessageBox>
#include <QColorDialog>

// vtk
#include <vtkLookupTable.h>
#include <vtkRenderWindow.h>

//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolStructureSetManager);
//------------------------------------------------------------------------------

int vvToolStructureSetManager::m_NumberOfTool = 0;
std::vector<vvSlicerManager*> vvToolStructureSetManager::mListOfInputs;
std::map<vvSlicerManager*, vvToolStructureSetManager*> vvToolStructureSetManager::mListOfOpenTool;

//------------------------------------------------------------------------------
vvToolStructureSetManager::vvToolStructureSetManager(vvMainWindowBase * parent, 
                                                     Qt::WindowFlags f, 
                                                     vvSlicerManager * current):
  vvToolWidgetBase(parent, f), 
  // if Qt::Widget -> No dialog in this case (in tab) ; PB = "invisible widget on menu" !
  // if "f" normal widget
  vvToolBase<vvToolStructureSetManager>(parent),
  Ui::vvToolStructureSetManager()
{
  Ui_vvToolStructureSetManager::setupUi(mToolWidget);

  //  this->setFixedWidth(120);
  m_NumberOfTool++;
  mCurrentStructureSetActor = 0;
  connect(mCloseButton, SIGNAL(clicked()), this, SLOT(close()));
  mCloseButton->setVisible(false);
  mTree->clear();
  mTree->header()->resizeSection(0, 30);
  mMainWindowBase->GetTab()->setTabIcon(mTabNumber, GetToolIcon());
  mCurrentStructureSet = NULL;
  mCurrentStructureSetIndex = -1;
  mGroupBoxROI->setEnabled(false);
  mCurrentROIActor = NULL;
  mIsAllVisibleEnabled = false;
  mNumberOfVisibleROI = 0;
  mNumberOfVisibleContourROI = 0;
  mDefaultLUTColor = vtkSmartPointer<vtkLookupTable>::New();
  for(int i=0; i<mDefaultLUTColor->GetNumberOfTableValues(); i++) {
    double r = (rand()/(RAND_MAX+1.0));
    double v = (rand()/(RAND_MAX+1.0));
    double b = (rand()/(RAND_MAX+1.0));
    mDefaultLUTColor->SetTableValue(i, r, v, b);
    //    std::cout << "mDefaultLUTColor->SetTableValue(" << i << ", " << r << ", " << v << ", " << b << ");" << std::endl;
  }
#include "vvDefaultLut.h"

  // Add input selector
  if (current == NULL) {
    MustOpenDialogWhenCreated = true;
    AddInputSelector("Select image");
  }
  else {
    MustOpenDialogWhenCreated = false;
    mMainButtonBox->setEnabled(true);
    mCurrentSlicerManager = current;
    mCurrentImage = mCurrentSlicerManager->GetImage();
    mToolWidget->setEnabled(true);
    InputIsSelected(mCurrentSlicerManager);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolStructureSetManager::~vvToolStructureSetManager()
{
  m_NumberOfTool--;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// STATIC
void vvToolStructureSetManager::Initialize() {
  SetToolName("ROIManager");
  SetToolMenuName("Display ROI (binary image)");
  SetToolIconFilename(":/common/icons/tool-roi.png");
  SetToolTip("Display ROI from a binary image.");
  SetToolExperimental(false);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::InputIsSelected(vvSlicerManager *m)
{
  //int mTabNumber = parent->GetTab()->addTab(this, "");
  //  this->setFixedWidth(120);
  //this->setPreferedHeight(441);  
  // Refuse if 4D
  if (mCurrentImage->GetNumberOfDimensions() != 3) {
    QMessageBox::information(this,tr("Sorry only 3D yet"), tr("Sorry only 3D yet"));
    close();
  }
  // Hide selector
  HideInputSelector(); // splitter
  mToolInputSelectionWidget->hide();
  mLabelInputInfo->setText(QString("%1").arg(m->GetFileName().c_str()));

  // add to instance
  // if (!isWindow()) {
  mListOfInputs.push_back(mCurrentSlicerManager);
  mListOfOpenTool[mCurrentSlicerManager] = this;
  // }

  // Connect open menus
  connect(mOpenBinaryButton, SIGNAL(clicked()), this, SLOT(OpenBinaryImage()));
  connect(mTree, SIGNAL(itemSelectionChanged()), this, SLOT(SelectedItemChangedInTree()));
  connect(mCheckBoxShow, SIGNAL(toggled(bool)), this, SLOT(VisibleROIToggled(bool)));
  connect(mOpacitySlider, SIGNAL(valueChanged(int)), this, SLOT(OpacityChanged(int)));
  connect(mChangeColorButton, SIGNAL(clicked()), this, SLOT(ChangeColor()));
  connect(mContourCheckBoxShow, SIGNAL(toggled(bool)), this, SLOT(VisibleContourROIToggled(bool)));  
  connect(mChangeContourColorButton, SIGNAL(clicked()), this, SLOT(ChangeContourColor()));
  connect(mContourWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(ChangeContourWidth(int)));
  connect(mReloadButton, SIGNAL(clicked()), this, SLOT(ReloadCurrentROI()));
  connect(mCheckBoxShowAll, SIGNAL(stateChanged(int)), this, SLOT(AllVisibleROIToggled(int)));
  connect(mContourCheckBoxShowAll, SIGNAL(toggled(bool)), this, SLOT(AllVisibleContourROIToggled(bool)));

  // Browse to load image
  if (MustOpenDialogWhenCreated)
    OpenBinaryImage();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::Open(int type) {
  switch (type) {
  case 0: OpenBinaryImage(); return; // Open binary image;
  case 1: DD("TODO"); return; // Open DICOM RT
  case 2: DD("TODO"); return; // Open mesh
  default: std::cerr << "Error ????" << std::endl; exit(0);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::AddRoiInTreeWidget(clitk::DicomRT_ROI * roi, QTreeWidget * ww) {
  mTreeWidgetList.push_back(QSharedPointer<QTreeWidgetItem>(new QTreeWidgetItem(ww)));
  QTreeWidgetItem * w = mTreeWidgetList.back().data();
  w->setText(0, QString("%1").arg(roi->GetROINumber()));
  w->setText(1, QString("%1").arg(roi->GetName().c_str()));
  QBrush brush(QColor(roi->GetDisplayColor()[0]*255, roi->GetDisplayColor()[1]*255, roi->GetDisplayColor()[2]*255));
  brush.setStyle(Qt::SolidPattern);
  w->setBackground(2, brush);
  mMapROIToTreeWidget[roi] = w;
  mMapTreeWidgetToROI[w] = roi;
  mTree->resizeColumnToContents(0);
  mTree->resizeColumnToContents(1);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::UpdateStructureSetInTreeWidget(int index, clitk::DicomRT_StructureSet * s) {
  // Insert ROI
  const std::vector<clitk::DicomRT_ROI::Pointer> & rois = s->GetListOfROI();
  for(unsigned int i=0; i<rois.size(); i++) {
    if (mMapROIToTreeWidget.find(rois[i]) == mMapROIToTreeWidget.end())
      AddRoiInTreeWidget(rois[i], mTree); // replace mTree with ss if several SS
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
int vvToolStructureSetManager::AddStructureSet(clitk::DicomRT_StructureSet * mStructureSet) {
  // Create actor for this SS

  QSharedPointer<vvStructureSetActor> mStructureSetActor(new vvStructureSetActor);
  
  mStructureSetActor->SetStructureSet(mStructureSet);
  mStructureSetActor->SetSlicerManager(mCurrentSlicerManager);
  // Insert in lists and get index
  mStructureSetsList.push_back(mStructureSet);
  mStructureSetActorsList.push_back(mStructureSetActor);
  int index = mStructureSetsList.size()-1;
  // Return index
  return index;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::OpenBinaryImage() 
{
  int index;
  if (mCurrentStructureSet == NULL) {
    if (mStructureSetsList.size() == 0) { // Create a default SS
      clitk::DicomRT_StructureSet::Pointer mStructureSet = clitk::DicomRT_StructureSet::New();
      index = AddStructureSet(mStructureSet);
    }
    else { // Get first SS
      index = 0;
    }
  } else {
    index = mCurrentStructureSetIndex;
  }
  mCurrentStructureSet = mStructureSetsList[index];
  mCurrentStructureSetActor = mStructureSetActorsList[index].data();
  mCurrentStructureSetIndex = index;
  // Open images
  QString Extensions = "Images files ( *.mhd *.hdr *.his)";
  Extensions += ";;All Files (*)";
  QStringList filename =
    QFileDialog::getOpenFileNames(this,tr("Open binary image"),
				  mMainWindowBase->GetInputPathName(),Extensions);
  if (filename.size() == 0) return;
  mLoadedROIIndex.clear();
  for(int i=0; i<filename.size(); i++) {
    // Open Image
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    vvImageReader::Pointer reader = vvImageReader::New();
    std::vector<std::string> filenames;
    filenames.push_back(filename[i].toStdString());
    reader->SetInputFilenames(filenames);
    reader->Update(IMAGE);
    QApplication::restoreOverrideCursor();

    if (reader->GetLastError().size() != 0) {
      std::cerr << "Error while reading " << filename[i].toStdString() << std::endl;
      QString error = "Cannot open file \n";
      error += reader->GetLastError().c_str();
      QMessageBox::information(this,tr("Reading problem"),error);
      return;
    }
    vvImage::Pointer binaryImage = reader->GetOutput();
    AddImage(binaryImage, filename[i].toStdString(), mBackgroundValueSpinBox->value());
    mOpenedBinaryImage.push_back(binaryImage);
  }

  UpdateImage();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::UpdateImage() 
{
  // Update the TreeWidget
  UpdateStructureSetInTreeWidget(mCurrentStructureSetIndex, mCurrentStructureSet);
  // Render loaded ROIs (the first is sufficient)
  for(unsigned int i=0; i<mLoadedROIIndex.size(); i++) {
    mCurrentStructureSetActor->GetROIActor(mLoadedROIIndex[i])->Update();
  }
  for(int i=0; i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
    mCurrentSlicerManager->GetSlicer(i)->Render();
  }  
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::AddImage(vvImage * binaryImage, std::string filename, 
                                         double BG, bool m_modeBG) 
{
  // Check current structure set
  int index;
  if (mCurrentStructureSet == NULL) {
    if (mStructureSetsList.size() == 0) { // Create a default SS
      clitk::DicomRT_StructureSet::Pointer mStructureSet = clitk::DicomRT_StructureSet::New();
      index = AddStructureSet(mStructureSet);
    }
    else { // Get first SS
      index = 0;
    }
  } else {
    index = mCurrentStructureSetIndex;
  }
  mCurrentStructureSet = mStructureSetsList[index];
  mCurrentStructureSetActor = mStructureSetActorsList[index].data();
  mCurrentStructureSetIndex = index;

  // Check Dimension
  int dim = mCurrentImage->GetNumberOfDimensions();
  int bin_dim = binaryImage->GetNumberOfDimensions();
  if (dim < bin_dim) {  ////////// TO CHANGE FOR 3D/4D
    std::ostringstream os;
    os << "Error. Loaded binary image is " << bin_dim
       << "D while selected image is " << dim << "D" << std::endl;
    QMessageBox::information(this,tr("Reading problem"),os.str().c_str());
    return;
  }
  
  // Add a new roi to the structure
  int n = mCurrentStructureSet->AddBinaryImageAsNewROI(binaryImage, filename);
  mLoadedROIIndex.push_back(n);
  if (m_modeBG) 
    mCurrentStructureSet->GetROI(n)->SetBackgroundValueLabelImage(BG);
  else 
    mCurrentStructureSet->GetROI(n)->SetForegroundValueLabelImage(BG);
  
  // Change color
  if (n<mDefaultLUTColor->GetNumberOfTableValues ()) {
    double * color = mDefaultLUTColor->GetTableValue(n % mDefaultLUTColor->GetNumberOfTableValues ());
    mCurrentStructureSet->GetROI(n)->SetDisplayColor(color[0], color[1], color[2]);
  }
  
  // Add a new roi actor
  mCurrentStructureSetActor->CreateNewROIActor(n, m_modeBG);
  
  // CheckBox for "All"
  if (mCurrentStructureSetActor->GetROIActor(n)->IsVisible())
    mNumberOfVisibleROI++;
  if (mCurrentStructureSetActor->GetROIActor(n)->IsContourVisible())
    mNumberOfVisibleContourROI++;
  UpdateAllROIStatus();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::apply() 
{
  close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolStructureSetManager::close()
{
  return vvToolWidgetBase::close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::closeEvent(QCloseEvent *event) 
{
  std::vector<vvSlicerManager*>::iterator iter = std::find(mListOfInputs.begin(), mListOfInputs.end(), mCurrentSlicerManager);
  if (iter != mListOfInputs.end()) mListOfInputs.erase(iter);
  
  //  DD("how delete mListOfOpenTool ???");
  mListOfOpenTool.erase(mCurrentSlicerManager);

  mCheckBoxShowAll->setCheckState(Qt::Unchecked);
  mContourCheckBoxShowAll->setCheckState(Qt::Unchecked);
  if (mCurrentSlicerManager != 0)
    mCurrentSlicerManager->Render();

  if (!isWindow()) {
    if (m_NumberOfTool == 1) {
      mMainWindow->GetTab()->removeTab(mTabNumber);
    }
  } 

  event->accept();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// CURRENT ROI INTERACTION
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::SelectedItemChangedInTree() {
  // Search which roi is selected
  QList<QTreeWidgetItem *> l = mTree->selectedItems();
  if (l.size() == 0) {
    mCurrentROIActor = NULL;
    mCurrentROI = NULL;
    mGroupBoxROI->setEnabled(false);
    return;
  }
  QTreeWidgetItem * w = l[0];
  if (mMapTreeWidgetToROI.find(w) == mMapTreeWidgetToROI.end()) {
    mCurrentROIActor = NULL;
    mCurrentROI = NULL;
    mGroupBoxROI->setEnabled(false);
    return;
  }
  clitk::DicomRT_ROI * roi = mMapTreeWidgetToROI[w];
  // Get selected roi actor
  vvROIActor * actor = mStructureSetActorsList[mCurrentStructureSetIndex]->GetROIActor(roi->GetROINumber());
  mCurrentROI = roi;
  mCurrentROIActor = actor;

  // Update GUI
  mGroupBoxROI->setEnabled(true);
  mROInameLabel->setText(roi->GetName().c_str());
  mCheckBoxShow->setChecked(actor->IsVisible());
  mContourCheckBoxShow->setChecked(actor->IsContourVisible());
  mContourWidthSpinBox->setValue(actor->GetContourWidth());
  
  // Warning -> avoir unuseful Render here by disconnect slider 
  disconnect(mOpacitySlider, SIGNAL(valueChanged(int)), 
	     this, SLOT(OpacityChanged(int)));
  mOpacitySlider->setValue((int)lrint(actor->GetOpacity()*100));
  mOpacitySpinBox->setValue((int)lrint(actor->GetOpacity()*100));
  connect(mOpacitySlider, SIGNAL(valueChanged(int)), 
	  this, SLOT(OpacityChanged(int)));
  actor->Update(); 

  // Final rendering
  mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::UpdateAllROIStatus() {
  int nbVisible = 0;
  int nb = mCurrentStructureSetActor->GetROIList().size();
  for(int i=0; i<nb; i++) {
    if (mCurrentStructureSetActor->GetROIList()[i]->IsVisible()) {
      nbVisible++;
    }
  }

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
void vvToolStructureSetManager::VisibleROIToggled(bool b) {
  if (mCurrentROIActor == NULL) return;
  if (b == mCurrentROIActor->IsVisible()) return; // nothing to do
  mCurrentROIActor->SetVisible(b);
  UpdateAllROIStatus();
  mCurrentSlicerManager->Render(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::VisibleContourROIToggled(bool b) {
  if (mCurrentROIActor == NULL) return;
  if (mCurrentROIActor->IsContourVisible() == b) return; // nothing to do
  mCurrentROIActor->SetContourVisible(b);
  mCurrentROIActor->UpdateColor();
  mCurrentSlicerManager->Render(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::OpacityChanged(int v) {
  if (mCurrentROIActor == NULL) return;
  mCurrentROIActor->SetOpacity((double)v/100.0);
  mCurrentROIActor->UpdateColor();
  mCurrentSlicerManager->Render(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::AllVisibleROIToggled(int b) {
  bool status = false;
  if ((mCheckBoxShowAll->checkState() == Qt::Checked) ||
      (mCheckBoxShowAll->checkState() == Qt::PartiallyChecked))  status = true;

  for(int i=0; i<mCurrentStructureSetActor->GetNumberOfROIs(); i++) {
    mCurrentStructureSetActor->GetROIList()[i]->SetVisible(status);
  }
  if (status) mCheckBoxShowAll->setCheckState(Qt::Checked);
  else  mCheckBoxShowAll->setCheckState(Qt::Unchecked);
  mCheckBoxShow->setChecked(status);
  mCurrentSlicerManager->Render(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::AllVisibleContourROIToggled(bool b) {
  bool status = false;
  if ((mContourCheckBoxShowAll->checkState() == Qt::Checked) ||
      (mContourCheckBoxShowAll->checkState() == Qt::PartiallyChecked))  status = true;
  // Update current 
  for(int i=0; i<mCurrentStructureSetActor->GetNumberOfROIs(); i++) {
    mCurrentStructureSetActor->GetROIList()[i]->SetContourVisible(status);
  }
  // Update current selection
  if (status) mContourCheckBoxShowAll->setCheckState(Qt::Checked);
  else  mContourCheckBoxShowAll->setCheckState(Qt::Unchecked);
  mContourCheckBoxShow->setChecked(status);
  mCurrentSlicerManager->Render(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::ChangeColor() {
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
  mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::ChangeContourColor() {
  QColor color;
  color.setRgbF(mCurrentROIActor->GetContourColor()[0], 
		mCurrentROIActor->GetContourColor()[1], 
		mCurrentROIActor->GetContourColor()[2]);
  QColor c = QColorDialog::getColor(color, this, "Choose the contour color");
  mCurrentROIActor->SetContourColor(c.redF(), c.greenF(), c.blueF());
  mCurrentROIActor->UpdateColor();
  mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::ChangeContourWidth(int n) {
  mCurrentROIActor->SetContourWidth(n);
  mCurrentROIActor->UpdateColor();
  mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::ReloadCurrentROI() {
  // Reload image
  vvImageReader::Pointer reader = vvImageReader::New();
  reader->SetInputFilename(mCurrentROI->GetFilename());
  reader->Update(IMAGE);
  if (reader->GetLastError() != "") {
    QMessageBox::information(mMainWindowBase, tr("Sorry, error. Could not reload"), reader->GetLastError().c_str());
    return;
  }
  mCurrentROI->GetImage()->GetFirstVTKImageData()->ReleaseData();
  mCurrentROI->SetImage(reader->GetOutput());
  
  // Update visu"
  mCurrentROIActor->UpdateImage();
  mCurrentSlicerManager->Render();    
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::CheckInputList(std::vector<vvSlicerManager*> & l, int & index) 
{
  for(unsigned int i=0; i<l.size(); i++) {
    std::vector<vvSlicerManager*>::iterator iter = std::find(mListOfInputs.begin(), mListOfInputs.end(), l[i]);
    if (iter != mListOfInputs.end()) {
      for(unsigned int j=i;j<l.size(); j++) l[j] = l[j+1];
      l.pop_back();
      if (index == (int)i) index = 0;
      i--;
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// STATIC
vvToolStructureSetManager * vvToolStructureSetManager::AddImage(vvSlicerManager * m, std::string name, vvImage::Pointer image, double BG, bool m_modeBG)
{
  // If the tool is open for this vvSlicerManager, use it and return
  if (mListOfOpenTool[m]) {
    vvToolStructureSetManager * tool = mListOfOpenTool[m];
    tool->AddImage(image, name, BG, m_modeBG);
    tool->UpdateImage();
    return tool;
  }

  // If the tool is not open, create it
  vvToolStructureSetManager * tool = new vvToolStructureSetManager
    (CREATOR(vvToolStructureSetManager)->GetMainWindow(), Qt::Dialog, m);
  tool->AddImage(image, name, BG, m_modeBG);
  tool->UpdateImage();
  tool->show();
  return tool;
}
//------------------------------------------------------------------------------
