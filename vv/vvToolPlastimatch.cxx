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
#include "vvToolPlastimatch.h"
#include "vvSlicerManager.h"
#include "vvSlicer.h"
#include "vvToolInputSelectorWidget.h"

// Plastimatch 
// PUT INCLUDE HERE

// qt
#include <QMessageBox>

//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolPlastimatch);
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolPlastimatch::Initialize()
{
  SetToolName("Plastimatch");
  SetToolMenuName("Plastimatch");
  SetToolIconFilename(":/common/icons/plastimatch.png");
  SetToolTip("Image registration with Plastimatch (G. Sharp).");
  SetToolExperimental(true);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolPlastimatch::vvToolPlastimatch(vvMainWindowBase * parent, Qt::WindowFlags f)
  :vvToolWidgetBase(parent,f),
   vvToolBase<vvToolPlastimatch>(parent),
   Ui::vvToolPlastimatch()
{
  // GUI Initialization
  Ui_vvToolPlastimatch::setupUi(mToolWidget);

  // Connect signals & slots
  // TODO

  // Set how many inputs are needed for this tool
  AddInputSelector("Select fixed (reference) image");
  AddInputSelector("Select moving image");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolPlastimatch::~vvToolPlastimatch()
{
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolPlastimatch::close()
{
  return vvToolWidgetBase::close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolPlastimatch::closeEvent(QCloseEvent *event) {
  event->accept();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolPlastimatch::reject()
{
  DD("vvToolPlastimatch::reject");
  return vvToolWidgetBase::reject();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolPlastimatch::InputIsSelected(std::vector<vvSlicerManager *> & m)
{
  DD("vvToolPlastimatch::InputIsSelected");

  // Get input images (vvImage)
  m_InputSlicerManagers = m;
  m_Fixed = m[0]->GetImage();
  m_Moving = m[1]->GetImage();

  // Check image
  if (m_Fixed->GetNumberOfDimensions() != 3) {
    QMessageBox::information(this, tr("Error"), tr("Sorry, fixed image should be 3D"));
    close();
    return;
  }
  if (m_Moving->GetNumberOfDimensions() != 3) {
    QMessageBox::information(this, tr("Errror"), tr("Sorry, moving image should be 3D"));
    close();
    return;
  }

  // We cannot used vvImageToITK directly because we need to cast to
  // float before

  // Convert input to float
  m_FixedVTK = m_Fixed->GetFirstVTKImageData();
  m_MovingVTK = m_Moving->GetFirstVTKImageData();
  DD(m_Fixed->GetScalarTypeAsITKString());
  if (m_Fixed->GetScalarTypeAsITKString() != "float") {
    DD("Cast input");
    m_FixedVTK = CopyAndCastToFloatFrom(m_Fixed->GetFirstVTKImageData());
    m_MovingVTK = CopyAndCastToFloatFrom(m_Moving->GetFirstVTKImageData());
  }

  // Convert vtk to itk
  typedef itk::Image<float, 3> FloatImageType;
  m_FixedITK = ItkImageFromVtk<3, float>(m_FixedVTK);
  m_MovingITK = ItkImageFromVtk<3, float>(m_MovingVTK);
  
  m_FixedITK->Print(std::cout);
  m_MovingITK->Print(std::cout);  
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolPlastimatch::GetOptionsFromGUI()
{
  DD("vvToolPlastimatch::GetOptionsFromGUI");


}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolPlastimatch::apply()
{
  DD("vvToolPlastimatch::apply");

  if (!mCurrentSlicerManager) close();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  GetOptionsFromGUI();

  // Create the command string
  /*
  const char *command_string =
    "[STAGE]\n"
    "xform=bspline\n"
    "max_its=30\n"
    "grid_spac=100 100 100\n"
    "res=4 4 2\n"
    ;
    */

  // Prepare the registration
  /*
  Plm_registration_context *prc = plm_registration_context_create ();
  plm_registration_set_fixed (prc, m_FixedITK);
  plm_registration_set_moving (prc, m_MovingITK);
  plm_registration_set_command_string (prc, command_string);


  // Run the registration
  plm_registration_execute (prc);
  if (plm_registration_get_status (prc) != 0) {
    // Handle error 
  }

  // Get registration outputs
  plm_registration_get_warped_image (prc, &m_WarpedImageITK);
  plm_registration_get_vector_field (prc, &m_DeformationField);

  // Free the memory
  plm_registration_context_destroy (prc);
  */

  // Get warped output and display it
  if (m_WarpedImageITK) {
    m_WarpedImage = vvImageFromITK<3, float>(m_WarpedImageITK);
    std::ostringstream osstream;
    osstream << "plm_warped_" << m_InputSlicerManagers[1]->GetFileName() << ".mhd";
    AddImage(m_WarpedImage, osstream.str());
    
    // Get DVF 
    DD("TODO get and display DVF");
  }
  else {
    QMessageBox::information(this, "Error", "No result ...");
  }

  // End
  QApplication::restoreOverrideCursor();
  close();
}
//------------------------------------------------------------------------------


