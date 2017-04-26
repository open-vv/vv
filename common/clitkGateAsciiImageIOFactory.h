/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
===========================================================================**/
#ifndef CLITKGATEASCIIIMAGEIOFACTORY_H
#define CLITKGATEASCIIIMAGEIOFACTORY_H

// clitk include
#include "clitkGateAsciiImageIO.h"

// itk include
#include "itkImageIOBase.h"
#include "itkObjectFactoryBase.h"
#include "itkVersion.h"

namespace clitk {

    //====================================================================
    // Factory for reading GateAscii Image file format
    class GateAsciiImageIOFactory: public itk::ObjectFactoryBase
    {
	public:
	    /** Standard class typedefs. */
	    typedef GateAsciiImageIOFactory        Self;
	    typedef itk::ObjectFactoryBase         Superclass;
	    typedef itk::SmartPointer<Self>        Pointer;
	    typedef itk::SmartPointer<const Self>  ConstPointer;

	    /** Class methods used to interface with the registered factories. */
	    const char* GetITKSourceVersion(void) const ITK_OVERRIDE {
		return ITK_SOURCE_VERSION;
	    }

	    const char* GetDescription(void) const ITK_OVERRIDE {
		return "GateAscii ImageIO Factory, allows the loading of gate ascii images into insight";
	    }

	    /** Method for class instantiation. */
	    itkFactorylessNewMacro(Self);

	    /** Run-time type information (and related methods). */
	    itkTypeMacro(GateAsciiImageIOFactory, ObjectFactoryBase);

	    /** Register one factory of this type  */
	    static void RegisterOneFactory(void) {
		ObjectFactoryBase::RegisterFactory( Self::New() );
	    }	

	protected:
	    GateAsciiImageIOFactory();
	    ~GateAsciiImageIOFactory() {};

	private:
	    GateAsciiImageIOFactory(const Self&); //purposely not implemented
	    void operator=(const Self&); //purposely not implemented
    };

} // end namespace

#endif /* end #define CLITKGATEASCIIIMAGEIOFACTORY_H */

