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
#ifndef __vvLandmarksGlyph_h
#define __vvLandmarksGlyph_h
#include "vtkTextSource.h"
#include "vtkPoints.h"

class vvLandmarksGlyph : public vtkTextSource
{
public:
    vtkTypeMacro(vvLandmarksGlyph,vtkTextSource);
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
