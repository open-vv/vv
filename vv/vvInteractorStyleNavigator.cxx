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
#include "vvInteractorStyleNavigator.h"

#include "vtkAbstractPropPicker.h"
#include "vtkAssemblyPath.h"
#include "vtkCallbackCommand.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkCamera.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include <vtkRendererCollection.h>
#include "clitkCommon.h"


vtkCxxRevisionMacro(vvInteractorStyleNavigator, "DummyRevision");
vtkStandardNewMacro(vvInteractorStyleNavigator);

//----------------------------------------------------------------------------
vvInteractorStyleNavigator::vvInteractorStyleNavigator()
{
    this->WindowLevelStartPosition[0]   = 0;
    this->WindowLevelStartPosition[1]   = 0;

    this->WindowLevelCurrentPosition[0] = 0;
    this->WindowLevelCurrentPosition[1] = 0;

    this->MotionFactor   = 10.0;
}

//----------------------------------------------------------------------------
vvInteractorStyleNavigator::~vvInteractorStyleNavigator()
{
    CurrentRenderer=NULL;
}

void vvInteractorStyleNavigator::FindPokedRenderer(int dummy1,int dummy2)
{
    vtkRenderWindow * renwin=this->GetInteractor()->GetRenderWindow();
    renwin->GetRenderers()->InitTraversal();
    while (true)
    {
        vtkRenderer* current = renwin->GetRenderers()->GetNextItem();
        if (current==NULL || current->GetDraw())
        {
            CurrentRenderer=current;
            return;
        }
    }
}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::StartWindowLevel()
{
    if (this->State != VTKIS_NONE)
    {
        return;
    }
    this->StartState(VTKIS_WINDOW_LEVEL);
    this->InvokeEvent(vtkCommand::StartWindowLevelEvent,this);
}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::EndWindowLevel()
{
    if (this->State != VTKIS_WINDOW_LEVEL)
    {
        return;
    }
    this->InvokeEvent(vtkCommand::EndWindowLevelEvent, this);
    this->StopState();
}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::StartPick()
{
    if (this->State != VTKIS_NONE)
    {
        return;
    }
    this->StartState(VTKIS_PICK);
    this->InvokeEvent(vtkCommand::StartPickEvent, this);
}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::EndPick()
{
    if (this->State != VTKIS_PICK)
    {
        return;
    }
    this->InvokeEvent(vtkCommand::EndPickEvent, this);
    this->StopState();
}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::OnMouseMove()
{
    int x = this->Interactor->GetEventPosition()[0];
    int y = this->Interactor->GetEventPosition()[1];

    switch (this->State)
    {
    case VTKIS_WINDOW_LEVEL:
        this->FindPokedRenderer(x, y);
        this->WindowLevel();
        this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
        break;

    case VTKIS_PICK:
        this->FindPokedRenderer(x, y);
        this->Pick();
        this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
        break;

    case VTKIS_PAN:
        this->FindPokedRenderer(x, y);
        this->Pan();
        this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
        break;

    case VTKIS_DOLLY:
        this->FindPokedRenderer(x, y);
        this->Dolly();
        this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
        break;

    default:
        this->InvokeEvent(vtkCommand::UserEvent, NULL);
        break;
    }

    // Call parent to handle all other states and perform additional work

}

void vvInteractorStyleNavigator::OnEnter()
{
    //  int x = this->Interactor->GetEventPosition()[0];
    //int y = this->Interactor->GetEventPosition()[1];

    switch (this->State)
    {
    case VTKIS_WINDOW_LEVEL:
        break;

    case VTKIS_PICK:
        break;

    case VTKIS_PAN:
        break;

    default:
        this->InvokeEvent(vtkCommand::EnterEvent, NULL);
        break;
    }

    // Call parent to handle all other states and perform additional work

}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::OnLeave()
{
    //  int x = this->Interactor->GetEventPosition()[0];
    //int y = this->Interactor->GetEventPosition()[1];

    switch (this->State)
    {
    case VTKIS_WINDOW_LEVEL:
        break;

    case VTKIS_PICK:
        break;

    case VTKIS_PAN:
        break;

    default:
        this->InvokeEvent(vtkCommand::LeaveEvent, NULL);
        break;
    }

    // Call parent to handle all other states and perform additional work

}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::OnRightButtonDown()
{
    int x = this->Interactor->GetEventPosition()[0];
    int y = this->Interactor->GetEventPosition()[1];

    this->FindPokedRenderer(x, y);
    if (this->CurrentRenderer == NULL)
    {
        return;
    }

    // Redefine this button to handle window/level
    this->GrabFocus(this->EventCallbackCommand);
    if (!this->Interactor->GetShiftKey() && !this->Interactor->GetControlKey())
    {
        this->WindowLevelStartPosition[0] = x;
        this->WindowLevelStartPosition[1] = y;
        this->StartWindowLevel();
    }

    // The rest of the button + key combinations remain the same

    else
    {
        this->Superclass::OnRightButtonDown();
    }
}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::OnRightButtonUp()
{
    switch (this->State)
    {
    case VTKIS_WINDOW_LEVEL:
        this->EndWindowLevel();
        if ( this->Interactor )
        {
            this->ReleaseFocus();
        }
        break;
    }

    // Call parent to handle all other states and perform additional work

    this->Superclass::OnRightButtonUp();
}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::OnLeftButtonDown()
{
    int x = this->Interactor->GetEventPosition()[0];
    int y = this->Interactor->GetEventPosition()[1];

    this->FindPokedRenderer(x, y);
    if (this->CurrentRenderer == NULL)
    {
        return;
    }

    // Redefine this button to handle pick
    this->GrabFocus(this->EventCallbackCommand);
    if (!this->Interactor->GetShiftKey() && !this->Interactor->GetControlKey())
    {
        this->StartPick();
    }

    // The rest of the button + key combinations remain the same

    else
    {
        this->Superclass::OnLeftButtonDown();
    }
}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::OnLeftButtonUp()
{
    switch (this->State)
    {
    case VTKIS_PICK:
        this->EndPick();
        if ( this->Interactor )
        {
            this->ReleaseFocus();
        }
        break;
    }

    // Call parent to handle all other states and perform additional work

    this->Superclass::OnLeftButtonUp();
}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::OnMiddleButtonDown()
{
    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                            this->Interactor->GetEventPosition()[1]);
    if (this->CurrentRenderer == NULL)
    {
        return;
    }
    this->CurrentRenderer->GetRenderWindow()->SetCurrentCursor(8);
    this->GrabFocus(this->EventCallbackCommand);
    this->StartPan();
}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::OnMiddleButtonUp()
{
    switch (this->State)
    {
    case VTKIS_PAN:
        this->EndPan();
        if ( this->Interactor )
        {
            this->Interactor->GetRenderWindow()->SetCurrentCursor(0);
            this->ReleaseFocus();
        }
        break;
    }
}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::OnChar()
{
    vtkRenderWindowInteractor *rwi = this->Interactor;

    switch (rwi->GetKeyCode())
    {
    case 'f' :
    case 'F' :
    {
        this->AnimState = VTKIS_ANIM_ON;
        this->AnimState = VTKIS_ANIM_OFF;
        break;
    }

    case 'w'  :
    case 'W'  :
    {
        this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                                this->Interactor->GetEventPosition()[1]);
        if (this->CurrentRenderer == NULL)
        {
            return;
        }
        this->GrabFocus(this->EventCallbackCommand);
        this->StartDolly();
        double factor = -2;
        this->Dolly(pow((double)1.1, factor));
        this->EndDolly();
        this->ReleaseFocus();
        break;
    }
    case 'x'  :
    case 'X'  :
    {
        this->FindPokedRenderer(rwi->GetEventPosition()[0],
                                rwi->GetEventPosition()[1]);
        if (this->CurrentRenderer == NULL)
        {
            return;
        }
        this->GrabFocus(this->EventCallbackCommand);
        this->StartDolly();
        double factor = 2;
        this->Dolly(pow((double)1.1, factor));
        this->EndDolly();
        this->ReleaseFocus();
        break;
    }
    case '3' :
        // Disable StereoVision
        break;
    case 'r' :
    case 'R' :
        //Do nothing, this is handled in vvSlicerManagerCommand
        break;
    default:
        this->Superclass::OnChar();
        break;
    }
}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::OnMouseWheelForward()
{
    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                            this->Interactor->GetEventPosition()[1]);
    if (this->CurrentRenderer == NULL)
    {
        return;
    }
    this->GrabFocus(this->EventCallbackCommand);
    if (this->Interactor->GetControlKey())
    {
        this->StartDolly();
        double factor = this->MotionFactor * 0.2 * this->MouseWheelMotionFactor;
        this->Dolly(pow((double)1.1, factor));
        this->EndDolly();
    }
    this->ReleaseFocus();
    this->InvokeEvent(vtkCommand::MouseWheelForwardEvent, this);
}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::OnMouseWheelBackward()
{
    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                            this->Interactor->GetEventPosition()[1]);
    if (this->CurrentRenderer == NULL)
    {
        return;
    }

    this->GrabFocus(this->EventCallbackCommand);
    if (this->Interactor->GetControlKey())
    {
        this->StartDolly();
        double factor = this->MotionFactor * -0.2 * this->MouseWheelMotionFactor;
        this->Dolly(pow((double)1.1, factor));
        this->EndDolly();
    }
    this->ReleaseFocus();
    this->InvokeEvent(vtkCommand::MouseWheelBackwardEvent, this);
}


//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::WindowLevel()
{
    vtkRenderWindowInteractor *rwi = this->Interactor;

    this->WindowLevelCurrentPosition[0] = rwi->GetEventPosition()[0];
    this->WindowLevelCurrentPosition[1] = rwi->GetEventPosition()[1];

    this->InvokeEvent(vtkCommand::WindowLevelEvent, this);
}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::Pick()
{
    this->InvokeEvent(vtkCommand::PickEvent, this);
}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::Pan()
{
    if (this->CurrentRenderer == NULL)
    {
        return;
    }

    vtkRenderWindowInteractor *rwi = this->Interactor;

    double viewFocus[4], focalDepth, viewPoint[3];
    double newPickPoint[4], oldPickPoint[4], motionVector[3];

    // Calculate the focal depth since we'll be using it a lot

    vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
    camera->GetFocalPoint(viewFocus);
    this->ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2],
                                viewFocus);
    focalDepth = viewFocus[2];

    this->ComputeDisplayToWorld((double)rwi->GetEventPosition()[0],
                                (double)rwi->GetEventPosition()[1],
                                focalDepth,
                                newPickPoint);

    // Has to recalc old mouse point since the viewport has moved,
    // so can't move it outside the loop

    this->ComputeDisplayToWorld((double)rwi->GetLastEventPosition()[0],
                                (double)rwi->GetLastEventPosition()[1],
                                focalDepth,
                                oldPickPoint);

    // Camera motion is reversed

    motionVector[0] = oldPickPoint[0] - newPickPoint[0];
    motionVector[1] = oldPickPoint[1] - newPickPoint[1];
    motionVector[2] = oldPickPoint[2] - newPickPoint[2];

    camera->GetFocalPoint(viewFocus);
    camera->GetPosition(viewPoint);
    camera->SetFocalPoint(motionVector[0] + viewFocus[0],
                          motionVector[1] + viewFocus[1],
                          motionVector[2] + viewFocus[2]);

    camera->SetPosition(motionVector[0] + viewPoint[0],
                        motionVector[1] + viewPoint[1],
                        motionVector[2] + viewPoint[2]);

    if (rwi->GetLightFollowCamera())
    {
        this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

    rwi->Render();
}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::Dolly()
{
    if (this->CurrentRenderer == NULL)
    {
        return;
    }

    vtkRenderWindowInteractor *rwi = this->Interactor;
    double *center = this->CurrentRenderer->GetCenter();
    int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
    double dyf = this->MotionFactor * (double)(dy) / (double)(center[1]);
    this->Dolly(pow((double)1.1, dyf));
}

//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::Dolly(double factor)
{
    if (this->CurrentRenderer == NULL)
    {
        return;
    }

    double viewFocus[4],viewPoint[4],motionVector[3], focalDepth;
    double oldPos[3], newPos[3], distance[2];
    vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
    camera->GetFocalPoint(viewFocus);
    this->ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2],
                                viewFocus);
    focalDepth = viewFocus[2];

    oldPos[0] = this->CurrentRenderer->GetCenter()[0];
    oldPos[1] = this->CurrentRenderer->GetCenter()[1];
    oldPos[2] = focalDepth;

    distance[0] = 1/factor*
                  (this->Interactor->GetEventPosition()[0]-this->CurrentRenderer->GetCenter()[0]);
    distance[1] = 1/factor*
                  (this->Interactor->GetEventPosition()[1]-this->CurrentRenderer->GetCenter()[1]);

    newPos[0] = this->Interactor->GetEventPosition()[0] - distance[0];
    newPos[1] = this->Interactor->GetEventPosition()[1] - distance[1];
    newPos[2] = focalDepth;

    this->CurrentRenderer->DisplayToNormalizedDisplay(oldPos[0],oldPos[1]);
    this->CurrentRenderer->NormalizedDisplayToViewport(oldPos[0],oldPos[1]);
    this->CurrentRenderer->ViewportToNormalizedViewport(oldPos[0],oldPos[1]);
    this->CurrentRenderer->NormalizedViewportToView(oldPos[0],oldPos[1],oldPos[2]);
    this->CurrentRenderer->ViewToWorld(oldPos[0],oldPos[1],oldPos[2]);

    this->CurrentRenderer->DisplayToNormalizedDisplay(newPos[0],newPos[1]);
    this->CurrentRenderer->NormalizedDisplayToViewport(newPos[0],newPos[1]);
    this->CurrentRenderer->ViewportToNormalizedViewport(newPos[0],newPos[1]);
    this->CurrentRenderer->NormalizedViewportToView(newPos[0],newPos[1],newPos[2]);
    this->CurrentRenderer->ViewToWorld(newPos[0],newPos[1],newPos[2]);

    motionVector[0] = newPos[0] - oldPos[0];
    motionVector[1] = newPos[1] - oldPos[1];
    motionVector[2] = newPos[2] - oldPos[2];

    camera->GetFocalPoint(viewFocus);
    camera->GetPosition(viewPoint);
    camera->SetFocalPoint(motionVector[0] + viewFocus[0],
                          motionVector[1] + viewFocus[1],
                          motionVector[2] + viewFocus[2]);

    camera->SetPosition(motionVector[0] + viewPoint[0],
                        motionVector[1] + viewPoint[1],
                        motionVector[2] + viewPoint[2]);

    if (camera->GetParallelProjection())
    {
        camera->SetParallelScale(camera->GetParallelScale() / factor);
    }
    else
    {
        camera->Dolly(factor);
        if (this->AutoAdjustCameraClippingRange)
        {
            this->CurrentRenderer->ResetCameraClippingRange();
        }
    }

    if (this->Interactor->GetLightFollowCamera())
    {
        this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }
    this->CurrentRenderer->ResetCameraClippingRange();
    //this->Interactor->Render();
}


//----------------------------------------------------------------------------
void vvInteractorStyleNavigator::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);

    os << indent << "Window Level Current Position: ("
    << this->WindowLevelCurrentPosition[0] << ", "
    << this->WindowLevelCurrentPosition[1] << ")" << endl;

    os << indent << "Window Level Start Position: ("
    << this->WindowLevelStartPosition[0] << ", "
    << this->WindowLevelStartPosition[1] << ")" << endl;
}
