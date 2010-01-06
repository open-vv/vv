/*=========================================================================

 Program:   vv
 Module:    $RCSfile: vvImage.h,v $
 Language:  C++
 Date:      $Date: 2010/01/06 13:32:01 $
 Version:   $Revision: 1.1 $
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
#ifndef vvImage_h
#define vvImage_h

#include <iostream>
#include <vector>
#include <itkObjectFactory.h>

class vtkImageData;

class vvImage : public itk::LightObject
{
public :
    typedef vvImage Self;
    typedef itk::SmartPointer<Self> Pointer;
    itkNewMacro(Self);

    void Init();
    void SetImage(std::vector<vtkImageData*> images);
    void AddImage(vtkImageData* image);
    const std::vector<vtkImageData*>& GetVTKImages() {
        return mVtkImages;
    }

    int GetNumberOfDimensions() const;
    int GetNumberOfSpatialDimensions();
    ///Writes the scalar range to the provided array, which must have room for two doubles
    void GetScalarRange(double* range);
    unsigned long GetActualMemorySize();
    std::vector<double> GetSpacing();
    std::vector<double> GetOrigin() const;
    std::vector<int> GetSize();
    std::string GetScalarTypeAsString();
    int GetNumberOfScalarComponents();
    int GetScalarSize();
    bool IsTimeSequence() {
        return mVtkImages.size()>1;
    }

private:
    vvImage();
    ~vvImage();
    std::vector<vtkImageData*> mVtkImages;

};

#endif
