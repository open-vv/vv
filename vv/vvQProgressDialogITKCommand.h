#ifndef VVQPROGRESSDIALOGITKCOMMAND_H
#define VVQPROGRESSDIALOGITKCOMMAND_H
#include "itkCommand.h"

// qt include
#include <QProgressDialog>

class vvQProgressDialogITKCommand : public itk::Command {

public:
    typedef vvQProgressDialogITKCommand    Self;
    typedef itk::Command                   Superclass;
    typedef itk::SmartPointer<Self>        Pointer;

    itkNewMacro(Self);

    void Initialize(QString title, float sec, int max);

    void Execute(itk::Object *caller, const itk::EventObject & event);
    void Execute(const itk::Object *caller, const itk::EventObject & event);

protected:
    vvQProgressDialogITKCommand();
    QProgressDialog progress;
    int i;

}; // end class vvQProgressDialogITKCommand

#endif /* end #define VVQPROGRESSDIALOGITKCOMMAND_H */

