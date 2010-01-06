/*=========================================================================

Program:   vv
Module:    $RCSfile: vvQProgressDialogITKCommand.cxx,v $
Language:  C++
Date:      $Date: 2010/01/06 13:31:57 $
Version:   $Revision: 1.1 $
Author :   Pierre Seroul (pierre.seroul@gmail.com)

Copyright (C) 2008
Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
CREATIS-LRMN http://www.creatis.insa-lyon.fr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================*/
#ifndef VVQPROGRESSDIALOGITKCOMMAND_CXX
#define VVQPROGRESSDIALOGITKCOMMAND_CXX

#include "clitkIOCommon.h"
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

