/*=========================================================================

 Program:   vv
 Language:  C++
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
