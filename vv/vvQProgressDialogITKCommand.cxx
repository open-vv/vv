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
#ifndef VVQPROGRESSDIALOGITKCOMMAND_CXX
#define VVQPROGRESSDIALOGITKCOMMAND_CXX
#include "clitkImageCommon.h"
#include "vvQProgressDialogITKCommand.h"

//====================================================================
vvQProgressDialogITKCommand::vvQProgressDialogITKCommand() {
    progress.setWindowModality(Qt::WindowModal);
    progress.setCancelButtonText("Cancel");
}
//====================================================================

//====================================================================
void vvQProgressDialogITKCommand::Initialize(QString title, float sec, int max) {
    progress.setMinimumDuration((int)lrint(1000.0*sec)); // number of seconds to wait before displaying dialog
    progress.setLabelText(title);
    i=0;
    progress.setMaximum(max);
}
//====================================================================

//====================================================================
void vvQProgressDialogITKCommand::Execute(itk::Object *caller, const itk::EventObject & event) {
    i++;
    progress.setValue(i);
    if (progress.wasCanceled()) {
        itk::ProcessObject * o = dynamic_cast<itk::ProcessObject *>(caller);
        o->SetAbortGenerateData(true);
    }
}
//====================================================================

//====================================================================
void vvQProgressDialogITKCommand::Execute(const itk::Object *caller, const itk::EventObject & event) {
    i++;
    progress.setValue(i);
}
//====================================================================

#endif /* end #define VVQPROGRESSDIALOGITKCOMMAND_CXX */

