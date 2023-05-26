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
#ifndef __vtkVOXImageWriter_h
#define __vtkVOXImageWriter_h
#include "vtkImageWriter.h"
#include <string>
#include <vtkVersion.h>


class vtkVOXImageWriter : public vtkImageWriter
{
public:
    vtkTypeMacro(vtkVOXImageWriter,vtkImageWriter);
#if VTK_MAJOR_VERSION >= 8
    void PrintSelf(ostream& os, vtkIndent indent) override;
#elif VTK_MAJOR_VERSION >= 7
    void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;
#else
    void PrintSelf(ostream& os, vtkIndent indent);
#endif

    // Description:
    static vtkVOXImageWriter *New();

    // Description:
    // Specify file name of meta file
    vtkSetStringMacro(FileName);

    // This is called by the superclass.
    // This is the method you should override.
#if VTK_MAJOR_VERSION >= 8
    virtual void Write() override;
#elif VTK_MAJOR_VERSION >= 7
    virtual void Write() VTK_OVERRIDE;
#else
    virtual void Write();
#endif

protected:
    vtkVOXImageWriter();
    ~vtkVOXImageWriter();

private:
    vtkVOXImageWriter(const vtkVOXImageWriter&);  // Not implemented.
    void operator=(const vtkVOXImageWriter&);  // Not implemented.


};

#endif
