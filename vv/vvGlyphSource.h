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
