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
