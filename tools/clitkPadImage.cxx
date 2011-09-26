#include "clitkCommon.h"
#include "clitkPadImage_ggo.h"
#include "clitkPadImageGenericFilter.h"

// template <class ImageType, class PadBoundType, unsigned int dim>
// int pad_like(typename ImageType::Pointer input, const std::string& likeFile, PadBoundType* padLower, PadBoundType* padUpper)
// {
//   typedef typename ImageType::SpacingType SpacingType;
//   typedef typename ImageType::RegionType RegionType;
//   typedef typename ImageType::SizeType SizeType;
//   typedef typename ImageType::IndexType IndexType;
//   typedef typename ImageType::PointType PointType;
//   typedef typename ImageType::PointValueType PointValueType;
// 
//   typedef itk::ImageFileReader<ImageType> ImageReaderType;
//   typename ImageReaderType::Pointer reader = ImageReaderType::New();
//   reader->SetFileName(likeFile);
//   reader->Update();
//   
//   typename ImageType::Pointer like_image = reader->GetOutput();
// 
//   SpacingType spacing = input->GetSpacing(), like_spacing = like_image->GetSpacing(); 
//   if (spacing != like_spacing) {
//     std::cerr << "Like-image must have same spacing as input: " << spacing << " " << like_spacing << std::endl;
//     return ERR_NOT_SAME_SPACING;
//   }
//   
//   SizeType size = input->GetLargestPossibleRegion().GetSize(), like_size = like_image->GetLargestPossibleRegion().GetSize();
//   PointType origin = input->GetOrigin(), like_origin = like_image->GetOrigin();
//   
//   PointType lower_bound, like_lower_bound;
//   PointType upper_bound, like_upper_bound;
//   PointValueType auxl = 0, auxu = 0;
//   for (unsigned int i = 0; i < dim; i++) {
//     lower_bound[i] = origin[i];
//     like_lower_bound[i] = like_origin[i];
//     auxl = itk::Math::Round<PointValueType>(((lower_bound[i] - like_lower_bound[i])/spacing[i]));
//     
//     upper_bound[i] = (lower_bound[i] + size[i]*spacing[i]);
//     like_upper_bound[i] = (like_lower_bound[i] + like_size[i]*spacing[i]);
//     auxu = itk::Math::Round<PointValueType>(((like_upper_bound[i] - upper_bound[i])/spacing[i]));
// 
//     if (auxl < 0 || auxu < 0) {
//       std::cerr << "Like-image's bounding box must be larger than input's" << std::endl;
//       return ERR_NOT_LIKE_LARGER;
//     }
// 
//     padLower[i] = (PadBoundType)auxl;
//     padUpper[i] = (PadBoundType)auxu;
//   }
//   
//   return ERR_SUCCESS;
// }

int main(int argc, char** argv)
{
/*  const DimType dim = 3;
  typedef short PixelType;
  typedef itk::Image<PixelType, dim> ImageType;
  typedef itk::ImageFileReader<ImageType> ImageReaderType;
  typedef itk::ImageFileWriter<ImageType> ImageWriterType;
  typedef itk::ConstantPadImageFilter<ImageType, ImageType> PadFilterType;
  typedef PadFilterType::SizeValueType PadBoundType;*/
  
  GGO(clitkPadImage, args_info);

  // Filter
  typedef clitk::PadImageGenericFilter FilterType;
  FilterType::Pointer filter = FilterType::New();

  filter->SetArgsInfo(args_info);

  try { 
    filter->Update();
  } catch(std::runtime_error e) {
    std::cout << e.what() << std::endl;
  }
  
//   ImageReaderType::Pointer reader = ImageReaderType::New();
//   reader->SetFileName(args_info.input_arg);
//   reader->Update();
//   
//   PadBoundType pad_lower[dim], pad_upper[dim];
//   ImageType::Pointer input = reader->GetOutput();
//   if (args_info.like_given) {
//     int err = pad_like<ImageType, PadBoundType, dim>(input, args_info.like_arg, pad_lower, pad_upper);
//     if (err) {
//       std::cerr << "Error processing like image." << std::endl;
//       return err;
//     }
//   }
//   else {
//     for (DimType i = 0; i < dim; i++) {
//       pad_lower[i] = args_info.lower_arg[i];
//       pad_upper[i] = args_info.upper_arg[i];
//     }
//   }
//   
//   PadFilterType::Pointer filter = PadFilterType::New();
//   filter->SetPadLowerBound(pad_lower);
//   filter->SetPadUpperBound(pad_upper);
//   filter->SetInput(input);
//   filter->SetConstant(args_info.value_arg);
//   filter->Update();
//   
//   ImageType::Pointer output = filter->GetOutput();
//   ImageWriterType::Pointer writer = ImageWriterType::New();
//   writer->SetInput(output);
//   writer->SetFileName(args_info.output_arg);
//   writer->Update();
  
  return EXIT_SUCCESS;
}

