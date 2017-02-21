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
#include "vvToolHistogramCommand.h"
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
vvToolHistogramCommand::vvToolHistogramCommand()
{
  mFollowMouse = false;
}
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
void vvToolHistogramCommand::Execute(vtkObject *caller,
                                     unsigned long event,
                                     void *vtkNotUsed(callData))
{
  if (event == vtkCommand::MouseWheelForwardEvent) {
    mHist->expandWindow();
  }
  if (event == vtkCommand::MouseWheelBackwardEvent) {
    mHist->reduceWindow();
  }
  if (event == vtkCommand::LeftButtonPressEvent) {
    mFollowMouse = !mFollowMouse;
  }
  if (event == vtkCommand::MouseMoveEvent) {
    if (mFollowMouse) {
      vtkSmartPointer<vtkRenderWindowInteractor> interactor = mHist->GetHistogramWidget()->GetRenderWindow()->GetInteractor();
      if (interactor->GetEventPosition()[0] != interactor->GetLastEventPosition()[0])
        mHist->translateWindow(interactor->GetEventPosition()[0] - interactor->GetLastEventPosition()[0]);
    }
  }
}
//------------------------------------------------------------------------------
