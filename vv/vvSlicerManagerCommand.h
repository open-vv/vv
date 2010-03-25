#ifndef __vvSlicerManagerCommand_h
#define __vvSlicerManagerCommand_h
#include "vtkCommand.h"
#include "vvSlicerManager.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkImageData.h"
#include "vtkImageActor.h"
#include "vvInteractorStyleNavigator.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkPropPicker.h"

#include <iostream>
#include <string>
#include <sstream>

class vvSlicerManagerCommand : public vtkCommand
{
public:
    static vvSlicerManagerCommand *New() {
        return new vvSlicerManagerCommand;
    }

    void Execute(vtkObject *caller,
                 unsigned long event,
                 void *vtkNotUsed(callData));

    vvSlicerManager *SM;
    void Dolly(double factor, vtkRenderWindowInteractor *interactor);
    void FlyToPosition(vtkRenderWindowInteractor *interactor, vvSlicer* slicer);
    void SetSlicerNumber(int slicer) {mSlicerNumber=slicer;}

protected :
    vvSlicerManagerCommand();
    ~vvSlicerManagerCommand() {}

private:

    int FindSlicerNumber(vtkRenderWindow* renwin);

    double InitialWindow;
    double InitialLevel;
    int mStartSlicer;
    bool newLandmark;
    ///The slicer this callback commands
    int mSlicerNumber;
};

#endif
