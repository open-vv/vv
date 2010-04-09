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
#ifndef CLITKXDRIMAGEIO_H
#define CLITKXDRIMAGEIO_H

/**
 * @file   clitkXdrImageIO.h
 * @author Simon Rit <simon.rit@gmail.com>
 * @date   Sun Jun  1 22:07:45 2008
 *
 * @brief
 *
 *
 */

// itk include
#include "itkImageIOBase.h"

#define AVSerror(v)   itkGenericExceptionMacro(<< "Error in clitk::XdrImageIO. Message:" << v);
#define AVSwarning(v) itkWarningMacro(<< "Error in clitk::XdrImageIO. Message:" << v);

namespace clitk {

//====================================================================
// Class for reading xdr Image file format
class XdrImageIO: public itk::ImageIOBase
{
public:
    /** Standard class typedefs. */
    typedef XdrImageIO              Self;
    typedef itk::ImageIOBase        Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef signed short int        PixelType;
    typedef enum {UNIFORM,RECTILINEAR,IRREGULAR} AVSType;

    XdrImageIO():Superclass() {
        forcenoswap = 0;
    }

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(XdrImageIO, ImageIOBase);

    /*-------- This part of the interface deals with reading data. ------ */
    virtual int ReadImageInformationWithError();
    virtual int ReadWithError(void * buffer);
    virtual void ReadImageInformation();
    virtual void Read(void * buffer);
    virtual bool CanReadFile( const char* FileNameToRead );

    /*-------- This part of the interfaces deals with writing data. ----- */
    virtual void WriteImageInformation(bool keepOfStream) {;}
    virtual void WriteImageInformation() { WriteImageInformation(false); }
    virtual bool CanWriteFile(const char* FileNameToWrite);
    virtual void Write(const void* buffer);

protected:
    void ITKError(std::string funcName, int msgID);
    void WriteImage(const char* file, char* headerinfo, char* headerfile, int raw,
                    int offset, char bLittleEndian, int iNkiCompression,
                    int wcoords, int append, int getsize, char *tobuffer, const void* data);

    int m_HeaderSize;
    int forcenoswap;
}; // end class XdrImageIO

} // end namespace

// explicit template instantiation
//template class itk::CreateObjectFunction<clitk::XdrImageIO>;

#endif /* end #define CLITKXDRIMAGEIO_H */

