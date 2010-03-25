#ifndef vvMaximumIntensityProjection_h
#define vvMaximumIntensityProjection_h
#include "vvImage.h"
class vvSlicerManager;

class vvMaximumIntensityProjection
{
public:
    vvMaximumIntensityProjection() {};
    ///Computes the MIP image on the given vvImage
    void Compute(vvSlicerManager*);
    vvImage::Pointer GetOutput() {return mOutputImage;};

protected:
    template<class PixelType,int Dim> void Update_WithDimAndPixelType(vvImage::Pointer);
    template<class PixelType> void Update_WithPixelType(vvImage::Pointer);
    vvImage::Pointer mOutputImage;
};

#endif
