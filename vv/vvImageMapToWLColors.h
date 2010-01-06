/*=========================================================================

 Program:   vv
 Language:  C++
 Author :   Joel Schaerer (joel.schaerer@insa-lyon.fr)
 Program:   vv

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
#ifndef vvImageMapToWLColors_h
#define vvImageMapToWLColors_h

#include <vtkImageMapToWindowLevelColors.h>

//This is mostly a copy of the vtk parent class, but with the option
//not to use the W/L when a LUT is set

class vvImageMapToWLColors : public vtkImageMapToWindowLevelColors
{
public:
    static vvImageMapToWLColors * New();
    vvImageMapToWLColors();
    void SetWindowLevelMode(bool wl) {wl_mode=wl;}
    void ThreadedRequestData(vtkInformation *request,
            vtkInformationVector **inputVector,
            vtkInformationVector *outputVector,
            vtkImageData ***inData, vtkImageData **outData,
            int extent[6], int id);

protected:
    bool wl_mode;

};

#endif
