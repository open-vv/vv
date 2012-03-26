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
#ifndef vvImageWriter_H
#define vvImageWriter_H
#include <string>

// ITK includes
#include "itkImage.h"
#include "itkCommand.h"
#include "vvImage.h"

class vvImageWriter : public itk::LightObject {

public:
  typedef vvImageWriter Self;
  typedef itk::SmartPointer<Self> Pointer;
  itkNewMacro(Self);

  void SetOutputFileName(std::string filename);
  void SetObserver(itk::Command::Pointer o) {
    mUseAnObserver = true;
    mObserver = o;
  }
  void DisableObserver() {
    mUseAnObserver = false;
  }

  void SetInput(vvImage::Pointer image) {
    mImage = image;
  }

  std::string GetLastError() {
    return mLastError;
  }

  bool GetSaveTransform() { return mSaveTransform; }
  void SetSaveTransform(bool b) { mSaveTransform = b; }

  //====================================================================
  // Main function
  void Update() {
    Update(mImage->GetNumberOfDimensions(),mImage->GetScalarTypeAsITKString());
  }
  void Update(int dim, std::string OutputPixelType);

protected:
  // constructor
  vvImageWriter();
  ~vvImageWriter();

  //====================================================================
  std::string mOutputFilename;
  itk::Command::Pointer mObserver;

  std::string mLastError;
  bool mUseAnObserver;
  bool mSaveTransform;

  //====================================================================
  template<unsigned int VImageDimension>
  void UpdateWithDim(std::string OutputPixelType);

  //====================================================================
  template<class OutputPixelType, unsigned int VImageDimension>
  void UpdateWithDimAndOutputPixelType();

private:
  vvImage::Pointer mImage;

}; // end class vvImageWriter

#endif /* end #define vvImageWriter_H */
