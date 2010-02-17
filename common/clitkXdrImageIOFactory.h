#ifndef CLITKXDRIMAGEIOFACTORY_H
#define CLITKXDRIMAGEIOFACTORY_H

/**
 * @file   clitkXdrImageIOFactory.h
 * @author Simon Rit <simon.rit@gmail.com>
 * @date   Sun Jun  1 22:09:56 2008
 *
 * @brief
 *
 *
 */

// clitk include
#include "clitkXdrImageIO.h"

// itk include
#include "itkImageIOBase.h"
#include "itkObjectFactoryBase.h"
#include "itkVersion.h"

namespace clitk {

//====================================================================
// Factory for reading XDR Image file format
class XdrImageIOFactory: public itk::ObjectFactoryBase
{
public:
    /** Standard class typedefs. */
    typedef XdrImageIOFactory              Self;
    typedef itk::ObjectFactoryBase         Superclass;
    typedef itk::SmartPointer<Self>        Pointer;
    typedef itk::SmartPointer<const Self>  ConstPointer;

    /** Class methods used to interface with the registered factories. */
    const char* GetITKSourceVersion(void) const {
        return ITK_SOURCE_VERSION;
    }

    const char* GetDescription(void) const {
        return "Xdr ImageIO Factory, allows the loading of Xdr images into insight";
    }

    /** Method for class instantiation. */
    itkFactorylessNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(XdrImageIOFactory, ObjectFactoryBase);

    /** Register one factory of this type  */
    static void RegisterOneFactory(void) {
        ObjectFactoryBase::RegisterFactory( Self::New() );
    }

protected:
    XdrImageIOFactory();
    ~XdrImageIOFactory() {};
    typedef XdrImageIOFactory myProductType;
    const myProductType* m_MyProduct;

private:
    XdrImageIOFactory(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
};

} // end namespace

#endif /* end #define CLITKXDRIMAGEIOFACTORY_H */

