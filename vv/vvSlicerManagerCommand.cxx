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
#include "vvSlicerManagerCommand.h"
#include "vvSlicerManager.h"

#include <vtkTextProperty.h>
#include <vtkRenderer.h>
#include <vtkImageActor.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPropPicker.h>
#include <vtkCamera.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkLookupTable.h>
#include <vtkMath.h>
#include <vtkAbstractPropPicker.h>
#include <vtkAssemblyPath.h>
#include <vtkCornerAnnotation.h>
#include <vtkRenderWindow.h>
#include <vvImage.h>
#include <vtkImageReslice.h>
#include "vvSlicer.h"
#include "vvInteractorStyleNavigator.h"
#include "vtkTransform.h"

#include <cmath>

//------------------------------------------------------------------------------
vvSlicerManagerCommand::vvSlicerManagerCommand()
{
  mStartSlicer = -1;
  mSlicerNumber=-1;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//return the num of the current slicer if visible (-1 else)
int vvSlicerManagerCommand::FindSlicerNumber(vtkRenderWindow* renwin)
{
  int rvalue;
  if (renwin != SM->GetSlicer(mSlicerNumber)->GetRenderWindow() ||
      !SM->GetSlicer(mSlicerNumber)->GetRenderer()->GetDraw())
    rvalue = -1;
  else rvalue = mSlicerNumber;
  //std::cerr << this << ":" << mSlicerNumber << ": " << rvalue << endl;
  return rvalue;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicerManagerCommand::Execute(vtkObject *caller,
                                     unsigned long event,
                                     void *vtkNotUsed(callData))
{
  //KeyPress event
  vvInteractorStyleNavigator *isi =
    dynamic_cast<vvInteractorStyleNavigator *>(caller);
  if (isi) {
    double x = isi->GetInteractor()->GetEventPosition()[0];
    double y = isi->GetInteractor()->GetEventPosition()[1];
    double z;

    int VisibleInWindow = this->FindSlicerNumber(isi->GetInteractor()->GetRenderWindow());
    vtkRenderer* renderer=NULL;
    if (VisibleInWindow>-1)
      renderer=this->SM->GetSlicer(VisibleInWindow)->GetRenderer();
    newLandmark = false;



    if (event == vtkCommand::StartPickEvent && VisibleInWindow == -1) {
      for (int i = 0; i < this->SM->GetNumberOfSlicers(); i++) {
        if (this->SM->GetSlicer(i)->GetCursorVisibility() && !this->SM->IsLinked()) {
          this->SM->GetSlicer(i)->SetCursorVisibility(0);
          this->SM->GetSlicer(i)->SetCornerAnnotationVisibility(0);
          this->SM->GetSlicer(i)->Render();
        }
      }
    }
    if ( VisibleInWindow > -1 ) {
      if (event == vtkCommand::KeyPressEvent) {
        std::string KeyPress = isi->GetInteractor()->GetKeySym();
        bool bCtrlKey = isi->GetInteractor()->GetControlKey();
        if (KeyPress == "Tab") {
          if(isi->GetInteractor()->GetShiftKey())
            this->SM->PrevImage(VisibleInWindow);
          else
            this->SM->NextImage(VisibleInWindow);
          return;
        }
        if (KeyPress == "f" || KeyPress == "F") {
          FlyToPosition(isi->GetInteractor(),this->SM->GetSlicer(VisibleInWindow));
          return;
        }
        if (KeyPress == "w") {
          this->SM->SetLocalColorWindowing(VisibleInWindow, bCtrlKey);
          return;
        }
        if (KeyPress == "0") {
          this->SM->SetPreset(0);
          this->SM->UpdateWindowLevel();
          return;
        }
        if (KeyPress == "1") {
          this->SM->SetPreset(1);
          this->SM->UpdateWindowLevel();
          return;
        }
        if (KeyPress == "2") {
          this->SM->SetPreset(2);
          this->SM->UpdateWindowLevel();

          return;
        }
        if (KeyPress == "3") {
          this->SM->SetPreset(3);
          this->SM->UpdateWindowLevel();
          return;
        }
        if (KeyPress == "4") {
          this->SM->SetPreset(4);
          this->SM->UpdateWindowLevel();
          return;
        }
        if (KeyPress == "5") {
          this->SM->SetPreset(5);
          this->SM->UpdateWindowLevel();
          return;
        }
        if (KeyPress == "6") {
          this->SM->SetColorMap(0);
          this->SM->UpdateWindowLevel();
          return;
        }
        if (KeyPress == "7") {
          this->SM->SetColorMap(1);
          this->SM->UpdateWindowLevel();
          return;
        }
        if (KeyPress == "8") {
          this->SM->SetColorMap(2);
          this->SM->UpdateWindowLevel();
          return;
        }
        if (KeyPress == "9") {
          this->SM->SetColorMap(3);
          this->SM->UpdateWindowLevel();
          return;
        }
        if (KeyPress == "equal") { //keycodes are in vtkWin32RenderWindowInteractor
          this->SM->SetPreset(7);
          //this->SM->SetColorMap(1);
          this->SM->UpdateWindowLevel();
          return;
        }
        if (KeyPress == "minus") {
          this->SM->SetColorWindow(-this->SM->GetColorWindow());
          this->SM->SetColorMap(-1);
          this->SM->UpdateWindowLevel();
          return;
        }
        if (KeyPress == "c") {
          this->SM->ToggleContourSuperposition();
          this->SM->Render();
          return;
        }
        if (KeyPress == "l") {
          this->SM->ToggleInterpolation();
          this->SM->Render();
          return;
        }
        if (KeyPress == "h") {
          this->SM->SetCursorAndCornerAnnotationVisibility(0);
          this->SM->Render();
          return;
        }
        if (KeyPress == "x") {
          for(int i=0; i<SM->GetNumberOfSlicers(); i++) {
            SM->RemoveActor("overlay",0);
            SM->SetColorMap(0);
            SM->Render();
          }
          return;
        }
        if (KeyPress == "u") {
          this->SM->Reload();
          this->SM->Render();
          return;
        }
        if (KeyPress == "r" || KeyPress=="R") {
          this->SM->GetSlicer(VisibleInWindow)->ResetCamera();
          this->SM->GetSlicer(VisibleInWindow)->Render();
          this->SM->UpdateLinkedNavigation( this->SM->GetSlicer(VisibleInWindow) );
          return;
        }
        if (KeyPress == "g") {
          if(bCtrlKey)
            this->SM->GetSlicer(VisibleInWindow)->SetCurrentPosition(0,0,0,0);
          else {
            double* cursorPos = this->SM->GetSlicer(VisibleInWindow)->GetCursorPosition();
            this->SM->GetSlicer(VisibleInWindow)->SetCurrentPosition(
              cursorPos[0],cursorPos[1],cursorPos[2],cursorPos[3]);
          }
          this->SM->UpdateViews(1,VisibleInWindow);
          this->SM->UpdateLinked(VisibleInWindow);
          return;
        }
        if (KeyPress == "F5") {
          this->SM->GetSlicer(VisibleInWindow)->FlipHorizontalView();
          this->SM->GetSlicer(VisibleInWindow)->Render();
          return;
        }
        if (KeyPress == "F6") {
          this->SM->GetSlicer(VisibleInWindow)->FlipVerticalView();
          this->SM->GetSlicer(VisibleInWindow)->Render();
          return;
        }
        if (KeyPress == "Up") {
          this->SM->GetSlicer(VisibleInWindow)->SetSlice(this->SM->GetSlicer(VisibleInWindow)->GetSlice()+1);
          this->SM->UpdateSlice(VisibleInWindow);
        }
        if (KeyPress == "Down") {
          this->SM->GetSlicer(VisibleInWindow)->SetSlice(this->SM->GetSlicer(VisibleInWindow)->GetSlice()-1);
          this->SM->UpdateSlice(VisibleInWindow);
        }
        if (KeyPress == "space") {
          newLandmark = true;
        }
        if (KeyPress == "Left")
          this->SM->SetPreviousTSlice(VisibleInWindow);
        if (KeyPress == "Right")
          this->SM->SetNextTSlice(VisibleInWindow);

        if (KeyPress == "F2") {
          this->SM->GetSlicer(VisibleInWindow)->GetAnnotation()->SetText(2,"Sagital\n<slice>");
          //this->SM->GetSlicer(VisibleInWindow)->SetSliceOrientation(0);
          this->SM->SetSliceOrientation(VisibleInWindow, 0);
          this->SM->UpdateSliceRange(VisibleInWindow);
        }
        if (KeyPress == "F3") {
          this->SM->GetSlicer(VisibleInWindow)->GetAnnotation()->SetText(2,"Coronal\n<slice>");
          //this->SM->GetSlicer(VisibleInWindow)->SetSliceOrientation(1);
          this->SM->SetSliceOrientation(VisibleInWindow, 1);
          this->SM->UpdateSliceRange(VisibleInWindow);
        }
        if (KeyPress == "F4") {
          this->SM->GetSlicer(VisibleInWindow)->GetAnnotation()->SetText(2,"Axial\n<slice>");
          //this->SM->GetSlicer(VisibleInWindow)->SetSliceOrientation(2);
          this->SM->SetSliceOrientation(VisibleInWindow, 2);
          this->SM->UpdateSliceRange(VisibleInWindow);
        }

        this->SM->EmitKeyPressed(KeyPress);
      }

      //All type of mouse events
      if (event == vtkCommand::LeaveEvent) {
        this->SM->GetSlicer(VisibleInWindow)->SetCurrentPosition(-VTK_DOUBLE_MAX,-VTK_DOUBLE_MAX,
            -VTK_DOUBLE_MAX,this->SM->GetSlicer(VisibleInWindow)->GetMaxCurrentTSlice());
        this->SM->GetSlicer(VisibleInWindow)->Render();
        return;
      }

      // Mouse release HERE
      if (event == vtkCommand::EndPickEvent) {
        //	      DD(VisibleInWindow);
        if (VisibleInWindow > -1)
          this->SM->LeftButtonReleaseEvent(VisibleInWindow);
        return; // no return !!!! ???
      }

      if (event == vtkCommand::StartWindowLevelEvent) {
        mStartSlicer = -1;
        this->InitialWindow = this->SM->GetColorWindow();
        this->InitialLevel = this->SM->GetColorLevel();

        if (VisibleInWindow > -1) {
          mStartSlicer = VisibleInWindow;
        }
        return;
      }

      if (event == vtkCommand::EndWindowLevelEvent) {
        mStartSlicer = -1;
      }

      if (event == vtkCommand::EndInteractionEvent) {
        this->SM->Picked();
        this->SM->UpdateLinkedNavigation(this->SM->GetSlicer(VisibleInWindow),true);
        return;
      }
    }
    if (VisibleInWindow > -1) {
      this->SM->Activated();
      this->SM->GetSlicer(VisibleInWindow)->SetCornerAnnotationVisibility(1);

      if (event == vtkCommand::MouseWheelForwardEvent && !isi->GetInteractor()->GetControlKey()) {
        this->SM->Picked();
        this->SM->GetSlicer(VisibleInWindow)->SetSlice(this->SM->GetSlicer(VisibleInWindow)->GetSlice()+1);
        this->SM->UpdateSlice(VisibleInWindow);
      } else if (event == vtkCommand::MouseWheelForwardEvent && isi->GetInteractor()->GetControlKey()) {
        double factor = 2;
        this->Dolly(pow((double)1.1, factor),isi->GetInteractor());
        Execute(caller, vtkCommand::EndInteractionEvent, NULL);
      } else if (event == vtkCommand::MouseWheelBackwardEvent && !isi->GetInteractor()->GetControlKey()) {
        this->SM->Picked();
        this->SM->GetSlicer(VisibleInWindow)->SetSlice(this->SM->GetSlicer(VisibleInWindow)->GetSlice()-1);
        this->SM->UpdateSlice(VisibleInWindow);
      } else if (event == vtkCommand::MouseWheelBackwardEvent && isi->GetInteractor()->GetControlKey()) {
        double factor = -2;
        this->Dolly(pow((double)1.1, factor),isi->GetInteractor());
        Execute(caller, vtkCommand::EndInteractionEvent, NULL);
      }
      double xWorld=0;
      double yWorld=0;
      double zWorld=0;

      //Move into World Coordinate
      renderer->DisplayToNormalizedDisplay(x,y);
      renderer->NormalizedDisplayToViewport(x,y);
      renderer->ViewportToNormalizedViewport(x,y);
      renderer->NormalizedViewportToView(x,y,z);
      renderer->ViewToWorld(x,y,z);
      switch (this->SM->GetSlicer(VisibleInWindow)->GetSliceOrientation()) {
      case vtkImageViewer2::SLICE_ORIENTATION_XY:
        xWorld = x;
        yWorld = y;
        zWorld = this->SM->GetSlicer(VisibleInWindow)->GetSlice()*
                 this->SM->GetSlicer(VisibleInWindow)->GetInput()->GetSpacing()[2] +
                 this->SM->GetSlicer(VisibleInWindow)->GetInput()->GetOrigin()[2];
        break;

      case vtkImageViewer2::SLICE_ORIENTATION_XZ:
        xWorld = x;
        yWorld = this->SM->GetSlicer(VisibleInWindow)->GetSlice()*
                 this->SM->GetSlicer(VisibleInWindow)->GetInput()->GetSpacing()[1] +
                 this->SM->GetSlicer(VisibleInWindow)->GetInput()->GetOrigin()[1];
        zWorld = z;
        break;

      case vtkImageViewer2::SLICE_ORIENTATION_YZ:
        xWorld = this->SM->GetSlicer(VisibleInWindow)->GetSlice()*
                 this->SM->GetSlicer(VisibleInWindow)->GetInput()->GetSpacing()[0] +
                 this->SM->GetSlicer(VisibleInWindow)->GetInput()->GetOrigin()[0];
        yWorld = y;
        zWorld = z;
        break;
      }

      // <<<<<<< HEAD
      //       this->SM->GetSlicer(VisibleInWindow)->SetCurrentPosition(xWorld,yWorld,zWorld,
      //                           this->SM->GetSlicer(VisibleInWindow)->GetTSlice());
      //       // We propagate the mouse position
      //       this->SM->EmitMousePositionUpdated(VisibleInWindow);      
      // =======
      double p[3]; p[0] = xWorld; p[1] = yWorld; p[2] = zWorld;
      double pt[3];
      this->SM->GetSlicer(VisibleInWindow)->GetSlicingTransform()->TransformPoint(p, pt);

      this->SM->GetSlicer(VisibleInWindow)->SetCurrentPosition(pt[0],pt[1],pt[2],
          this->SM->GetSlicer(VisibleInWindow)->GetMaxCurrentTSlice());

      // We propagate the mouse position
      this->SM->EmitMousePositionUpdated(VisibleInWindow);

      //>>>>>>> 921642d767beba2442dacc8fdb40dc36396e1b7d

      if (newLandmark) {
        this->SM->AddLandmark(xWorld,yWorld,zWorld,
                              this->SM->GetSlicer(VisibleInWindow)->GetTSlice());
        this->SM->GetSlicer(VisibleInWindow)->UpdateLandmarks();
        this->SM->Render();
      }
      if (event == vtkCommand::PickEvent || event == vtkCommand::StartPickEvent) {
        this->SM->Picked();
        this->SM->UpdateViews(1,VisibleInWindow);
        this->SM->UpdateLinked(VisibleInWindow);
        this->SM->UpdateInfoOnCursorPosition(VisibleInWindow);
      } else {
        this->SM->GetSlicer(VisibleInWindow)->Render();
      }
      //this->SM->GetSlicer(VisibleInWindow)->SetCurrentPosition(-VTK_DOUBLE_MAX,-VTK_DOUBLE_MAX,
      //-VTK_DOUBLE_MAX,this->SM->GetSlicer(VisibleInWindow)->GetTSlice());
      //this->SM->GetSlicer(VisibleInWindow)->Render();
    }

    if (event == vtkCommand::WindowLevelEvent && mStartSlicer > -1) {
      this->SM->GetSlicer(mStartSlicer)->SetCornerAnnotationVisibility(1);
      // Adjust the window level here
      int *size = isi->GetInteractor()->GetRenderWindow()->GetSize();
      double window = this->InitialWindow;
      double level = this->InitialLevel;
      double range[2];
      this->SM->GetImage()->GetScalarRange(range);

      // Compute normalized delta
      double dx = static_cast<double>(isi->GetWindowLevelCurrentPosition()[0] -
                                      isi->GetWindowLevelStartPosition()[0]) / size[0];
      double dy = static_cast<double>(isi->GetWindowLevelStartPosition()[1] -
                                      isi->GetWindowLevelCurrentPosition()[1]) / size[1];
      //Window is exponential in nature, use exponential to avoid falling into negative numbers
      dx = std::exp(1.0 * (dx*fabs(dx) + dx)) ; //Quadratic behavior for more reactive interface
      dy = 0.15 * (dy*fabs(dy) + dy) * (range[1]-range[0]);//Quadratic behavior for more reactive interface

      this->SM->SetColorWindow(window*dx);
      this->SM->SetColorLevel(level-dy);
      this->SM->SetPreset(6);
      this->SM->Render();
      this->SM->UpdateWindowLevel();
      return;
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicerManagerCommand::Dolly(double factor, vtkRenderWindowInteractor *interactor)
{
  int VisibleInWindow = this->FindSlicerNumber(interactor->GetRenderWindow());
  vtkRenderer* renderer;
  if (VisibleInWindow>-1)
    renderer=this->SM->GetSlicer(VisibleInWindow)->GetRenderer();
  else {
    return;
  }

  double viewFocus[4],viewPoint[4],motionVector[3], focalDepth;
  double oldPos[3], newPos[3], distance[2];
  vtkCamera *camera = renderer->GetActiveCamera();
  camera->GetFocalPoint(viewFocus);

  renderer->SetWorldPoint(viewFocus[0], viewFocus[0], viewFocus[0], 1.0);
  renderer->WorldToDisplay();
  renderer->GetDisplayPoint(viewFocus);

  focalDepth = viewFocus[2];

  oldPos[0] = renderer->GetCenter()[0];
  oldPos[1] = renderer->GetCenter()[1];
  oldPos[2] = focalDepth;

  distance[0] = 1/factor*
                (interactor->GetEventPosition()[0]-renderer->GetCenter()[0]);
  distance[1] = 1/factor*
                (interactor->GetEventPosition()[1]-renderer->GetCenter()[1]);

  newPos[0] = interactor->GetEventPosition()[0] - distance[0];
  newPos[1] = interactor->GetEventPosition()[1] - distance[1];
  newPos[2] = focalDepth;

  renderer->DisplayToNormalizedDisplay(oldPos[0],oldPos[1]);
  renderer->NormalizedDisplayToViewport(oldPos[0],oldPos[1]);
  renderer->ViewportToNormalizedViewport(oldPos[0],oldPos[1]);
  renderer->NormalizedViewportToView(oldPos[0],oldPos[1],oldPos[2]);
  renderer->ViewToWorld(oldPos[0],oldPos[1],oldPos[2]);

  renderer->DisplayToNormalizedDisplay(newPos[0],newPos[1]);
  renderer->NormalizedDisplayToViewport(newPos[0],newPos[1]);
  renderer->ViewportToNormalizedViewport(newPos[0],newPos[1]);
  renderer->NormalizedViewportToView(newPos[0],newPos[1],newPos[2]);
  renderer->ViewToWorld(newPos[0],newPos[1],newPos[2]);

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

  if (camera->GetParallelProjection()) {
    camera->SetParallelScale(camera->GetParallelScale() / factor);
  } else {
    camera->Dolly(factor);
  }

  if (interactor->GetLightFollowCamera()) {
    renderer->UpdateLightsGeometryToFollowCamera();
  }
  renderer->ResetCameraClippingRange();
  //interactor->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicerManagerCommand::FlyToPosition(vtkRenderWindowInteractor *interactor,vvSlicer* slicer)
{
  double flyFrom[3], flyTo[3];
  double d[3], focalPt[3], position[3], positionFrom[3];
  int i, j;
  int VisibleInWindow = this->FindSlicerNumber(interactor->GetRenderWindow());
  vtkRenderer* renderer=NULL;
  if (VisibleInWindow>-1)
    renderer=this->SM->GetSlicer(VisibleInWindow)->GetRenderer();
  else
    return;

  interactor->GetPicker()->Pick(interactor->GetEventPosition()[0],
                                interactor->GetEventPosition()[1], 0.0,
                                renderer);

  vtkAssemblyPath *path=NULL;
  vtkAbstractPropPicker *picker;
  if ( (picker=vtkAbstractPropPicker::SafeDownCast(interactor->GetPicker()))) {
    path = picker->GetPath();
  }
  if ( path != NULL ) {
    flyTo[0] = picker->GetPickPosition()[0];
    flyTo[1] = picker->GetPickPosition()[1];
    flyTo[2] = picker->GetPickPosition()[2];
    renderer->GetActiveCamera()->GetFocalPoint(flyFrom);
    renderer->GetActiveCamera()->GetPosition(positionFrom);

    switch (slicer->GetSliceOrientation()) {
    case vtkImageViewer2::SLICE_ORIENTATION_XY:
      flyTo[2] = flyFrom[2];
      break;

    case vtkImageViewer2::SLICE_ORIENTATION_XZ:
      flyTo[1] = flyFrom[1];
      break;

    case vtkImageViewer2::SLICE_ORIENTATION_YZ:
      flyTo[0] = flyFrom[0];
      break;
    }


    for (i=0; i<3; i++) {
      d[i] = flyTo[i] - flyFrom[i];
    }
    double distance = vtkMath::Normalize(d);
    double delta = distance/15;

    for (i=1; i<=15; i++) {
      for (j=0; j<3; j++) {
        focalPt[j] = flyFrom[j] + d[j]*i*delta;
        position[j] = positionFrom[j] + d[j]*i*delta;
      }
      renderer->GetActiveCamera()->SetFocalPoint(focalPt);
      renderer->GetActiveCamera()->SetPosition(position);
      renderer->GetActiveCamera()->Dolly(0.3/15 + 1.0);
      renderer->ResetCameraClippingRange();
      interactor->Render();
    }
  }
}
//------------------------------------------------------------------------------
