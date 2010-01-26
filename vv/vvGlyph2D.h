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
