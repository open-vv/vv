#include <itkImageToVTKImageFilter.h>

//--------------------------------------------------------------------
template<class TItkImageType>
void vvImage::AddItkImage(TItkImageType *input)
{
  mImageDimension = TItkImageType::ImageDimension; 
  typedef itk::ImageToVTKImageFilter <TItkImageType> ConverterType;
  typename ConverterType::Pointer converter = ConverterType::New();
  converter->SetInput(input);
  converter->Update();

  mItkToVtkConverters.push_back(dynamic_cast< itk::ProcessObject *>(converter.GetPointer()));
  mVtkImages.push_back(converter->GetOutput());
 
  //mVtkImageReslice.push_back(vtkSmartPointer<vtkImageReslice>::New());
  //mVtkImageReslice.back()->SetInterpolationModeToLinear();
  //mVtkImageReslice.back()->AutoCropOutputOn();
  //mVtkImageReslice.back()->SetBackgroundColor(-1000,-1000,-1000,1);
  //mVtkImageReslice.back()->SetResliceTransform(mTransform);
  //mVtkImageReslice.back()->SetInput(0, image);
  //mVtkImageReslice.back()->Update();
  //mVtkImages.push_back( mVtkImageReslice.back()->GetOutput(0) );
}
//--------------------------------------------------------------------

