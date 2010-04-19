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
#include <QFileDialog>
#include <QMessageBox>
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
   Ui::vvToolStructureSetManager() {
  // Setup the UI
  Ui_vvToolStructureSetManager::setupUi(mToolWidget);
  mTree->clear();
  mCurrentStructureSet = NULL;
  
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
vvToolStructureSetManager::~vvToolStructureSetManager() {
  DD("vvToolStructureSetManager DESTRUCTOR");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::Initialize() {
  SetToolName("StructureSetManager");
  SetToolMenuName("StructureSet");
  SetToolIconFilename(":/common/icons/ducky.png");
  SetToolTip("Display Structure Set.");
  SetToolExperimental(true);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::InputIsSelected(vvSlicerManager *m) {
  // Hide the input selector
  QList<int> s;
  s.push_back(0);
  s.push_back(1);
  splitter->setSizes(s);
  // Connect open menus
  connect(mOpenComboBox, SIGNAL(activated(int)), this, SLOT(open(int)));
  DD(mCurrentImage->GetNumberOfDimensions());

  // To trigger the Render ??
  //  connect(m,SIGNAL(releasemouse()),this,SLOT(Render()));
  
  //  connect(m, SIGNAL(UpdateSlice(int, int)), SLOT(UpdateSlice(int, int)));
  connect(m, SIGNAL(LeftButtonReleaseSignal(int)), SLOT(LeftButtonReleaseEvent(int)));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::LeftButtonReleaseEvent(int slicer) {
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
void vvToolStructureSetManager::open(int type) {
  DD(type);
  switch (type) {
  case 0: openBinaryImage(); return; // Open binary image;
  case 1: DD("TODO"); return; // Open DICOM RT
  case 2: DD("TODO"); return; // Open mesh
  default: std::cerr << "Error ????" << std::endl; exit(0);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::addRoiInTreeWidget(clitk::DicomRT_ROI * roi, QTreeWidgetItem * ww) {
  QTreeWidgetItem * w = new QTreeWidgetItem(ww);
  w->setText(0, QString("%1").arg(roi->GetROINumber()));
  w->setText(1, QString("%1").arg(roi->GetName().c_str()));
  QBrush brush(QColor(roi->GetDisplayColor()[0]*255, roi->GetDisplayColor()[1]*255, roi->GetDisplayColor()[2]*255));
  brush.setStyle(Qt::SolidPattern);
  for(int i=0; i<w->columnCount (); i++) {
    w->setBackground(i, brush);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::addStructureSetInTreeWidget(int index, clitk::DicomRT_StructureSet * s) {
  // Main row item
  QTreeWidgetItem * ss = new QTreeWidgetItem(mTree);
  //  ss->setFlags(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsTristate);
  ss->setText(0, QString("S%1").arg(index));
  ss->setText(1, QString("%1").arg(s->GetLabel().c_str()));

  // Insert ROI
  const std::vector<clitk::DicomRT_ROI*> & rois = s->GetListOfROI();
  for(unsigned int i=0; i<rois.size(); i++) {
    DD(i);
    addRoiInTreeWidget(rois[i], ss);
  }
  
  // Insert in list
  mStructureSetItemsList[index] = ss;
  
  // Connect
  //  TODO
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
int vvToolStructureSetManager::addStructureSet(clitk::DicomRT_StructureSet * mStructureSet) {

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
void vvToolStructureSetManager::openBinaryImage() {
  DD("openBinaryImage");
  // Select current StructureSet (or create)
  int index;
  if (mCurrentStructureSet == NULL) {
    if (mStructureSetsList.size() == 0) { // Create a default SS
      clitk::DicomRT_StructureSet * mStructureSet = new clitk::DicomRT_StructureSet;
      index = addStructureSet(mStructureSet);
      DD(index);
    }
    else { // Get first SS
      index = 0;
    }
    // TODO -> SET THIS SS AS CURRENT
    mCurrentStructureSet = mStructureSetsList[index];
    mCurrentStructureSetActor = mStructureSetActorsList[index];
  }
  else {
    index = mCurrentStructureSetIndex;
  }
  DD(mCurrentStructureSet->GetName());

  // Open images
  QString Extensions = "Images files ( *.mhd *.hdr *.his)";
  Extensions += ";;All Files (*)";
  QStringList filename = 
    QFileDialog::getOpenFileNames(this,tr("Open binary image"),
				 mMainWindowBase->GetInputPathName(),Extensions);
  if (filename.size() == 0) return;

  for(int i=0; i<filename.size(); i++) {
    DD(filename[i].toStdString());

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
  DD(dim);
  int bin_dim = binaryImage->GetNumberOfDimensions();
  DD(bin_dim);
  if (dim < bin_dim) {  ////////// TO CHANGE FOR 3D/4D
    std::ostringstream os;
    os << "Error. Loaded binary image is " << bin_dim 
	      << "D while selected image is " << dim << "D" << std::endl;
    QMessageBox::information(this,tr("Reading problem"),os.str().c_str());
    return;
  }

  // Add a new roi to the structure
  int n = mCurrentStructureSet->AddBinaryImageAsNewROI(binaryImage, filename[i].toStdString());
  DD(n);
  
  // Change color NEED DEFAULT COLOR LIST
  DD(mDefaultLUTColor->GetNumberOfTableValues ());
  if (n<mDefaultLUTColor->GetNumberOfTableValues ()) {
    double * color = mDefaultLUTColor->GetTableValue(n % mDefaultLUTColor->GetNumberOfTableValues ());
    DD(color[0]);
    DD(color[1]);
    DD(color[2]);
    mCurrentStructureSet->GetROI(n)->SetDisplayColor(color[0], color[1], color[2]);
  }
  
  // Add a new roi actor
  mCurrentStructureSetActor->CreateNewROIActor(n);
  }

  // Update the TreeWidget
  addStructureSetInTreeWidget(index, mCurrentStructureSet);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolStructureSetManager::apply() {
  close();
}
//------------------------------------------------------------------------------
