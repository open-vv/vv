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
#include "vvToolExtractLung.h"
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
ADD_TOOL(vvToolExtractLung);
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolExtractLung::vvToolExtractLung(vvMainWindowBase * parent, Qt::WindowFlags f):
  vvToolWidgetBase(parent,f),
  vvToolBase<vvToolExtractLung>(parent),
  Ui::vvToolExtractLung()
{
  // GUI
  Ui_vvToolExtractLung::setupUi(mToolWidget);
  mMaskLoaderBox->setEnabled(false);
  mOptionsBox->setEnabled(false);
  mPatientMaskInputWidget->SetText("Patient mask");
  connect(mPatientMaskInputWidget, SIGNAL(accepted()), this, SLOT(PatientMaskInputIsSelected()));

  // Default values
  mArgsInfo = new ArgsInfoType;
  cmdline_parser_clitkExtractLung_init(mArgsInfo);
  SetGUIFromArgsInfo();
  m_IsThreadInterrupted = false;

  // Create a new ExtractLung filter
  mFilter = new FilterType;  // used in AddInputSelector

  // Add input selector
  AddInputSelector("Select CT thorax image", mFilter);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolExtractLung::~vvToolExtractLung()
{
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolExtractLung::Initialize() {
  SetToolName("ExtractLung");
  SetToolMenuName("Extract lungs");
  SetToolIconFilename(":/common/icons/lung-overlay.png");
  SetToolTip("Extract lung mask from thorax CT.");
  SetToolExperimental(true);
  //  SetToolInMenu("Segmentation");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolExtractLung::InputIsSelected(vvSlicerManager *m)
{
  // Hide selector
  HideInputSelector(); // splitter
  mToolInputSelectionWidget->hide();
  mCurrentSlicerManager = m;
  mCurrentImage = m->GetImage();
  mMaskLoaderBox->setEnabled(true);
  mLabelInput->setText(m->GetFileName().c_str());
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolExtractLung::PatientMaskInputIsSelected()
{
  // Get Patient mask and BG value
  mPatient = mPatientMaskInputWidget->GetImage();  
  mPatientBackgroundValue = mPatientMaskInputWidget->GetBackgroundValue();
  
  // Check patient dimension
  if (mPatient->GetNumberOfDimensions() != 3) {
    QMessageBox::information(this,tr("*Error*"), "Mask image must be 3D");
    return;
  }
   
  mMaskLoaderBox->setEnabled(false);
  mOptionsBox->setEnabled(true);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolExtractLung::SetGUIFromArgsInfo() 
{
  mAirUpperThresholdSlider->SetText("Upper threshold for air");
  mAirUpperThresholdSlider->SetMinimum(-1200);
  mAirUpperThresholdSlider->SetMaximum(2000);
  DD(mArgsInfo->upper_arg);
  mAirUpperThresholdSlider->SetValue(mArgsInfo->upper_arg);

  mAirLowerThresholdSlider->SetText("Lower threshold for air");
  mAirLowerThresholdSlider->SetMinimum(-1200);
  mAirLowerThresholdSlider->SetMaximum(2000);
  mAirLowerThresholdSlider->SetValue(mArgsInfo->lower_arg);

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolExtractLung::GetArgsInfoFromGUI() 
{
  // mArgsInfo->patientBG_arg = mPatientMaskInputWidget->GetBackgroundValue();
  mArgsInfo->verboseOption_flag = true; // DEBUG. TO CHANGE
  mArgsInfo->verboseStep_flag = true; // DEBUG. TO CHANGE
  mArgsInfo->writeStep_flag = false;
  mArgsInfo->input_given = 0;
  // mArgsInfo->patient_given = 0;
  mArgsInfo->output_given = 0;
  mArgsInfo->outputTrachea_given = 0;
  mArgsInfo->remove1_given = 0;
  
  mArgsInfo->upper_arg = mAirUpperThresholdSlider->GetValue();
  mArgsInfo->lower_arg = mAirLowerThresholdSlider->GetValue();
  if (mRadioButtonLowerThan->isChecked()) mArgsInfo->lower_given = 1;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolExtractLung::apply() 
{
  // Change cursor to wait
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  
  // Read options from GUI and put it in the ArgsInfo struct
  GetArgsInfoFromGUI();
  
  // Check options
  if (mArgsInfo->lower_given) {
    if (mArgsInfo->lower_arg > mArgsInfo->upper_arg) {    
      QApplication::restoreOverrideCursor(); 
      QMessageBox::information(this,tr("Error"), "Lower threshold cannot be greater than upper threshold.");
      return;
    }
  }

  // Create new filter
  if (mFilter) delete mFilter;
  mFilter = new FilterType;  // needed when thread cancel the filter
  // mFilter->StopOnErrorOff();
  //  mFilter->SetIOVerbose(true);
  mFilter->AddInputVVImage(mCurrentImage); // CT
  mFilter->AddInputVVImage(mPatient); // patient mask
  mFilter->SetArgsInfo(*mArgsInfo);

  // Created threaded execution
  vvThreadedFilter thread;
  connect(&thread, SIGNAL(ThreadInterrupted()), this, SLOT(ThreadInterrupted()));
  thread.SetFilter(mFilter);

  try {
    thread.Update();
  }
  catch(std::runtime_error e) {
    DD("Error exception handling");
    DD(m_IsThreadInterrupted);
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
  } // end exception
  
  // Get output
  std::vector<vvImage::Pointer> output = mFilter->GetOutputVVImages();
  if (output.size() == 0) {
    std::cerr << "Error : no output ?" << std::endl;
    QApplication::restoreOverrideCursor();
    close();
    return;
  }
  
  // Set Lung into VV
  vvImage::Pointer lung = output[0];
  std::ostringstream osstream;
  osstream << "Lung_" << mCurrentSlicerManager->GetSlicer(0)->GetFileName() << ".mhd";
  vvSlicerManager * v = AddImage(lung,osstream.str());
  v->SetPreset(5);
  vvToolStructureSetManager::AddImage(mCurrentSlicerManager, "Right lung", lung, 1, false); // Right is greater than Left
  vvToolStructureSetManager::AddImage(mCurrentSlicerManager, "Left lung", lung, 2, false);  

  // Set trachea into VV
  if (output.size() == 2) {
    vvImage::Pointer trachea = output[1];
    std::ostringstream osstream;
    osstream << "Trachea_" << mCurrentSlicerManager->GetSlicer(0)->GetFileName() << ".mhd";
    vvSlicerManager * v = AddImage(trachea,osstream.str());
    v->SetPreset(5);
    vvToolStructureSetManager::AddImage(mCurrentSlicerManager, "Trachea", trachea, 0);
  }

  // End
  QApplication::restoreOverrideCursor();
  close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolExtractLung::close()
{
  return vvToolWidgetBase::close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolExtractLung::ThreadInterrupted() 
{ 
  m_IsThreadInterrupted = true; 
}
//------------------------------------------------------------------------------

