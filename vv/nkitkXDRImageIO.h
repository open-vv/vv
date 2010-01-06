#ifndef NKITKXDRIMAGEIO_H
#define NKITKXDRIMAGEIO_H

/**
 * @file   nkitkXDRImageIO.h
 * @author Simon Rit <simon.rit@gmail.com>
 * @date   Sun Jun  1 22:07:45 2008
 *
 * @brief
 *
 *
 */

// itk include
#include "itkImageIOBase.h"

namespace nkitk {

//====================================================================
// Class for reading XDR Image file format
class XDRImageIO: public itk::ImageIOBase
{
public:
    /** Standard class typedefs. */
    typedef XDRImageIO              Self;
    typedef itk::ImageIOBase        Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef signed short int        PixelType;
    typedef enum {UNIFORM,RECTILINEAR,IRREGULAR} AVSType;

    XDRImageIO():Superclass() {
        forcenoswap = 0;
    }

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(XDRImageIO, ImageIOBase);

    /*-------- This part of the interface deals with reading data. ------ */
    virtual int ReadImageInformationWithError();
    virtual int ReadWithError(void * buffer);
    virtual void ReadImageInformation();
    virtual void Read(void * buffer);
    virtual bool CanReadFile( const char* FileNameToRead );

    /*-------- This part of the interfaces deals with writing data. ----- */
    virtual void WriteImageInformation(bool keepOfStream) {
        ;
    }
    virtual void WriteImageInformation() {
        WriteImageInformation(false);
    }
    virtual bool CanWriteFile(const char* filename) {
        return false;
    }
    virtual void Write(const void* buffer) {
        ;
    }

protected:
    void ITKError(std::string funcName, int msgID);

    int m_HeaderSize;
    int forcenoswap;
}; // end class XDRImageIO

} // end namespace

// explicit template instantiation
template class itk::CreateObjectFunction<nkitk::XDRImageIO>;

#endif /* end #define NKITKXDRIMAGEIO_H */

