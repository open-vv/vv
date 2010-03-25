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
