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
