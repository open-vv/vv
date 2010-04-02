/*=========================================================================
  Program:         vv http://www.creatis.insa-lyon.fr/rio/vv
  Main authors :   XX XX XX

  Authors belongs to: 
  - University of LYON           http://www.universite-lyon.fr/
  - Léon Bérard cancer center    http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory      http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence
  - BSD       http://www.opensource.org/licenses/bsd-license.php
  - CeCILL-B  http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html

  =========================================================================*/

#ifndef CLITKDICOMRT_ROI_CONVERTTOIMAGEFILTER_H
#define CLITKDICOMRT_ROI_CONVERTTOIMAGEFILTER_H

#include "clitkDicomRT_ROI.h"
#include "clitkImageCommon.h"
#include <vtkImageData.h>

namespace clitk {

  //--------------------------------------------------------------------
  class DicomRT_ROI_ConvertToImageFilter {
    
  public:
    DicomRT_ROI_ConvertToImageFilter();
    ~DicomRT_ROI_ConvertToImageFilter();

    void SetROI(clitk::DicomRT_ROI * roi);
    void SetImageFilename(std::string s);
    void SetOutputImageFilename(std::string s);
    void Update();    
    vtkImageData * GetOutput();
    void SetCropMaskEnabled(bool b);

  protected:
    bool mImageInfoIsSet;
    bool mWriteOutput;
    bool mCropMask;
    std::string mOutputFilename;
    std::vector<double> mSpacing;
    std::vector<double> mOrigin;
    std::vector<int> mSize;
    clitk::DicomRT_ROI * mROI;
    vtkImageData * mBinaryImage;
  };
  //--------------------------------------------------------------------

} // end namespace clitk
#endif // CLITKDICOMRT_ROI_CONVERTTOIMAGEFILTER_H

