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
#ifndef vvImageWriter_CXX
#define vvImageWriter_CXX

#include "vvImageWriter.h"
#include "vvImageWriter.txx"

#include <string.h>
//====================================================================
vvImageWriter::vvImageWriter() {
    mImage = NULL;
    mOutputFilename = "";
    mLastError = "";
    mUseAnObserver = false;
}
//====================================================================

//====================================================================
vvImageWriter::~vvImageWriter() {
}
//====================================================================

//====================================================================
void vvImageWriter::Update(int dim,std::string OutputPixelType) {
    //CALL_FOR_ALL_DIMS(dim,UpdateWithDim,inputPixelType);
    if (dim == 2)
        UpdateWithDim<2>(OutputPixelType);
    else if (dim == 3)
        UpdateWithDim<3>(OutputPixelType);
    else if (dim == 4)
        UpdateWithDim<4>(OutputPixelType);
    else
        std::cerr << "dim not know in Update ! " << std::endl;
}
//====================================================================

//====================================================================
void vvImageWriter::SetOutputFileName(std::string filename) {
    mOutputFilename = filename;
}
//====================================================================

#endif

