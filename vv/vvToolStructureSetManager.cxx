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

#include "vvToolStructureSetManager.h"
#include "vvImageReader.h"
#include "vvStructureSetActor.h"
#include "vvSlicer.h"
#include "vvROIActor.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QColorDialog>

#include <vtkLookupTable.h>
#include <vtkRenderWindow.h>

//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolStructureSetManager);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vvToolStructureSetManager::vvToolStructureSetManager(vvMainWindowBase * parent, Qt::WindowFlags f)
  :vvToolWidgetBase(parent, f),
   vvToolBase<vvToolStructureSetManager>(parent),
   Ui::vvToolStructureSetManager()
{
  // Setup the UI
  Ui_vvToolStructureSetManager::setupUi(mToolWidget);
  mTree->clear();
  mCurrentStructureSet = NULL;
  mCurrentStructureSetIndex = -1;
  mGroupBoxROI->setEnabled(false);
  mCurrentROIActor = NULL;
  mIsAllVisibleEnabled = false;
  mDefaultLUTColor = vtkLookupTable::New();
  for(unsigned int i=0; i<mDefaultLUTColor->GetNumberOfTableValues(); i++) {
    double r = (rand()/(RAND_MAX+1.0));
    double v = (rand()/(RAND_MAX+1.0));
    double b = (rand()/(RAND_MAX+1.0));
    mDefaultLUTColor->SetTableValue(i, r, v, b);
    //    std::cout << "mDefaultLUTColor->SetTableValue(" << i << ", " << r << ", " << v << ", " << b << ");" << std::endl;
  }
#include "vvDefaultLut.h"

  // Add input selector
  AddInputSelector("Select image");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolStructureSetManager::~vvToolStructureSetManager()
{
  DD("vvToolStructureSetManager DESTRUCTOR");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::Initialize() {
  SetToolName("ROIManager");
  SetToolMenuName("Display ROI");
  SetToolIconFilename(":/common/icons/ducky.png");
  SetToolTip("Display ROI from label image.");
  SetToolExperimental(true);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::InputIsSelected(vvSlicerManager *m)
{
  // Hide the input selector
  QList<int> s;
  s.push_back(0);
  s.push_back(1);
  splitter->setSizes(s);
  // Connect open menus
  //  connect(mOpenComboBox, SIGNAL(activated(int)), this, SLOT(open(int)));
  connect(mOpenBinaryButton, SIGNAL(clicked()), this, SLOT(OpenBinaryImage()));
  DD(mCurrentImage->GetNumberOfDimensions());

  // Seems that the following is not needed to refresh ...
  //  connect(m, SIGNAL(LeftButtonReleaseSignal(int)), SLOT(LeftButtonReleaseEvent(int)));

  connect(mTree, SIGNAL(itemSelectionChanged()), this, SLOT(SelectedItemChangedInTree()));
  connect(mCheckBoxShow, SIGNAL(toggled(bool)), this, SLOT(VisibleROIToggled(bool)));
  connect(mOpacitySlider, SIGNAL(valueChanged(int)), this, SLOT(OpacityChanged(int)));
  connect(mChangeColorButton, SIGNAL(clicked()), this, SLOT(ChangeColor()));
  connect(mContourCheckBoxShow, SIGNAL(toggled(bool)), this, SLOT(VisibleContourROIToggled(bool)));  
  connect(mChangeContourColorButton, SIGNAL(clicked()), this, SLOT(ChangeContourColor()));
  connect(mContourWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(ChangeContourWidth(int)));

  connect(mCheckBoxShowAll, SIGNAL(toggled(bool)), this, SLOT(AllVisibleROIToggled(bool)));
  connect(mOpacitySliderAll, SIGNAL(valueChanged(int)), this, SLOT(AllOpacityChanged(int)));
  connect(mContourCheckBoxShowAll, SIGNAL(toggled(bool)), this, SLOT(AllVisibleContourROIToggled(bool)));  
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::LeftButtonReleaseEvent(int slicer)
{
  DD("vvToolStructureSetManager::UpdateSlice");
  //DD(slicer);
  //DD(view);
  //DD(slices);
  for(int i=0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
    if (i != slicer);
    mCurrentSlicerManager->GetSlicer(i)->GetRenderWindow()->Render();
  }
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
  QTreeWidgetItem * w = new QTreeWidgetItem(ww);
  w->setText(0, QString("%1").arg(roi->GetROINumber()));
  w->setText(1, QString("%1").arg(roi->GetName().c_str()));
  //  w->setText(1, QString("%1").arg(roi->GetName().c_str()));
  QBrush brush(QColor(roi->GetDisplayColor()[0]*255, roi->GetDisplayColor()[1]*255, roi->GetDisplayColor()[2]*255));
  brush.setStyle(Qt::SolidPattern);
  //  for(int i=0; i<w->columnCount (); i++) {
  w->setBackground(2, brush);
  //}
  mMapROIToTreeWidget[roi] = w;
  mMapTreeWidgetToROI[w] = roi;
  // Connect ROI TreeWidget
  // TODO
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::UpdateStructureSetInTreeWidget(int index, clitk::DicomRT_StructureSet * s) {

  /* ==> Please, keep this comment (if need several SS)
     QTreeWidgetItem * ss;
     if (mMapStructureSetIndexToTreeWidget.find(index) == mMapStructureSetIndexToTreeWidget.end()) {
     // Main row item
     ss = new QTreeWidgetItem(mTree);
     //  ss->setFlags(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsTristate);
     ss->setText(0, QString("S%1").arg(index));
     ss->setText(1, QString("%1").arg(s->GetLabel().c_str()));
     // Insert in list
     mMapStructureSetIndexToTreeWidget[index] = ss;
    
     // Connect Structure TreeWidget
     // TODO
     }
     else ss = mMapStructureSetIndexToTreeWidget[index];
  */

  // Insert ROI
  const std::vector<clitk::DicomRT_ROI*> & rois = s->GetListOfROI();
  for(unsigned int i=0; i<rois.size(); i++) {
    if (mMapROIToTreeWidget.find(rois[i]) == mMapROIToTreeWidget.end())
      AddRoiInTreeWidget(rois[i], mTree); // replace mTree with ss if several SS
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
int vvToolStructureSetManager::AddStructureSet(clitk::DicomRT_StructureSet * mStructureSet) {

  // Create actor for this SS
  vvStructureSetActor * mStructureSetActor = new vvStructureSetActor;
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
void vvToolStructureSetManager::OpenBinaryImage() {
  //  DD("openBinaryImage");
  // Select current StructureSet (or create)
  int index;
  //  DD(mCurrentStructureSetIndex);
  if (mCurrentStructureSet == NULL) {
    if (mStructureSetsList.size() == 0) { // Create a default SS
      clitk::DicomRT_StructureSet * mStructureSet = new clitk::DicomRT_StructureSet;
      index = AddStructureSet(mStructureSet);
      //DD(index);
    }
    else { // Get first SS
      index = 0;
    }
  } else {
    index = mCurrentStructureSetIndex;
  }
  //  DD(index);
  // TODO -> SET THIS SS AS CURRENT
  mCurrentStructureSet = mStructureSetsList[index];
  mCurrentStructureSetActor = mStructureSetActorsList[index];
  mCurrentStructureSetIndex = index;
  //  DD(mCurrentStructureSetIndex);
  //DD(mCurrentStructureSet->GetName());

  // Open images
  QString Extensions = "Images files ( *.mhd *.hdr *.his)";
  Extensions += ";;All Files (*)";
  QStringList filename =
    QFileDialog::getOpenFileNames(this,tr("Open binary image"),
				  mMainWindowBase->GetInputPathName(),Extensions);
  if (filename.size() == 0) return;

  std::vector<int> mLoadedROIIndex;
  for(int i=0; i<filename.size(); i++) {
    //DD(filename[i].toStdString());

    // Open Image
    //init the progress events
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    vvImageReader * mReader = new vvImageReader;
    std::vector<std::string> filenames;
    filenames.push_back(filename[i].toStdString());
    mReader->SetInputFilenames(filenames);
    mReader->Update(IMAGE);
    QApplication::restoreOverrideCursor();

    if (mReader->GetLastError().size() != 0) {
      std::cerr << "Error while reading " << filename[i].toStdString() << std::endl;
      QString error = "Cannot open file \n";
      error += mReader->GetLastError().c_str();
      QMessageBox::information(this,tr("Reading problem"),error);
      delete mReader;
      return;
    }
    vvImage::Pointer binaryImage = mReader->GetOutput();
    //  delete mReader;

    // Check Dimension
    int dim = mCurrentImage->GetNumberOfDimensions();
    //DD(dim);
    int bin_dim = binaryImage->GetNumberOfDimensions();
    //DD(bin_dim);
    if (dim < bin_dim) {  ////////// TO CHANGE FOR 3D/4D
      std::ostringstream os;
      os << "Error. Loaded binary image is " << bin_dim
         << "D while selected image is " << dim << "D" << std::endl;
      QMessageBox::information(this,tr("Reading problem"),os.str().c_str());
      return;
    }

    // Add a new roi to the structure
    int n = mCurrentStructureSet->AddBinaryImageAsNewROI(binaryImage, filename[i].toStdString());
    //DD(n);
    mLoadedROIIndex.push_back(n);

    mCurrentStructureSet->GetROI(n)->SetBackgroundValueLabelImage(mBackgroundValueSpinBox->value());

    // Change color NEED DEFAULT COLOR LIST
    //DD(mDefaultLUTColor->GetNumberOfTableValues ());
    if (n<mDefaultLUTColor->GetNumberOfTableValues ()) {
      double * color = mDefaultLUTColor->GetTableValue(n % mDefaultLUTColor->GetNumberOfTableValues ());
      //DD(color[0]);
      //DD(color[1]);
      //DD(color[2]);
      mCurrentStructureSet->GetROI(n)->SetDisplayColor(color[0], color[1], color[2]);
    }

    // Add a new roi actor
    mCurrentStructureSetActor->CreateNewROIActor(n);

    // CheckBox for "All"
    if (mCurrentStructureSetActor->GetROIActor(n)->IsVisible())
      mNumberOfVisibleROI++;
  } // end loop on n selected filenames

  // Update the TreeWidget
  UpdateStructureSetInTreeWidget(index, mCurrentStructureSet);
  // Render loaded ROIs (the first is sufficient)
  for(unsigned int i=0; i<mLoadedROIIndex.size(); i++) {
    mCurrentStructureSetActor->GetROIActor(mLoadedROIIndex[i])->Update();
  }
  for(int i=0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
    mCurrentSlicerManager->GetSlicer(i)->Render();
  }
  
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::apply() {
  close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// CURRENT ROI INTERACTION
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::SelectedItemChangedInTree() {
  // Search which roi is selected
  QList<QTreeWidgetItem *> l = mTree->selectedItems();
  if (l.size() == 0) return;
  QTreeWidgetItem * w = l[0];
  if (mMapTreeWidgetToROI.find(w) == mMapTreeWidgetToROI.end()) {
    mCurrentROIActor = NULL;
    mCurrentROI = NULL;
    mGroupBoxROI->setEnabled(false);
    return; // Search for SS (first)
  }
  clitk::DicomRT_ROI * roi = mMapTreeWidgetToROI[w];
  //  DD(roi->GetName());

  // Get selected roi actor
  if (mCurrentROIActor != NULL) {
    mCurrentROIActor->SetSelected(false);
    mCurrentROIActor->Update();
  }

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

  // Temporary disable selection
  //  actor->SetSelected(true); // remove old selection  

  // The following must not render !!
  //  DD("before update");
  actor->Update(); // To change in UpdateSelecte
  //DD("after update");

  mCurrentSlicerManager->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::VisibleROIToggled(bool b) {
  mCurrentROIActor->SetVisible(b);
  if (b) mNumberOfVisibleROI++;
  else mNumberOfVisibleROI--;
  //mNumberOfVisibleROI;
  //  if (mNumberOfVisibleROI == mCurrentStructureSetIndex
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::VisibleContourROIToggled(bool b) {
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
void vvToolStructureSetManager::AllVisibleROIToggled(bool b) {
  DD(b);
  DD(mIsAllVisibleEnabled);
  DD(mNumberOfVisibleROI);
  if (b == mIsAllVisibleEnabled) return;
  if (b) mCheckBoxShowAll->setCheckState(Qt::Checked);
  else mCheckBoxShowAll->setCheckState(Qt::Unchecked);
  mIsAllVisibleEnabled = b;
  for(int i=0; i<mCurrentStructureSetActor->GetNumberOfROIs(); i++) {
    mCurrentStructureSetActor->GetROIList()[i]->SetVisible(b);
  }
  // Update current selection
  mCheckBoxShow->setChecked(b);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::AllVisibleContourROIToggled(bool b) {
  /*mCurrentROIActor->SetContourVisible(b);
    mCurrentROIActor->UpdateColor();
    mCurrentSlicerManager->Render(); 
  */
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::AllOpacityChanged(int v) {
  /*if (mCurrentROIActor == NULL) return;
    mCurrentROIActor->SetOpacity((double)v/100.0);
    mCurrentROIActor->UpdateColor();
    mCurrentSlicerManager->Render(); 
  */
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
  //  for(int i=0; i<w->columnCount (); i++) {
  w->setBackground(2, brush);
  //}
  
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
//void vvToolStructureSetManager::getActorFromROI() {
//  mStructureSetActorsList[mCurrentStructureSetIndex]->GetROIActor(n);
//}
//------------------------------------------------------------------------------


