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

#ifndef __vvLandmarksGlyph_h
#define __vvLandmarksGlyph_h

#include "vtkTextSource.h"
#include "vtkPoints.h"

class vvLandmarksGlyph : public vtkTextSource
{
public:
    vtkTypeRevisionMacro(vvLandmarksGlyph,vtkTextSource);
    void PrintSelf(ostream& os, vtkIndent indent);

    // Description:
    // Construct object with no string set and backing enabled.
    static vvLandmarksGlyph *New();


protected:
    vvLandmarksGlyph();
    ~vvLandmarksGlyph();

    void AddTextGlyph(vtkPoints* newPoints,vtkUnsignedCharArray* newScalars, vtkCellArray *newPolys, int orientation);
    void AddCrossGlyph(vtkPoints* newPts,vtkCellArray* newLines);
    void ChangeOrientation(double v[3], int orientation);

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
    vvLandmarksGlyph(const vvLandmarksGlyph&);  // Not implemented.
    void operator=(const vvLandmarksGlyph&);  // Not implemented.
};

#endif
