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
#include "vvToolWidgetBase.h"
#include "vvMainWindowBase.h"
#include "vvSlicerManager.h"

// Qt
#include <QMessageBox>
#include <QKeyEvent>
#include <QDockWidget>

//------------------------------------------------------------------------------
// Static initialisation
int vvToolWidgetBase::mTabNumber = -1;
QWidget * vvToolWidgetBase::mStaticWidgetForTab = NULL;
QVBoxLayout * vvToolWidgetBase::mStaticVerticalLayout = NULL;
bool vvToolWidgetBase::mIsAnotherToolWaitInput = false;

//------------------------------------------------------------------------------
vvToolWidgetBase::vvToolWidgetBase(vvMainWindowBase * parent, Qt::WindowFlags f, bool initialize):
  QWidget(parent, f),
  Ui::vvToolWidgetBase()
{
  mMainWindow = parent;
  setAttribute(Qt::WA_DeleteOnClose);
  if (initialize) Initialization();
  // DD(isWindow());
  if (isWindow()) { // In this case, the tool is a floating windows
    // this->grabKeyboard();   // for the ESC key to close the dialog
    mPreventToUseTwoToolsOnSameInput = false;
  }
  else { // In this case it is inserted into a tab
    DD("Not implemented yet TODO !!");
    exit(0);
    // Prevent to load two tools at the same time
    DD(mIsAnotherToolWaitInput);
    if (mIsAnotherToolWaitInput) {
      //      setVisible(false);
      QWidget::close();
      DD("before return");
      return;
    }
    else mIsAnotherToolWaitInput = true;
    mPreventToUseTwoToolsOnSameInput = true;
    // Setup the UI in a new widget

    mWidgetForTab = new QWidget(this); 
                                // <-- try to set mToolWidget instead of this ? NO  (loop)
                                // <-- try to set parent instead of this ? NO, change nothing

    QVBoxLayout * verticalLayout = new QVBoxLayout;//(mWidgetForTab);
    verticalLayout->addWidget(mToolInputSelectionWidget);
    verticalLayout->addWidget(mToolWidget);
    verticalLayout->setContentsMargins(1, 1, 1, 1);
    mWidgetForTab->setLayout(verticalLayout);
    DD(mWidgetForTab->isVisible());
    mWidgetForTab->setVisible(true);
    DD(mWidgetForTab->isVisible());

    // Is this the first time we add a tab ? 
    if (parent->GetTab()->widget(mTabNumber) == NULL) { // Yes, create main widget
      DD("Create main widget");
      mStaticWidgetForTab = new QWidget(parent->GetTab());//parent, f); 
                                // <-- try to set mToolWidget instead of nothing ? NO loop
                                // <-- try to set parent->GetTab() instead of nothing ? 
      mStaticVerticalLayout = new QVBoxLayout;//(mStaticWidgetForTab);
      mStaticWidgetForTab->setLayout(mStaticVerticalLayout);

      /*mWidgetForTab = new QWidget(mStaticWidgetForTab);
    QVBoxLayout * verticalLayout = new QVBoxLayout;//(mWidgetForTab);
    verticalLayout->addWidget(mToolInputSelectionWidget);
    verticalLayout->addWidget(mToolWidget);
    verticalLayout->setContentsMargins(1, 1, 1, 1);
    mWidgetForTab->setLayout(verticalLayout);
    DD(mWidgetForTab->isVisible());
    mWidgetForTab->setVisible(true);
    DD(mWidgetForTab->isVisible());*/

      //<----------      mStaticVerticalLayout->addWidget(mWidgetForTab); 

      mTabNumber = parent->GetTab()->addTab(mStaticWidgetForTab, "");
      DD(mStaticWidgetForTab->isVisible());
      mStaticWidgetForTab->setVisible(true);
      DD(mStaticWidgetForTab->isVisible());
      //      mWidgetForTab->setParent(mStaticWidgetForTab);
    }
    else {
      DD("insert into widget");
      mStaticVerticalLayout->addWidget(mWidgetForTab);
      SwapCurrentWidget();
      mToolWidget->setEnabled(true);
    }
    parent->GetTab()->setCurrentIndex(mTabNumber); 
    buttonBox->hide(); // No OK/Cancel by default in this case
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::Initialization() 
{
  mCurrentSlicerManager = 0;
  mIsInitialized = false;
  mFilter = 0;
  setWindowModality(Qt::NonModal);
  // GUI Initialization
  setupUi(this); ///////////////////////////// TRIAL
  // Connect signals & slots
  connect(mMainWindow, SIGNAL(AnImageIsBeingClosed(vvSlicerManager*)), this, SLOT(AnImageIsBeingClosed(vvSlicerManager*)));
  connect(mMainWindow, SIGNAL(SelectedImageHasChanged(vvSlicerManager*)), this, SLOT(SelectedImageHasChanged(vvSlicerManager*)));
  connect(mToolInputSelectionWidget, SIGNAL(accepted()), this, SLOT(InputIsSelected()));
  connect(mToolInputSelectionWidget, SIGNAL(rejected()), this, SLOT(close()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(apply()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

  // Disable main widget while input image is not selected
  mToolWidget->setEnabled(false);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolWidgetBase::~vvToolWidgetBase()
{
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::keyPressEvent(QKeyEvent *event) 
{
 //  DD("KEYPRESSEVENT");
  if (event->key() == Qt::Key_Escape) {
    reject();
    event->accept();
    return;
  } 
  else {
    QWidget::keyPressEvent(event);
  }
  //  event->ignore();
  //mMainWindow->keyPressEvent(event);
  // QWidget::keyPressEvent(event);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::accept()
{
  apply();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::reject()
{
  close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::AddInputSelector(QString s, clitk::ImageToImageGenericFilterBase * f, bool allowSkip)
{
  int j=0;
  mFilter = f;
  mSlicerManagersCompatible.clear();
  mToolInputSelectionWidget->setToolTip(QString("%1").arg(mFilter->GetAvailableImageTypes().c_str()));
  for(unsigned int i=0; i<mMainWindow->GetSlicerManagers().size(); i++) {
    vvImage * s = mMainWindow->GetSlicerManagers()[i]->GetImage();
    if (mFilter->CheckImageType(s->GetNumberOfDimensions(),
                                s->GetNumberOfScalarComponents(),
                                s->GetScalarTypeAsITKString())) {
      mSlicerManagersCompatible.push_back(mMainWindow->GetSlicerManagers()[i]);
      if ((int)i == mMainWindow->GetSlicerManagerCurrentIndex()) mCurrentCompatibleIndex = j;
      j++;
    }
  }
  if (mSlicerManagersCompatible.size() == 0) {
    std::ostringstream osstream;
    osstream << "Sorry, could not perform operation. No (compatible) image. "
             << mFilter->GetAvailableImageTypes();
    QMessageBox::information(this, "No image", osstream.str().c_str());
    reject();
    return;
  }
  if (mPreventToUseTwoToolsOnSameInput) {
    CheckInputList(mSlicerManagersCompatible, mCurrentCompatibleIndex);
    if (mSlicerManagersCompatible.size() == 0) {
      QMessageBox::information(mMainWindow, tr("Error"), "Sorry, no other loaded images can use this tool. Abort");
      reject();
      return;
    }
  }
  mToolInputSelectionWidget->AddInputSelector(s, mSlicerManagersCompatible, mCurrentCompatibleIndex, allowSkip);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::AddInputSelector(QString s, bool allowSkip)
{
  mSlicerManagersCompatible.clear();
  for(unsigned int i=0; i<mMainWindow->GetSlicerManagers().size(); i++) {
    mSlicerManagersCompatible.push_back(mMainWindow->GetSlicerManagers()[i]);
  }
  if (mMainWindow->GetSlicerManagers().size() == 0) {
    QMessageBox::information(this, "No image","Sorry, could not perform operation. No (compatible) image.");
    close();
    return;
  }
  mCurrentCompatibleIndex = mMainWindow->GetSlicerManagerCurrentIndex();
  if (mPreventToUseTwoToolsOnSameInput) {
    CheckInputList(mSlicerManagersCompatible,  mCurrentCompatibleIndex);
    if (mSlicerManagersCompatible.size() == 0) {
      QMessageBox::information(mMainWindow, tr("Error"), "Sorry, no other loaded images can use this tool. Abort");
      close();
      return;
    }
  }
  mToolInputSelectionWidget->AddInputSelector(s, mSlicerManagersCompatible, mCurrentCompatibleIndex, allowSkip);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::HideInputSelector()
{
  QList<int> s;
  s.push_back(0);
  s.push_back(1);
  splitter->setSizes(s);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::show()
{
  if (!mIsInitialized) {
    mToolInputSelectionWidget->Initialize();
    mIsInitialized = true;
  }
  QWidget::show();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::closeEvent(QCloseEvent *event) 
{
 //  DD("closeEvent");
  mIsAnotherToolWaitInput = false;
  if (isWindow()) {
    event->accept();//return QWidget::close();
    return;
  }
  else {
    if (!mStaticWidgetForTab) {
      event->accept();//return QWidget::close();
      return;
    }
    mStaticVerticalLayout->removeWidget(mWidgetForTab);
    mWidgetForTab->close();
    delete mWidgetForTab;
    QList<QObject*> l =mStaticWidgetForTab->children(); 
    if (l.size() > 1) {
      QWidget * c = dynamic_cast<QWidget*>(l[1]);
      c->setVisible(true);
    }
  }
  event->accept();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolWidgetBase::close()
{
  // DD("vvToolWidgetBase::close()");
  QApplication::restoreOverrideCursor();
  return QWidget::close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::AnImageIsBeingClosed(vvSlicerManager * m)
{
  mToolInputSelectionWidget->AnImageIsBeingClosed(m);
  if (m == mCurrentSlicerManager) {
    close();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::SwapCurrentWidget()
{
  mStaticWidgetForTab->setUpdatesEnabled(false);
  QList<QObject*> l =mStaticWidgetForTab->children(); 
  for(int i=1; i<l.size(); i++) {
    QWidget * c = dynamic_cast<QWidget*>(l[i]);
    if (l[i] == mWidgetForTab) {
      c->setVisible(true);
    }
    else {
      c->setVisible(false);
    }
  } 
  mStaticWidgetForTab->setUpdatesEnabled(true);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::SelectedImageHasChanged(vvSlicerManager * m)
{
  if (!isWindow()) { // When the tool is not in a window, it is in a tab : we only display if needed
    if (mCurrentSlicerManager == NULL) return;
    if (mToolWidget == NULL) return;
    if (m != mCurrentSlicerManager) { // current tool is not selected
      mToolWidget->setEnabled(false);
    }
    else { // The current tool is selected
      SwapCurrentWidget();
      mToolWidget->setEnabled(true);
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::InitializeInputs()
{
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::InputIsSelected()
{
  //  DD("vvToolWidgetBase::InputIsSelected()");
  //DD(mWidgetForTab->isVisible());
  //DD(mToolWidget->isVisible());
  //DD(mStaticWidgetForTab->isVisible());

  buttonBox->setEnabled(true);
  std::vector<vvSlicerManager*> & l = mToolInputSelectionWidget->GetSelectedInputs();
  mCurrentSlicerManager = l[0];
  mCurrentImage = mCurrentSlicerManager->GetImage();
  mToolWidget->setEnabled(true);
  if (!mCurrentSlicerManager) close();
  if (l.size() == 1) InputIsSelected(mCurrentSlicerManager);
  else InputIsSelected(l);
  mIsAnotherToolWaitInput = false;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::InputIsSelected(vvSlicerManager * m)
{
  std::cerr << "You MUST overwrite this method vvToolWidgetBase::InputIsSelected(vvSlicerManager * m) if you use one single input" << std::endl;
  exit(0);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolWidgetBase::InputIsSelected(std::vector<vvSlicerManager*> & l)
{
  buttonBox->setEnabled(true);
  if (l.size() == 1) InputIsSelected(l[0]);
  else {
    std::cerr << "You MUST overwrite this method vvToolWidgetBase::InputIsSelected(vector<vvSlicerManager *> m) if you use several input" << std::endl;
    exit(0);
  }
}
//------------------------------------------------------------------------------
