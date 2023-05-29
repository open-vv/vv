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
#ifdef VTK_OPENGL
  #include <vtkgl.h>
  #include <vtkOpenGLExtensionManager.h>
#elif VTK_OPENGL2
  #include <vtk_glew.h> //VTK_OPENGL2
#else
  #error "You need to compile VTK with OpenGL!"
#endif
#include <vtkOpenGLRenderWindow.h>
#include <vtkOpenGLRenderer.h>
#include <vtkOpenGL.h>
#include <vtkObjectFactory.h>

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
#ifdef VTK_OPENGL
  vtkOpenGLExtensionManager *extensions = renwin->GetExtensionManager();
  if (extensions->ExtensionSupported("GL_EXT_blend_minmax")) {
    extensions->LoadExtension("GL_EXT_blend_minmax");
    vtkgl::BlendEquationEXT( vtkgl::MAX );
  }

  //Call normal render
  VTK_IMAGE_ACTOR::Render(ren);

  //Move back blending to weighted sum
  if (vtkgl::BlendEquationEXT!=0) {
    vtkgl::BlendEquationEXT( vtkgl::FUNC_ADD );
  }
#else
  //VTK_OPENGL2
  const char *extensions = renwin->ReportCapabilities();

  //Call normal render
  VTK_IMAGE_ACTOR::Render(ren);
#endif
}

void vvBlendImageActor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
