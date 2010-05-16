  /*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  ======================================================================-====*/

#include "vvBlendImageActor.h"

#include <vtkOpenGLRenderWindow.h>
#include <vtkOpenGLExtensionManager.h>
#include <vtkOpenGLRenderer.h>
#include <vtkOpenGL.h>
#include <vtkgl.h>
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
  vtkOpenGLExtensionManager *extensions = renwin->GetExtensionManager();
  if (extensions->ExtensionSupported("GL_EXT_blend_minmax"))
    {
    extensions->LoadExtension("GL_EXT_blend_minmax");
    vtkgl::BlendEquationEXT( vtkgl::MAX );
    }

  //Call normal render
  vtkOpenGLImageActor::Render(ren);

  //Move back blending to weighted sum
  if (vtkgl::BlendEquationEXT!=0)
    {
    vtkgl::BlendEquationEXT( vtkgl::FUNC_ADD );
    }
}

void vvBlendImageActor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
