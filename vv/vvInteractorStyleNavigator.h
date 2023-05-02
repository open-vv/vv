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
#ifndef __vvInteractorStyleNavigator_h
#define __vvInteractorStyleNavigator_h
#include "vtkInteractorStyle.h"
#include <vtkVersion.h>

// Motion flags

#define VTKIS_WINDOW_LEVEL 1024
#define VTKIS_PICK_NEW     1025

class vvInteractorStyleNavigator : public vtkInteractorStyle

{
public:
    static vvInteractorStyleNavigator *New();
    vtkTypeMacro(vvInteractorStyleNavigator, vtkInteractorStyle);
#if VTK_MAJOR_VERSION >= 8
    void PrintSelf(ostream& os, vtkIndent indent) override;
#else
    void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;
#endif

    // Description:
    // Some useful information for handling window level
    vtkGetVector2Macro(WindowLevelStartPosition,int);
    vtkGetVector2Macro(WindowLevelCurrentPosition,int);

    // Description:
    // Event bindings controlling the effects of pressing mouse buttons
    // or moving the mouse.
#if VTK_MAJOR_VERSION >= 8
    virtual void OnMouseMove() override;
    virtual void OnLeftButtonDown() override;
    virtual void OnLeftButtonUp() override;
    virtual void OnRightButtonDown() override;
    virtual void OnRightButtonUp() override;
    virtual void OnMiddleButtonDown() override;
    virtual void OnMiddleButtonUp() override;
    virtual void OnEnter() override;
    virtual void OnLeave() override;
    virtual void OnMouseWheelForward() override;
    virtual void OnMouseWheelBackward() override;
#else
    virtual void OnMouseMove() VTK_OVERRIDE;
    virtual void OnLeftButtonDown() VTK_OVERRIDE;
    virtual void OnLeftButtonUp() VTK_OVERRIDE;
    virtual void OnRightButtonDown() VTK_OVERRIDE;
    virtual void OnRightButtonUp() VTK_OVERRIDE;
    virtual void OnMiddleButtonDown() VTK_OVERRIDE;
    virtual void OnMiddleButtonUp() VTK_OVERRIDE;
    virtual void OnEnter() VTK_OVERRIDE;
    virtual void OnLeave() VTK_OVERRIDE;
    virtual void OnMouseWheelForward() VTK_OVERRIDE;
    virtual void OnMouseWheelBackward() VTK_OVERRIDE;
#endif

    // Description:
    // Override the "fly-to" (f keypress) for images.
#if VTK_MAJOR_VERSION >= 8
    virtual void OnChar() override;
#else
    virtual void OnChar() VTK_OVERRIDE;
#endif

    // These methods for the different interactions in different modes
    // are overridden in subclasses to perform the correct motion. Since
    // they might be called from OnTimer, they do not have mouse coord parameters
    // (use interactor's GetEventPosition and GetLastEventPosition)
    virtual void WindowLevel();
    virtual void Pick();

    // Interaction mode entry points used internally.
    virtual void StartWindowLevel();
    virtual void EndWindowLevel();
    virtual void StartPick();
    virtual void EndPick();
#if VTK_MAJOR_VERSION >= 8
    virtual void Dolly() override;
    virtual void Pan() override;
#else
    virtual void Dolly() VTK_OVERRIDE;
    virtual void Pan() VTK_OVERRIDE;
#endif

    // We need to reimplement this because otherwise it returns the top renderer,
    // not the active one
    virtual void FindPokedRenderer(int, int);

protected:
    vvInteractorStyleNavigator();
    ~vvInteractorStyleNavigator();

    static void ProcessEvents(vtkObject* object,
                              unsigned long event,
                              void* clientdata,
                              void* calldata);

    double MotionFactor;

    virtual void Dolly(double factor);

    int WindowLevelStartPosition[2];
    int WindowLevelCurrentPosition[2];

private:
    vvInteractorStyleNavigator(const vvInteractorStyleNavigator&);  // Not implemented.
    void operator=(const vvInteractorStyleNavigator&);  // Not implemented.
};

#endif
