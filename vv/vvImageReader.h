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
#ifndef vvImageReader_H
#define vvImageReader_H

#include <string>
#include <vector>
#include <QThread>

#include "itkCommand.h"
#include "vvImage.h"
#include "vvConstants.h"

class vvImageReader : public QThread {

public:
    vvImageReader();
    ~vvImageReader();

    void SetInputFilename(const std::string & filename);
    void SetInputFilenames(const std::vector<std::string> & filenames);

    vvImage::Pointer GetOutput() {
        return mImage;
    }

    std::string GetLastError() {
        return mLastError;
    }

    //====================================================================
    // Main function
    void Update();
    void Update(LoadedImageType type);
    void Update(int dim, std::string InputPixelType, LoadedImageType type);
    //void Extract(int dim, std::string InputPixelType, int slice);

protected:
    void run();
    //====================================================================
    std::vector<std::string> mInputFilenames;
    ///Method used to load the image, see vvConstants.h for definition
    LoadedImageType mType;
    itk::Command::Pointer mObserver;

    std::string mLastError;

    //====================================================================
    template<unsigned int VImageDimension>
    void UpdateWithDim(std::string inputPixelType);

    //====================================================================
    /*template<unsigned int VImageDimension>
    void ExtractWithDim(std::string inputPixelType, int slice);*/

    //====================================================================
    template<class InputPixelType, unsigned int VImageDimension>
    void UpdateWithDimAndInputPixelType();
    ///Input dimension and pixel type
    int mDim;
    std::string mInputPixelType;

private:
    vvImage::Pointer mImage;

}; // end class vvImageReader

#endif /* end #define CLITKvvImageReader_H */

