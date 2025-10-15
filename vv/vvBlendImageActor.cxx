/*=========================================================================
Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

Authors belong to:
- University of LYON              http://www.universite-lyon.fr/
- L�on B�rard cancer center       http://www.centreleonberard.fr
- CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the copyright notices for more information.

It is distributed under dual licence

- BSD        See included LICENSE.txt file
- CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
===========================================================================**/

#include "vvBlendImageActor.h"
#include <vtkOpenGLRenderWindow.h>
#include <vtkOpenGLRenderer.h>
#include <vtkObjectFactory.h>
#include <vtkOpenGLState.h>
#include <vtk_glad.h>

vtkStandardNewMacro(vvBlendImageActor);

vvBlendImageActor::vvBlendImageActor()
{
}

vvBlendImageActor::~vvBlendImageActor()
{
}

// Actual actor render method.
void vvBlendImageActor::Render(vtkRenderer *ren)
{
  //Change blending to maximum per component instead of weighted sum
  vtkOpenGLRenderWindow *renwin = dynamic_cast<vtkOpenGLRenderWindow*>(ren->GetRenderWindow());
#ifdef VTK_OPENGL2
  const char *extensions = renwin->ReportCapabilities();

  //Call normal render
  VTK_IMAGE_ACTOR::Render(ren);

#else
  // VTK 9 uses OpenGL 3.2+ which has blend equations built-in
  glBlendEquation(GL_MAX);

  //Call normal render
  VTK_IMAGE_ACTOR::Render(ren);

  //Move back blending to weighted sum
  glBlendEquation(GL_FUNC_ADD);
#endif
}

void vvBlendImageActor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
