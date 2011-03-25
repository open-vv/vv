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
#include <itkImage.h>
#include <itkVTKImageToImageFilter.h>

namespace clitk {

  //--------------------------------------------------------------------
  class DicomRT_ROI_ConvertToImageFilter {
    
  public:
    DicomRT_ROI_ConvertToImageFilter();
    ~DicomRT_ROI_ConvertToImageFilter();

    void SetROI(clitk::DicomRT_ROI * roi);
    ///This is used to create a mask with the same characteristics as an input image
    void SetImageFilename(std::string s);
    void SetOutputOrigin(const double* origin);
    void SetOutputSpacing(const double* spacing);
    void SetOutputSize(const unsigned long* size);
    void SetOutputImageFilename(std::string s);
    void Update();    
    vtkImageData * GetOutput();
    template <int Dimension> typename itk::Image<unsigned char,Dimension>::ConstPointer GetITKOutput();
    void SetCropMaskEnabled(bool b);

  protected:
    bool ImageInfoIsSet() const;
    bool mWriteOutput;
    bool mCropMask;
    std::string mOutputFilename;
    std::vector<double> mSpacing;
    std::vector<double> mOrigin;
    std::vector<unsigned long> mSize;
    clitk::DicomRT_ROI * mROI;
    vtkSmartPointer<vtkImageData> mBinaryImage;
  };
  //--------------------------------------------------------------------

} // end namespace clitk


//--------------------------------------------------------------------

template <int Dimension> 
typename itk::Image<unsigned char,Dimension>::ConstPointer clitk::DicomRT_ROI_ConvertToImageFilter::GetITKOutput()
{
  assert(mBinaryImage);
  typedef itk::Image<unsigned char,Dimension> ConnectorImageType;
  typedef itk::VTKImageToImageFilter <ConnectorImageType> ConnectorType;
  typename ConnectorType::Pointer connector = ConnectorType::New();
  connector->SetInput(mBinaryImage);
  connector->Update();
  return connector->GetOutput();
}
//--------------------------------------------------------------------
#endif // CLITKDICOMRT_ROI_CONVERTTOIMAGEFILTER_H

