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
