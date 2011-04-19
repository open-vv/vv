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
#ifndef CLITKHNDIMAGEIO_H
#define CLITKHNDIMAGEIO_H

// itk include
#include "itkImageIOBase.h"

#if defined (_MSC_VER) && (_MSC_VER < 1600)
//SR: taken from
//#include "msinttypes/stdint.h"
typedef unsigned int uint32_t;
#else
#include <stdint.h>
#endif

namespace clitk {
  
  //====================================================================
  // Class for reading Hnd Image file format
  class HndImageIO: public itk::ImageIOBase
  {
  public: 
	/** Standard class typedefs. */
  typedef HndImageIO              Self;
	typedef itk::ImageIOBase        Superclass;
	typedef itk::SmartPointer<Self> Pointer;	
	typedef signed short int        PixelType;
	
  typedef struct hnd_header {
      char sFileType[32];
      uint32_t FileLength;
      char sChecksumSpec[4];
      uint32_t nCheckSum;
      char sCreationDate[8];
      char sCreationTime[8];
      char sPatientID[16];
      uint32_t nPatientSer;
      char sSeriesID[16];
      uint32_t nSeriesSer;
      char sSliceID[16];
      uint32_t nSliceSer;
      uint32_t SizeX;
      uint32_t SizeY;
      double dSliceZPos;
      char sModality[16];
      uint32_t nWindow;
      uint32_t nLevel;
      uint32_t nPixelOffset;
      char sImageType[4];
      double dGantryRtn;
      double dSAD;
      double dSFD;
      double dCollX1;
      double dCollX2;
      double dCollY1;
      double dCollY2;
      double dCollRtn;
      double dFieldX;
      double dFieldY;
      double dBladeX1;
      double dBladeX2;
      double dBladeY1;
      double dBladeY2;
      double dIDUPosLng;
      double dIDUPosLat;
      double dIDUPosVrt;
      double dIDUPosRtn;
      double dPatientSupportAngle;
      double dTableTopEccentricAngle;
      double dCouchVrt;
      double dCouchLng;
      double dCouchLat;
      double dIDUResolutionX;
      double dIDUResolutionY;
      double dImageResolutionX;
      double dImageResolutionY;
      double dEnergy;
      double dDoseRate;
      double dXRayKV;
      double dXRayMA;
      double dMetersetExposure;
      double dAcqAdjustment;
      double dCTProjectionAngle;
      double dCTNormChamber;
      double dGatingTimeTag;
      double dGating4DInfoX;
      double dGating4DInfoY;
      double dGating4DInfoZ;
      double dGating4DInfoTime;
  } Hnd_header;

  HndImageIO():Superclass() {;}

	/** Method for creation through the object factory. */
	itkNewMacro(Self);
	
	/** Run-time type information (and related methods). */
  itkTypeMacro(HndImageIO, ImageIOBase);
	
	/*-------- This part of the interface deals with reading data. ------ */
	virtual void ReadImageInformation();
	virtual bool CanReadFile( const char* FileNameToRead );
	virtual void Read(void * buffer);

	/*-------- This part of the interfaces deals with writing data. ----- */
        virtual void WriteImageInformation(bool keepOfStream) { ; }
	virtual void WriteImageInformation() { WriteImageInformation(false); }
	virtual bool CanWriteFile(const char* filename);
	virtual void Write(const void* buffer);
	
  protected:
	int m_HeaderSize;

  }; // end class HndImageIO
} // end namespace

// explicit template instantiation
//template class itk::CreateObjectFunction<clitk::HndImageIO>;

#endif /* end #define CLITKHNDIMAGEIO_H */

