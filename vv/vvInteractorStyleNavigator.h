#ifndef __vvInteractorStyleNavigator_h
#define __vvInteractorStyleNavigator_h
#include "vtkInteractorStyle.h"

// Motion flags

#define VTKIS_WINDOW_LEVEL 1024
#define VTKIS_PICK         1025

class vvInteractorStyleNavigator : public vtkInteractorStyle

{
public:
    static vvInteractorStyleNavigator *New();
    vtkTypeRevisionMacro(vvInteractorStyleNavigator, vtkInteractorStyle);
    void PrintSelf(ostream& os, vtkIndent indent);

    // Description:
    // Some useful information for handling window level
    vtkGetVector2Macro(WindowLevelStartPosition,int);
    vtkGetVector2Macro(WindowLevelCurrentPosition,int);

    // Description:
    // Event bindings controlling the effects of pressing mouse buttons
    // or moving the mouse.
    virtual void OnMouseMove();
    virtual void OnLeftButtonDown();
    virtual void OnLeftButtonUp();
    virtual void OnRightButtonDown();
    virtual void OnRightButtonUp();
    virtual void OnMiddleButtonDown();
    virtual void OnMiddleButtonUp();
    virtual void OnEnter();
    virtual void OnLeave();
    virtual void OnMouseWheelForward();
    virtual void OnMouseWheelBackward();

    // Description:
    // Override the "fly-to" (f keypress) for images.
    virtual void OnChar();

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
    virtual void Dolly();
    virtual void Pan();

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
