#include <itkImageToVTKImageFilter.h>

//--------------------------------------------------------------------
template<class TItkImageType>
void vvImage::AddItkImage(TItkImageType *input)
{
  mImageDimension = TItkImageType::ImageDimension; 
  typedef itk::ImageToVTKImageFilter <TItkImageType> ConverterType;
  typename ConverterType::Pointer converter = ConverterType::New();
  mItkToVtkConverters.push_back(dynamic_cast< itk::ProcessObject *>(converter.GetPointer()));
  converter->SetInput(input);
  converter->Update();
  mVtkImages.push_back( converter->GetOutput() );
 
  mVtkImageReslice.push_back(vtkSmartPointer<vtkImageReslice>::New());
  mVtkImageReslice.back()->SetInterpolationModeToLinear();
  mVtkImageReslice.back()->AutoCropOutputOn();
  mVtkImageReslice.back()->SetBackgroundColor(-1000,-1000,-1000,1);
  mVtkImageReslice.back()->SetResliceTransform(mTransform);
  mVtkImageReslice.back()->SetInput(0, converter->GetOutput());
  mVtkImageReslice.back()->UpdateInformation();
  mTransformedVtkImages.push_back( mVtkImageReslice.back()->GetOutput(0) );
}
//--------------------------------------------------------------------

