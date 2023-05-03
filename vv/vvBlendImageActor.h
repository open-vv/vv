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

#ifndef VVBLENDIMAGEACTOR_H
#define VVBLENDIMAGEACTOR_H

#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkVersion.h>

#if VTK_MAJOR_VERSION >= 6 || (VTK_MAJOR_VERSION >= 5 && VTK_MINOR_VERSION >= 10)
#define VTK_IMAGE_ACTOR vtkImageActor
#include <vtkImageActor.h>
#else
#define VTK_IMAGE_ACTOR vtkOpenGLImageActor
#include <vtkOpenGLImageActor.h>
#endif

class vvBlendImageActor : public VTK_IMAGE_ACTOR
{
public:
  static vvBlendImageActor *New();
  vtkTypeMacro(vvBlendImageActor, VTK_IMAGE_ACTOR);
#if VTK_MAJOR_VERSION >= 8
  virtual void PrintSelf(ostream& os, vtkIndent indent) override;
#else
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;
#endif

  // Description:
  // Implement base class method.
#if VTK_MAJOR_VERSION >= 8
  void Render(vtkRenderer *ren) override;
#else
  void Render(vtkRenderer *ren) VTK_OVERRIDE;
#endif

protected:
  vvBlendImageActor();
  ~vvBlendImageActor();

private:
  vvBlendImageActor(const vvBlendImageActor&);  // Not implemented.
  void operator=(const vvBlendImageActor&);  // Not implemented.
};

#endif
