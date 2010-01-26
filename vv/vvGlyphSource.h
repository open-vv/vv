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
#ifndef __vvGlyphSource_h
#define __vvGlyphSource_h

#include "vtkGlyphSource2D.h"

#define VTK_SPECIFICARROW_GLYPH 13

class vvGlyphSource: public vtkGlyphSource2D
{
public:
    static vvGlyphSource *New();
    vtkTypeRevisionMacro(vvGlyphSource,vtkGlyphSource2D);
    void PrintSelf(ostream& os, vtkIndent indent);

    void SetGlyphTypeToSpecificArrow() {
        this->SetGlyphType(VTK_SPECIFICARROW_GLYPH);
    }

    vtkSetClampMacro(GlyphType,int,VTK_NO_GLYPH,VTK_SPECIFICARROW_GLYPH);

protected:
    void CreateSpecificArrow(vtkPoints *pts, vtkCellArray *lines,
                             vtkCellArray *polys, vtkUnsignedCharArray *colors);

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:


};
#endif
