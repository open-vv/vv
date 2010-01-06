#ifndef NKITKXDRIMAGEIOFACTORY_H
#define NKITKXDRIMAGEIOFACTORY_H

/**
 * @file   nkitkXDRImageIOFactory.h
 * @author Simon Rit <simon.rit@gmail.com>
 * @date   Sun Jun  1 22:09:56 2008
 *
 * @brief
 *
 *
 */

// nkitk include
#include "nkitkXDRImageIO.h"

// itk include
#include "itkImageIOBase.h"
#include "itkObjectFactoryBase.h"
#include "itkVersion.h"

namespace nkitk {

//====================================================================
// Factory for reading XDR Image file format
class XDRImageIOFactory: public itk::ObjectFactoryBase
{
public:
    /** Standard class typedefs. */
    typedef XDRImageIOFactory              Self;
    typedef itk::ObjectFactoryBase         Superclass;
    typedef itk::SmartPointer<Self>        Pointer;
    typedef itk::SmartPointer<const Self>  ConstPointer;

    /** Class methods used to interface with the registered factories. */
    const char* GetITKSourceVersion(void) const {
        return ITK_SOURCE_VERSION;
    }

    const char* GetDescription(void) const {
        return "XDR ImageIO Factory, allows the loading of XDR images into insight";
    }

    /** Method for class instantiation. */
    itkFactorylessNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(XDRImageIOFactory, ObjectFactoryBase);

    /** Register one factory of this type  */
    static void RegisterOneFactory(void) {
        ObjectFactoryBase::RegisterFactory( Self::New() );
    }

protected:
    XDRImageIOFactory();
    ~XDRImageIOFactory() {};
    typedef XDRImageIOFactory myProductType;
    const myProductType* m_MyProduct;

private:
    XDRImageIOFactory(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
};

} // end namespace

#endif /* end #define NKITKXDRIMAGEIOFACTORY_H */

