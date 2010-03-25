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

