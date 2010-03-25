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

