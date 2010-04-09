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
#ifndef CLITKVFIMAGEIO_H
#define CLITKVFIMAGEIO_H

/**
 * @file   clitkVfImageIO.h
 * @author Simon Rit <Simon Rit <simon.rit@gmail.com>>
 * @date   Mon Sep 18 10:13:21 2006
 * 
 * @brief  VectorField .vf I/O header
 * 
 * 
 */


// clitk include
#include "clitkCommon.h"

// itk include
#include "itkImageIOBase.h"

// std include 
#include <fstream>

namespace clitk {
  
  //====================================================================
  // Class for reading Vf Image file format
  class VfImageIO: public itk::ImageIOBase
  {
  public: 
	/** Standard class typedefs. */
	typedef VfImageIO              Self;
	typedef itk::ImageIOBase        Superclass;
	typedef itk::SmartPointer<Self> Pointer;	
	typedef signed short int        PixelType;
	
	VfImageIO():Superclass() { mustWriteHeader = false; }

	/** Method for creation through the object factory. */
	itkNewMacro(Self);
	
	/** Run-time type information (and related methods). */
	itkTypeMacro(VfImageIO, ImageIOBase);
	
	/*-------- This part of the interface deals with reading data. ------ */
	virtual void ReadImageInformation();
	virtual bool CanReadFile( const char* FileNameToRead );
	virtual void Read(void * buffer);

	/*-------- This part of the interfaces deals with writing data. ----- */
	virtual void WriteImageInformation(bool keepOfStream);
	virtual void WriteImageInformation() { WriteImageInformation(false); }
	virtual bool CanWriteFile(const char* filename);
	virtual void Write(const void* buffer);
	
  protected:
	bool mustWriteHeader;
	int m_HeaderSize;
	std::ofstream file;

  }; // end class VfImageIO  

} // end namespace

  // explicit template instantiation
//template class itk::CreateObjectFunction<clitk::VfImageIO>;

#endif /* end #define CLITKVFIMAGEIO_H */

