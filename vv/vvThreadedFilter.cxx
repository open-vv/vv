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
#include "vvThreadedFilter.h"
#include "vvProgressDialog.h"

// Qt
#include <QMessageBox>

//------------------------------------------------------------------------------
vvThreadedFilter::vvThreadedFilter():
  QThread()
{
  m_Filter = NULL;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvThreadedFilter::~vvThreadedFilter()
{
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvThreadedFilter::SetFilter(clitk::ImageToImageGenericFilterBase * f)
{
  m_Filter = f;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvThreadedFilter::Update()
{
  assert(m_Filter != NULL);

  // Show a progress bar while computing
  vvProgressDialog progress("Computing ...",100);
  progress.SetCancelButtonEnabled(true);
  connect(&progress, SIGNAL(rejected()), this, SLOT(reject()));
  this->start();
  this->setTerminationEnabled(true);
  std::string temp;
  while (this->isRunning()) {
    m_FilterBase = m_Filter->GetFilterBase(); // get filterbase is only set after Update
    if (m_FilterBase != NULL) {
      //      m_FilterBase->StopOnErrorOff(); // filter can be interrupted
      progress.SetProgress(m_FilterBase->GetCurrentStepNumber(), 
			   m_FilterBase->GetNumberOfSteps());
      if (temp != m_FilterBase->GetCurrentStepName()) {
	progress.AddToText(m_FilterBase->GetCurrentStepName());
      }
      temp = m_FilterBase->GetCurrentStepName();
    }
    this->wait(200); // in milisecond
    qApp->processEvents();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvThreadedFilter::run()
{
  assert(m_Filter != NULL);
  m_Filter->Update();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvThreadedFilter::reject()
{
  // First, say the filter it must stop
  if (m_FilterBase != NULL) {
    m_FilterBase->SetMustStop(true);
  }
  // Indicate to the user it will stop
  QApplication::restoreOverrideCursor();
  QMessageBox::information(new QWidget, tr("Error"), m_FilterBase->GetLastError().c_str());  
  // Quit the thread (is it needed ?)
  this->quit();
  emit ThreadInterrupted();
}
//------------------------------------------------------------------------------




