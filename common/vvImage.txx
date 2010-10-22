#include <itkImageToVTKImageFilter.h>

//--------------------------------------------------------------------
template<class TItkImageType>
void vvImage::AddItkImage(TItkImageType *input)
{
  mImageDimension = TItkImageType::ImageDimension; 
  typedef itk::ImageToVTKImageFilter <TItkImageType> ConverterType;
  typename ConverterType::Pointer converter = ConverterType::New();
  converter->SetInput(input);

  try {
      converter->Update();
  }
  catch ( itk::ExceptionObject & err ) {
      std::cerr << "Error while setting vvImage from ITK"
                << " " << err << std::endl;
  }
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

