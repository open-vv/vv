/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/
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
