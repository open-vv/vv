#ifndef CLITKVOXIMAGEIOFACTORY_H
#define CLITKVOXIMAGEIOFACTORY_H

/**
 ===================================================================
 * @file   clitkVoxImageIOFactory.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   03 Jul 2006 11:27:55

 * @brief  

 ===================================================================*/

// clitk include
#include "clitkVoxImageIO.h"

// itk include
#include "itkImageIOBase.h"
#include "itkObjectFactoryBase.h"
#include "itkVersion.h"

namespace clitk {
  
  //====================================================================
  // Factory for reading Vox Image file format
  class VoxImageIOFactory: public itk::ObjectFactoryBase
  {
  public:
	/** Standard class typedefs. */
	typedef VoxImageIOFactory              Self;
	typedef itk::ObjectFactoryBase         Superclass;
	typedef itk::SmartPointer<Self>        Pointer;
	typedef itk::SmartPointer<const Self>  ConstPointer;
	
	/** Class methods used to interface with the registered factories. */
	const char* GetITKSourceVersion(void) const {
	  return ITK_SOURCE_VERSION;
	}
	
	const char* GetDescription(void) const {
	  return "Vox ImageIO Factory, allows the loading of Vox images into insight";
	}
	
	/** Method for class instantiation. */
	itkFactorylessNewMacro(Self);
	
	/** Run-time type information (and related methods). */
	itkTypeMacro(VoxImageIOFactory, ObjectFactoryBase);
	
	/** Register one factory of this type  */
	static void RegisterOneFactory(void) {
	  ObjectFactoryBase::RegisterFactory( Self::New() );
	}	

  protected:
	VoxImageIOFactory();
	~VoxImageIOFactory() {};
	typedef VoxImageIOFactory myProductType;
	const myProductType* m_MyProduct;
	
  private:
	VoxImageIOFactory(const Self&); //purposely not implemented
	void operator=(const Self&); //purposely not implemented
  };

} // end namespace

#endif /* end #define CLITKVOXIMAGEIOFACTORY_H */

