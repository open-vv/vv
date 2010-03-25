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
