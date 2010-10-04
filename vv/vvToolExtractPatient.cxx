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
#include "vvToolExtractPatient.h"
#include "vvToolStructureSetManager.h"
#include "vvSlicer.h"
#include "vvImageReader.h"
#include "vvImageWriter.h"
#include "vvLabelImageLoaderWidget.h"
#include "vvThreadedFilter.h"

// Qt
#include <QMessageBox>

//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolExtractPatient);
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolExtractPatient::vvToolExtractPatient(vvMainWindowBase * parent, Qt::WindowFlags f):
  vvToolWidgetBase(parent,f),
  vvToolBase<vvToolExtractPatient>(parent),
  Ui::vvToolExtractPatient()
{
  // GUI
  Ui_vvToolExtractPatient::setupUi(mToolWidget);
  mOptionsBox->setEnabled(true);
  //connect(mPatientMaskInputWidget, SIGNAL(accepted()), this, SLOT(PatientMaskInputIsSelected()));

  // Default values
  mArgsInfo = new ArgsInfoType;
  cmdline_parser_clitkExtractPatient_init(mArgsInfo);
  SetGUIFromArgsInfo();
  m_IsThreadInterrupted = false;

  // Create a new ExtractPatient filter
  mFilter = new FilterType;  // used in AddInputSelector

  // Add input selector
  AddInputSelector("Select CT thorax image", mFilter);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolExtractPatient::~vvToolExtractPatient()
{
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolExtractPatient::Initialize() {
  SetToolName("ExtractPatient");
  SetToolMenuName("Extract Patient");
  SetToolIconFilename(":/common/icons/Patient-overlay.png");
  SetToolTip("Extract Patient mask from thorax CT.");
  SetToolExperimental(true);
  //  SetToolInMenu("Segmentation");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolExtractPatient::InputIsSelected(vvSlicerManager *m)
{
  // Hide selector
  HideInputSelector(); // splitter
  mToolInputSelectionWidget->hide();
  mCurrentSlicerManager = m;
  mCurrentImage = m->GetImage();
  mLabelInput->setText(m->GetFileName().c_str());
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void vvToolExtractPatient::PatientMaskInputIsSelected()
// {
//   // Get Patient mask and BG value
//   mPatient = mPatientMaskInputWidget->GetImage();  
//   mPatientBackgroundValue = mPatientMaskInputWidget->GetBackgroundValue();
  
//   // Check patient dimension
//   if (mPatient->GetNumberOfDimensions() != 3) {
//     QMessageBox::information(this,tr("*Error*"), "Mask image must be 3D");
//     return;
//   }
   
//   mMaskLoaderBox->setEnabled(false);
//   mOptionsBox->setEnabled(true);
// }
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolExtractPatient::SetGUIFromArgsInfo() 
{
  /*
  mAirUpperThresholdSlider->SetText("Upper threshold for air");
  mAirUpperThresholdSlider->SetMinimum(-1200);
  mAirUpperThresholdSlider->SetMaximum(2000);
  DD(mArgsInfo->upper_arg);
  mAirUpperThresholdSlider->SetValue(mArgsInfo->upper_arg);

  mAirLowerThresholdSlider->SetText("Lower threshold for air");
  mAirLowerThresholdSlider->SetMinimum(-1200);
  mAirLowerThresholdSlider->SetMaximum(2000);
  mAirLowerThresholdSlider->SetValue(mArgsInfo->lower_arg);
  */

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolExtractPatient::GetArgsInfoFromGUI() 
{
  //mArgsInfo->patientBG_arg = mPatientMaskInputWidget->GetBackgroundValue();
  mArgsInfo->verboseOption_flag = true; // DEBUG. TO CHANGE
  mArgsInfo->verboseStep_flag = true; // DEBUG. TO CHANGE
  mArgsInfo->writeStep_flag = false;
  mArgsInfo->input_given = 0;
  //mArgsInfo->patient_given = 0;
  mArgsInfo->output_given = 0;
  //mArgsInfo->outputTrachea_given = 0;
  //mArgsInfo->remove1_given = 0;
  
 //  mArgsInfo->upper_arg = mAirUpperThresholdSlider->GetValue();
//   mArgsInfo->lower_arg = mAirLowerThresholdSlider->GetValue();
//   if (mRadioButtonLowerThan->isChecked()) mArgsInfo->lower_given = 1;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolExtractPatient::apply() 
{
  // Change cursor to wait
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  
  // Read options from GUI and put it in the ArgsInfo struct
  GetArgsInfoFromGUI();
  
  // Check options
  // if (mArgsInfo->lower_given) {
//     if (mArgsInfo->lower_arg > mArgsInfo->upper_arg) {    
//       QApplication::restoreOverrideCursor(); 
//       QMessageBox::information(this,tr("Error"), "Lower threshold cannot be greater than upper threshold.");
//       return;
//     }
//   }

  // Create new filter
  //if (mFilter) delete mFilter;
  DD("new filter");
  mFilter = new FilterType;  // needed when thread cancel the filter
  // mFilter->StopOnErrorOff();
  //  mFilter->SetIOVerbose(true);
  mFilter->AddInputVVImage(mCurrentImage); // CT
//   mFilter->AddInputVVImage(mPatient); // patient mask
  mFilter->SetArgsInfo(*mArgsInfo);

  // Created threaded execution
  DD("thread");
  vvThreadedFilter thread;
  connect(&thread, SIGNAL(ThreadInterrupted()), this, SLOT(ThreadInterrupted()));
  thread.SetFilter(mFilter);

  try {
  thread.Update();
  }
  catch(std::runtime_error e) {

  // Check if the thread has been canceled. In this case, return
  if (m_IsThreadInterrupted) {
    m_IsThreadInterrupted = false;
    QApplication::restoreOverrideCursor(); 
    return;
  }
  disconnect(&thread, SIGNAL(ThreadInterrupted()), this, SLOT(ThreadInterrupted()));
    
  // Check error during filter
  // if (mFilter->HasError()) {
    QApplication::restoreOverrideCursor();
    QMessageBox::information(this,tr("Error"), e.what());//mFilter->GetLastError().c_str());
    return;
  // }

  }
  
  // Get output
  std::vector<vvImage::Pointer> output = mFilter->GetOutputVVImages();
  if (output.size() == 0) {
    std::cerr << "Error : no output ?" << std::endl;
    QApplication::restoreOverrideCursor();
    close();
    return;
  }
  
  // Set Patient into VV
  vvImage::Pointer Patient = output[0];
  std::ostringstream osstream;
  osstream << "Patient_" << mCurrentSlicerManager->GetSlicer(0)->GetFileName() << ".mhd";
  vvSlicerManager * v = AddImage(Patient,osstream.str());
  v->SetPreset(5);
  vvToolStructureSetManager::AddImage(mCurrentSlicerManager, "Right Patient", Patient, 1, false); // Right is greater than Left
//   vvToolStructureSetManager::AddImage(mCurrentSlicerManager, "Left Patient", Patient, 2, false);  

 //  // Set trachea into VV
//   if (output.size() == 2) {
//     vvImage::Pointer trachea = output[1];
//     std::ostringstream osstream;
//     osstream << "Trachea_" << mCurrentSlicerManager->GetSlicer(0)->GetFileName() << ".mhd";
//     vvSlicerManager * v = AddImage(trachea,osstream.str());
//     v->SetPreset(5);
//     vvToolStructureSetManager::AddImage(mCurrentSlicerManager, "Trachea", trachea, 0);
//   }

  // End
  QApplication::restoreOverrideCursor();
  close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolExtractPatient::close()
{
  return vvToolWidgetBase::close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolExtractPatient::ThreadInterrupted() 
{ 
  m_IsThreadInterrupted = true; 
}
//------------------------------------------------------------------------------

