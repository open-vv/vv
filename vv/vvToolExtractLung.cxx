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
#include "vvProgressDialog.h"

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
  connect(mPatientMaskInputWidget, SIGNAL(accepted()), this, SLOT(PatientMaskInputIsSelected()));

  // Default values
  cmdline_parser_clitkExtractLung_init(&mArgsInfo);

  // Create a new ExtractLung filter
  mFilter = FilterType::New();  

  // Add input selector
  AddInputSelector("Select image", mFilter);
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
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolExtractLung::InputIsSelected(vvSlicerManager *m)
{
  DD("InputIsSelected");
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
  DD("PatientMaskInputIsSelected");
  mMaskLoaderBox->setEnabled(false);
  mOptionsBox->setEnabled(true);

  // Get Patient mask and BG value
  mPatient = mPatientMaskInputWidget->GetImage();  
  mPatientBackgroundValue = mPatientMaskInputWidget->GetBackgroundValue();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolExtractLung::GetArgsInfoFromGUI() 
{
  DD("GetArgsInfoFromGUI");
  mArgsInfo.patientBG_arg = 0; //mPatientBackgroundValueSpinBox->value();
  mArgsInfo.verboseOption_flag = true;
  mArgsInfo.verboseStep_flag = true;
  mArgsInfo.writeStep_flag = false;
  mArgsInfo.input_given = 0;
  mArgsInfo.patient_given = 0;
  mArgsInfo.output_given = 0;
  mArgsInfo.outputTrachea_given = 0;
  mArgsInfo.remove1_given = 0;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolExtractLung::apply() 
{
  DD("apply");
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  
  GetArgsInfoFromGUI();

  mFilter->SetIOVerbose(true);  
  mFilter->AddInputVVImage(mCurrentImage); // CT
  mFilter->AddInputVVImage(mPatient); // patient mask
  mFilter->SetArgsInfo(mArgsInfo);

  DD("mfilter->Update");
  // vvThreadedFilter a;
  // a->SetFilter(mFilter)
  // connect(a, SIGNAL(rejected()), this, SLOT(FilterHasBeenCanceled()));
  // a->Update();
  // if (a->HasError()) { DD(a->GetError()); return; }
  mFilter->Update();

  // Check error
  if (mFilter->HasError()) {
    QMessageBox::information(this,tr("*Error* while finding lung"), mFilter->GetLastError().c_str());
    reject();
    return;
  }
  
  // Get output
  std::vector<vvImage::Pointer> output = mFilter->GetOutputVVImages();
  DD(output.size());
  
  // Set Lung into VV
  DD("lung");
  vvImage::Pointer lung = output[0];
  std::ostringstream osstream;
  osstream << "Lung_" << mCurrentSlicerManager->GetSlicer(0)->GetFileName() << ".mhd";
  vvSlicerManager * v = AddImage(lung,osstream.str());
  v->SetPreset(5);
  vvToolStructureSetManager::AddImage(mCurrentSlicerManager, "Right lung", lung, 1, false); // Right is greater than Left
  vvToolStructureSetManager::AddImage(mCurrentSlicerManager, "Left lung", lung, 2, false);  

  // Set trachea into VV
  if (output.size() == 2) {
    DD("trachea");
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


