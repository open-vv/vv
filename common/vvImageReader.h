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
#ifndef vvImageReader_H
#define vvImageReader_H
#include <string>
#include <vector>

#include "itkCommand.h"
#include "vvImage.h"

class vvImageReader : public itk::LightObject {

public:
  //Image types
  typedef enum _IMAGETYPE {
  IMAGE = 20,
  DICOM,
  MERGED,
  SLICED,
  IMAGEWITHTIME,
  MERGEDWITHTIME,
  VECTORFIELD,
  VECTORFIELDWITHTIME,
  VECTORPIXELIMAGE,
  VECTORPIXELIMAGEWITHTIME,
  UNDEFINEDIMAGETYPE
  } LoadedImageType;

  typedef vvImageReader Self;
  typedef itk::SmartPointer<Self> Pointer;
  itkNewMacro(Self);

  const std::vector<std::string>& GetInputFilenames(){return mInputFilenames;}

  void SetInputFilename(const std::string & filename);
  void SetInputFilenames(const std::vector<std::string> & filenames);

  vvImage::Pointer GetOutput() {
      return mImage;
  }

  std::string GetLastError() {
      return mLastError;
  }

  void SetSlice(unsigned int i) { mSlice = i; }
  void SetPatientCoordinateSystem(bool patientCoordinateSystem);

  //====================================================================
  // Main function
  void Update();
  void Update(LoadedImageType type);
  void Update(int dim, std::string InputPixelType, LoadedImageType type);

protected:
  //====================================================================
  std::vector<std::string> mInputFilenames;
  ///Method used to load the image, see vvConstants.h for definition
  LoadedImageType mType;
  unsigned int mSlice;
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
  template<class InputPixelType, unsigned int VImageDimension>
  void UpdateWithDimAndInputVectorPixelType();
  ///Input dimension and pixel type
  int mDim;
  bool mPatientCoordinateSystem;
  std::string mInputPixelType;

  //====================================================================
//  void ReadNkiImageTransform();
  void ReadMatImageTransform();
private:
  vvImageReader();
  ~vvImageReader();

  vvImage::Pointer mImage;
}; // end class vvImageReader

#endif /* end #define CLITKvvImageReader_H */

