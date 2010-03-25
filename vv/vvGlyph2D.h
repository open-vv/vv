#ifndef __vvGlyph2D_h
#define __vvGlyph2D_h
#include "vtkGlyph3D.h"

class vvGlyph2D : public vtkGlyph3D
{
public:
    vtkTypeRevisionMacro(vvGlyph2D,vtkGlyph3D);
    void PrintSelf(ostream& os, vtkIndent indent);

    // Description
    // Construct object with scaling on, scaling mode is by scalar value,
    // scale factor = 1.0, the range is (0,1), orient geometry is on, and
    // orientation is by vector. Clamping and indexing are turned off. No
    // initial sources are defined.
    void SetOrientation(int x,int y,int z);
    static vvGlyph2D *New();

    void SetUseLog(int log) {
        mUseLog = log;
    }
    int GetUseLog() {
        return mUseLog;
    }

protected:
    vvGlyph2D();
    ~vvGlyph2D() {};

    virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
    vvGlyph2D(const vvGlyph2D&);  // Not implemented.
    void operator=(const vvGlyph2D&);  // Not implemented.
    double mOrientation[3];
    int mUseLog;
};

#endif
