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
#ifndef __vvGlyphSource_h
#define __vvGlyphSource_h
#include "vtkGlyphSource2D.h"

#define VTK_SPECIFICARROW_GLYPH 13

class vvGlyphSource: public vtkGlyphSource2D
{
public:
    static vvGlyphSource *New();
    vtkTypeMacro(vvGlyphSource,vtkGlyphSource2D);
#if VTK_MAJOR_VERSION >= 8
    void PrintSelf(ostream& os, vtkIndent indent) override;
#elif VTK_MAJOR_VERSION >= 7
    void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;
#else
    void PrintSelf(ostream& os, vtkIndent indent);
#endif

    void SetGlyphTypeToSpecificArrow() {
        this->SetGlyphType(VTK_SPECIFICARROW_GLYPH);
    }

    vtkSetClampMacro(GlyphType,int,VTK_NO_GLYPH,VTK_SPECIFICARROW_GLYPH);

protected:
    void CreateSpecificArrow(vtkPoints *pts, vtkCellArray *lines,
                             vtkCellArray *polys, vtkUnsignedCharArray *colors);
#if VTK_MAJOR_VERSION >= 8
    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
#elif VTK_MAJOR_VERSION >= 7
    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;
#else
    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
#endif

private:


};
#endif
