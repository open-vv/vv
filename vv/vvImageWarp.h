#ifndef vvImageWarp_h
#define vvImageWarp_h
#include "vvImage.h"
class QWidget;

/// Allows the computation of a warped image sequence and a difference image sequence, for the purpose of verifying registration results
class vvImageWarp
{
public:
    ///Slicer manager containing the image sequence and deformation field, plus the index of the image used as a reference for the registration
    vvImageWarp(vvImage::Pointer input,vvImage::Pointer vf,unsigned int ref_image,QWidget* parent);
    ///Computes a warped sequence and a difference image sequence. Return false in case of error
    bool ComputeWarpedImage();
    vvImage::Pointer GetWarpedImage() {
        return mWarpedImage;
    }
    vvImage::Pointer GetDiffImage() {
        return mDiffImage;
    }
    vvImage::Pointer GetJacobianImage() {
        return mJacobianImage;
    }

protected:
    template<unsigned int Dim, class PixelType> void Update_WithDimAndPixelType();
    template<unsigned int Dim> void Update_WithDim();
    unsigned int mRefImage;
    vvImage::Pointer mWarpedImage;
    vvImage::Pointer mDiffImage;
    vvImage::Pointer mJacobianImage;
    QWidget * parent_window;
    vvImage::Pointer mInputImage;
    vvImage::Pointer mVF;
};

#endif
