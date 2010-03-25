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

