#include <iostream>
using std::cout;
using std::endl;

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkShiftScaleImageFilter.h>
#include <itkImageConstIteratorWithIndex.h>
#include <itkChangeInformationImageFilter.h>
#include <itkSignedDanielssonDistanceMapImageFilter.h>

#include "clitkGammaIndex_ggo.h"

const unsigned int image_dim = 2;

typedef itk::Image<float,image_dim> Image;
typedef itk::ImageRegionIteratorWithIndex<Image> ImageIterator;
typedef itk::ImageRegionConstIteratorWithIndex<Image> ImageConstIterator;
typedef itk::ImageFileReader<Image> Reader;
typedef itk::MinimumMaximumImageCalculator<Image> MinMaxer;
typedef itk::ShiftScaleImageFilter<Image,Image> Normalizer;
typedef itk::ChangeInformationImageFilter<Image> Scaler;

typedef itk::Image<unsigned char,image_dim+1> ImageBin;
typedef itk::Image<float,image_dim+1> ImageMap;
typedef itk::SignedDanielssonDistanceMapImageFilter<ImageBin,ImageMap> Mapper;

template <typename ImageType>
void SaveImage(const ImageType *image, const std::string &filename) {
  typedef typename itk::ImageFileWriter<ImageType> Writer;
  typename Writer::Pointer writer = Writer::New();
  writer->SetFileName(filename);
  writer->SetInput(image);
  writer->Update();
}

ImageBin::Pointer AllocateImageBin(const Image *reference, const Image *target, unsigned int dose_size, float dose_max) {
  const Image::RegionType reference_region = reference->GetLargestPossibleRegion();
  const Image::RegionType target_region = target->GetLargestPossibleRegion();
  const Image::SpacingType reference_spacing = reference->GetSpacing();
  const Image::SpacingType target_spacing = target->GetSpacing();
  const Image::PointType reference_origin = reference->GetOrigin();
  const Image::PointType target_origin = target->GetOrigin();

  assert(reference_region == target_region);
  assert(reference_spacing == target_spacing);
  assert(reference_origin == target_origin);

  ImageBin::RegionType region;
  ImageBin::SpacingType spacing;
  ImageBin::PointType origin;
  for (unsigned long kk=0; kk<Image::GetImageDimension(); kk++) {
    region.SetSize(kk,reference_region.GetSize(kk));
    region.SetIndex(kk,reference_region.GetIndex(kk));
    spacing[kk] = reference_spacing[kk];
    origin[kk] = reference_origin[kk];
  }
  const unsigned long dose_dimension = Image::GetImageDimension();
  region.SetSize(dose_dimension,dose_size);
  region.SetIndex(dose_dimension,0);
  spacing[dose_dimension] = dose_max/(dose_size-1);
  origin[dose_dimension] = 0;

  ImageBin::Pointer map = ImageBin::New();
  map->SetRegions(region);
  map->SetSpacing(spacing);
  map->SetOrigin(origin);
  map->Allocate();
  map->FillBuffer(0);

  return map;
}

Image::Pointer AllocateImageGamma(const Image *target) {
  const Image::RegionType target_region = target->GetLargestPossibleRegion();
  const Image::SpacingType target_spacing = target->GetSpacing();
  const Image::PointType target_origin = target->GetOrigin();

  Image::Pointer gamma = Image::New();
  gamma->SetRegions(target_region);
  gamma->SetSpacing(target_spacing);
  gamma->SetOrigin(target_origin);
  gamma->Allocate();
  gamma->FillBuffer(0);

  return gamma;
}

void FillImageBin(ImageBin *image_bin, const Image *reference) {
  ImageConstIterator iterator(reference,reference->GetLargestPossibleRegion());
  iterator.GoToBegin();
  while (!iterator.IsAtEnd()) {
    Image::PixelType value = iterator.Get();
    Image::PointType point;
    reference->TransformIndexToPhysicalPoint(iterator.GetIndex(),point);

    ImageBin::PointType point_bin;
    for (unsigned long kk=0; kk<Image::GetImageDimension(); kk++) {
      point_bin[kk] = point[kk];
    }
    point_bin[Image::GetImageDimension()] = value;

    ImageBin::IndexType index_bin;
#ifdef NDEBUG
    image_bin->TransformPhysicalPointToIndex(point_bin,index_bin);
#else
    bool found = image_bin->TransformPhysicalPointToIndex(point_bin,index_bin);
    assert(found);
#endif

    while (index_bin[Image::GetImageDimension()] >=0 ) {
      image_bin->SetPixel(index_bin,255);
      index_bin[Image::GetImageDimension()]--;
    }
    
    ++iterator;
  }
}

void FillImageGamma(Image *gamma, const Image *target, const ImageMap *distance) {
  ImageIterator gamma_iterator(gamma,gamma->GetLargestPossibleRegion());
  ImageConstIterator target_iterator(target,target->GetLargestPossibleRegion());
  gamma_iterator.GoToBegin();
  target_iterator.GoToBegin();
  while (!target_iterator.IsAtEnd()) {
    assert(!gamma_iterator.IsAtEnd());

    Image::PixelType value = target_iterator.Get();
    Image::PointType point;
    target->TransformIndexToPhysicalPoint(target_iterator.GetIndex(),point);

    ImageMap::PointType point_map;
    point_map.Fill(0);
    for (unsigned long kk=0; kk<Image::GetImageDimension(); kk++) {
      point_map[kk] = point[kk];
    }
    point_map[Image::GetImageDimension()] = value;

    ImageMap::IndexType index_map;
#ifdef NDEBUG
    distance->TransformPhysicalPointToIndex(point_map,index_map);
#else
    bool found = distance->TransformPhysicalPointToIndex(point_map,index_map);
    assert(found);
#endif

    gamma_iterator.Set(fabsf(distance->GetPixel(index_map)));
    
    ++gamma_iterator;
    ++target_iterator;
  }
}

void TuneScaler(Scaler *scaler,float space_margin) {
  Scaler::InputImageType::PointType origin = scaler->GetInput()->GetOrigin();
  Scaler::InputImageType::SpacingType spacing = scaler->GetInput()->GetSpacing();
  for (unsigned int kk=0; kk<Scaler::InputImageType::GetImageDimension(); kk++) {
    origin[kk] /= space_margin;
    spacing[kk] /= space_margin;
  }

  scaler->SetOutputSpacing(spacing);
  scaler->SetOutputOrigin(origin);
  scaler->ChangeSpacingOn();
  scaler->ChangeOriginOn();
}

Image::PixelType GetImageMaximum(const Image *image) {
    MinMaxer::Pointer minmaxer = MinMaxer::New();
    minmaxer->SetImage(image);
    minmaxer->ComputeMaximum();
    return minmaxer->GetMaximum();
}

void ComputeGammaRatio(const Image *image) {
  ImageConstIterator iterator(image,image->GetLargestPossibleRegion());
  iterator.GoToBegin();
  unsigned long total = 0;
  unsigned long over_one = 0;
  while (!iterator.IsAtEnd()) {
    Image::PixelType value = iterator.Get();

    if (value>1) over_one++;
    total++;

    ++iterator;
  }

  cout << "total=" << total << endl;
  cout << "over_one=" << over_one << endl;
  cout << "ratio=" << static_cast<float>(over_one)/total << endl;
}

int main(int argc,char * argv[])
{
  args_info_clitkGammaIndex args_info;

  if (cmdline_parser_clitkGammaIndex(argc, argv, &args_info) != 0)
    exit(1);

  if (!args_info.absolute_dose_margin_given && !args_info.relative_dose_margin_given) {
    std::cerr << "Specify either relative or absolute dose margin" << endl;
    exit(1);
  }

  if (args_info.isodose_number_arg <= 0) {
    std::cerr << "Specify a valid isodose number (>0)" << endl;
    exit(1);
  }

  bool verbose = args_info.verbose_flag;

  std::string reference_filename(args_info.reference_arg);
  std::string target_filename(args_info.target_arg);
  std::string gamma_filename(args_info.output_arg);
  float space_margin = args_info.spatial_margin_arg;
  float dose_rel_margin = args_info.relative_dose_margin_arg;
  float dose_margin = args_info.absolute_dose_margin_arg;
  bool use_dose_margin = args_info.absolute_dose_margin_given;
  unsigned int dose_size = args_info.isodose_number_arg;

  if (verbose) {
    cout << "reference_filename=" << reference_filename << endl;
    cout << "target_filename=" << target_filename << endl;
    cout << "gamma_filename=" << gamma_filename << endl;
    cout << "dose_size=" << dose_size << endl;
    cout << "space_margin=" << space_margin << endl;
    if (use_dose_margin) cout << "dose_margin=" << dose_margin << endl;
    else cout << "dose_rel_margin=" << dose_rel_margin << endl;
  }

  // load images
  Reader::Pointer reference_reader = Reader::New();
  Reader::Pointer target_reader = Reader::New();
  {
    reference_reader->SetFileName(reference_filename);
    target_reader->SetFileName(target_filename);
    reference_reader->Update();
    target_reader->Update();
  }

  // intensity normalisation
  if (!use_dose_margin) {
    MinMaxer::PixelType reference_max = GetImageMaximum(reference_reader->GetOutput());
    //MinMaxer::PixelType target_max = GetImageMaximum(target_reader->GetOutput());

    dose_margin = reference_max*dose_rel_margin;

    if (verbose) cout << "dose_margin=" << dose_margin << endl;
  }

  // scale intensity
  Normalizer::Pointer reference_normalizer = Normalizer::New();
  Normalizer::Pointer target_normalizer = Normalizer::New();
  {
    reference_normalizer->SetShift(0);
    reference_normalizer->SetScale(1/dose_margin);
    reference_normalizer->SetInput(reference_reader->GetOutput());
    reference_normalizer->Update();

    target_normalizer->SetShift(0);
    target_normalizer->SetScale(1/dose_margin);
    target_normalizer->SetInput(target_reader->GetOutput());
    target_normalizer->Update();

    //cout << "scale=" << reference_normalizer->GetScale() << "/" << target_normalizer->GetScale() << endl;
    //cout << "shift=" << reference_normalizer->GetShift() << "/" << target_normalizer->GetShift() << endl;
  }

  // normalize space coordinates
  Scaler::Pointer reference_scaler = Scaler::New();
  Scaler::Pointer target_scaler = Scaler::New();
  {
    reference_scaler->SetInput(reference_normalizer->GetOutput());
    TuneScaler(reference_scaler,space_margin);
    reference_scaler->Update();

    target_scaler->SetInput(target_normalizer->GetOutput());
    TuneScaler(target_scaler,space_margin);
    target_scaler->Update();

    //SaveImage(reference_scaler->GetOutput(),"norm_reference.mhd");
    //SaveImage(reference_scaler->GetOutput(),"norm_target.mhd");
  }

  // compute hyper surface plane
  float reference_dose_scaled_max = GetImageMaximum(reference_scaler->GetOutput());
  float target_dose_scaled_max = GetImageMaximum(target_scaler->GetOutput());
  float dose_scaled_max = reference_dose_scaled_max > target_dose_scaled_max ? reference_dose_scaled_max : target_dose_scaled_max;
  ImageBin::Pointer image_bin = AllocateImageBin(reference_scaler->GetOutput(),target_scaler->GetOutput(),dose_size,dose_scaled_max);
  FillImageBin(image_bin,reference_scaler->GetOutput());
  //SaveImage(image_bin.GetPointer(),"surface.mhd");

  // compute distance map
  Mapper::Pointer mapper = Mapper::New();
  mapper->InsideIsPositiveOn();
  mapper->UseImageSpacingOn();
  mapper->SetInput(image_bin);
  mapper->Update();
  //SaveImage(mapper->GetOutput(),"distance.mhd");

  // extract gamma index from distance map
  Image::Pointer image_gamma = AllocateImageGamma(target_normalizer->GetOutput());
  FillImageGamma(image_gamma,target_scaler->GetOutput(),mapper->GetOutput());
  SaveImage(image_gamma.GetPointer(),gamma_filename);

  if (verbose) ComputeGammaRatio(image_gamma);

  return 0;
}

