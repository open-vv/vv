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
#ifndef vvImageWriter_H
#define vvImageWriter_H

#include <string>

// ITK includes
#include "itkImage.h"
#include "itkCommand.h"
#include "vvImage.h"

class vvImageWriter {

public:
    // constructor
    vvImageWriter();
    ~vvImageWriter();

    void SetOutputFileName(std::string filename);
    void SetObserver(itk::Command::Pointer o) {
        mUseAnObserver = true;
        mObserver = o;
    }
    void DisableObserver() {
        mUseAnObserver = false;
    }

    void SetInput(vvImage::Pointer image) {
        mImage = image;
    }

    std::string GetLastError() {
        return mLastError;
    }

    //====================================================================
    // Main function
    void Update() {
        Update(mImage->GetNumberOfDimensions(),mImage->GetScalarTypeAsString());
    }
    void Update(int dim, std::string OutputPixelType);

protected:
    //====================================================================
    std::string mOutputFilename;
    itk::Command::Pointer mObserver;

    std::string mLastError;
    bool mUseAnObserver;

    //====================================================================
    template<unsigned int VImageDimension>
    void UpdateWithDim(std::string OutputPixelType);

    //====================================================================
    template<class OutputPixelType, unsigned int VImageDimension>
    void UpdateWithDimAndOutputPixelType();

private:
    vvImage::Pointer mImage;

}; // end class vvImageWriter

#endif /* end #define vvImageWriter_H */
