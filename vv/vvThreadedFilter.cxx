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
    // try {
    m_FilterBase = m_Filter->GetFilterBase(); // get filterbase is only set after Update
    if (m_FilterBase != NULL) {
      progress.SetProgress(m_FilterBase->GetCurrentStepNumber(), 
			   m_FilterBase->GetNumberOfSteps());
      if (temp != m_FilterBase->GetCurrentStepName()) {
	progress.AddToText(m_FilterBase->GetCurrentStepName());
      }
      temp = m_FilterBase->GetCurrentStepName();
    }
    this->wait(200); // in miliseconds
    qApp->processEvents();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvThreadedFilter::run()
{
  assert(m_Filter != NULL);
  try {
    m_Filter->Update();
  }
  catch(clitk::ExceptionObject e) {
    DD("vvThreadedFilter : exceptionobject handeled");
    DD(e.what());
    QApplication::restoreOverrideCursor();
    QMessageBox::information(new QWidget, tr("Error"), e.what());  
  }
  DD("end RUN");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvThreadedFilter::reject()
{
  // First, say the filter it must stop as soon as possible. We then
  // wait that an exception occur in the main thread.
  if (m_FilterBase != NULL) {
    m_FilterBase->Cancel();
  }
}
//------------------------------------------------------------------------------




