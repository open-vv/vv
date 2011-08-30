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
#ifndef CLITKIMAGECOMMON_TXX
#define CLITKIMAGECOMMON_TXX


//--------------------------------------------------------------------
template<class PixelType>
typename itk::Image<PixelType,1>::Pointer
NewImage1D(int vsize, double vspacing)
{
  typedef itk::Image<PixelType,1> ImageType;
  typename ImageType::Pointer g = ImageType::New();
  typename ImageType::SizeType size;
  size[0] = vsize;
  typename ImageType::RegionType region;
  region.SetSize(size);
  g->SetRegions(region);
  typename ImageType::SpacingType spacing;
  spacing[0] = vspacing;
  g->SetSpacing(spacing);
  return g;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class PixelType>
typename itk::Image<PixelType,2>::Pointer
NewImage2D(int sx, int sy, double dx, double dy)
{
  typedef itk::Image<PixelType,2> ImageType;
  typename ImageType::Pointer g = ImageType::New();
  typename ImageType::SizeType size;
  size[0] = sx;
  size[1] = sy;
  typename ImageType::RegionType region;
  region.SetSize(size);
  g->SetRegions(region);
  typename ImageType::SpacingType spacing;
  spacing[0] = dx;
  spacing[1] = dy;
  g->SetSpacing(spacing);
  return g;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class PixelType>
typename itk::Image<PixelType,3>::Pointer
NewImage3D(int sx, int sy, int sz, double dx, double dy, double dz)
{
  typedef itk::Image<PixelType,3> ImageType;
  typename ImageType::Pointer g = ImageType::New();
  typename ImageType::SizeType size;
  size[0] = sx;
  size[1] = sy;
  size[2] = sz;
  typename ImageType::RegionType region;
  region.SetSize(size);
  g->SetRegions(region);
  typename ImageType::SpacingType spacing;
  spacing[0] = dx;
  spacing[1] = dy;
  spacing[2] = dz;
  g->SetSpacing(spacing);
  return g;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class PixelType>
typename itk::Image<PixelType,4>::Pointer NewImage4D(int sx, int sy, int sz, int st, double dx, double dy, double dz, double dt)
{
  typedef itk::Image<PixelType,3> ImageType;
  typename ImageType::Pointer g = ImageType::New();
  typename ImageType::SizeType size;
  size[0] = sx;
  size[1] = sy;
  size[2] = sz;
  size[3] = st;
  typename ImageType::RegionType region;
  region.SetSize(size);
  g->SetRegions(region);
  typename ImageType::SpacingType spacing;
  spacing[0] = dx;
  spacing[1] = dy;
  spacing[2] = dz;
  spacing[3] = dt;
  g->SetSpacing(spacing);
  return g;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer NewImageLike(const typename ImageType::Pointer input, bool allocate)
{
  typename ImageType::Pointer output = ImageType::New();
  output->CopyInformation(input);
  output->SetRegions(input->GetLargestPossibleRegion());
  if (allocate) output->Allocate();
  return output;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void CopyValues(const typename ImageType::Pointer input,
                typename ImageType::Pointer output)
{
  typedef itk::ImageRegionConstIterator<ImageType> ConstIteratorType;
  ConstIteratorType pi(input,input->GetLargestPossibleRegion());
  pi.GoToBegin();
  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  IteratorType po(output,input->GetLargestPossibleRegion());
  po.GoToBegin();
  while (!pi.IsAtEnd()) {
    po.Set(pi.Get());
    ++pi;
    ++po;
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer readImage(const std::string & filename, const bool verbose)
{
  typedef itk::ImageFileReader<ImageType> ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(filename.c_str());
  if (verbose) {
    std::cout << "Reading [" << filename << "] ... " << std::endl;
  }
  try {
    reader->Update();
  } catch(itk::ExceptionObject & err) {
    std::cerr << "Exception while reading image [" << filename << "]" << std::endl;
    std::cerr << err << std::endl;
    exit(0);
  }
  return reader->GetOutput();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<typename ImageType>
typename ImageType::Pointer readImage(const std::vector<std::string> & filenames,
                                      const bool verbose)
{
  if (filenames.size() == 1) return readImage<ImageType>(filenames[0], verbose);
  typedef itk::ImageSeriesReader<ImageType> ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileNames(filenames);
  if (verbose) {
    std::cout << "Reading " << filenames[0] << " and others ..." << std::endl;
  }
  try {
    reader->Update();
  } catch( itk::ExceptionObject & err ) {
    std::cerr << "Error while reading " << filenames[0]
              << " or other files ..." << err << std::endl;
    exit(0);
  }
  return reader->GetOutput();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void writeImage(const typename ImageType::Pointer image, const std::string & filename, const bool verbose, const bool compression)
{
  return writeImage(image.GetPointer(), filename, verbose, compression);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void writeImage(const ImageType* image, const std::string & filename, const bool verbose, const bool compression)
{
  typedef itk::ImageFileWriter<ImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(filename.c_str());
  writer->SetInput(image);
  writer->SetUseCompression(compression);
  if (verbose) {
    std::cout << "Writing [" << filename << "] ... " << std::endl;
  }
  try {
    writer->Update();
  } catch( itk::ExceptionObject & err ) {
    std::cerr << "Exception while writing image [" << filename << "]" << std::endl;
    std::cerr << err << std::endl;
    exit(-1);
  }
}
// //--------------------------------------------------------------------

// //--------------------------------------------------------------------
// template<class ImageType>
// void writeImage(const typename ImageType::ConstPointer image, const std::string & filename, const bool verbose=false) {
//   typedef itk::ImageFileWriter<ImageType> WriterType;
//   typename WriterType::Pointer writer = WriterType::New();
//   writer->SetFileName(filename.c_str());
//   writer->SetInput(image);
//   if (verbose) {
//     std::cout << "Writing [" << filename << "] ... " << std::endl;
//   }
//   try {
//     writer->Update();
//   }
//   catch( itk::ExceptionObject & err ) {
//     std::cerr << "Exception while writing image [" << filename << "]" << std::endl;
//     std::cerr << err << std::endl;
//     exit(-1);
//   }
// }

//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Compute the number of different intensities in an image
template<class ImageType>
int ComputeHowManyDifferentIntensity(const typename ImageType::Pointer & image,
                                     std::vector<typename ImageType::PixelType> & l)
{
  //std::set<typename ImageType::PixelType> listOfIntensities;
  std::map<typename ImageType::PixelType, bool> listOfIntensities;
  //  listOfIntensities.resize(0);
  typedef itk::ImageRegionConstIterator<ImageType> ConstIteratorType;
  ConstIteratorType pi(image, image->GetLargestPossibleRegion());
  pi.Begin();
  while (!pi.IsAtEnd()) {
    if (!listOfIntensities[pi.Get()]) listOfIntensities[pi.Get()] = true;
    // if (std::find(listOfIntensities.begin(),
    // 				  listOfIntensities.end(),
    // 				  pi.Get()) == listOfIntensities.end()) {
    // 	  listOfIntensities.insert(pi.Get());
    // 	}
    ++pi;
  }

  //typename std::set<typename ImageType::PixelType>::const_iterator ppi = listOfIntensities.begin();
  typename std::map<typename ImageType::PixelType, bool>::const_iterator ppi = listOfIntensities.begin();
  while (ppi != listOfIntensities.end()) {
    l.push_back(ppi->first);
    ++ppi;
  }

  return listOfIntensities.size();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class InputImageType, class MaskImageType>
void ComputeWeightsOfEachClasses(const typename InputImageType::Pointer & input,
                                 const typename MaskImageType::Pointer & mask,
                                 const std::vector<typename MaskImageType::PixelType> & listOfIntensities,
                                 std::map<typename MaskImageType::PixelType,
                                 std::map<typename InputImageType::PixelType, double> > & mapOfLabelsAndWeights)
{
  // Check size
  if (input->GetLargestPossibleRegion() != mask->GetLargestPossibleRegion()) {
    itkGenericExceptionMacro(<< "Input and mask images have not the same size"
                             << std::endl
                             << "Input = " << input->GetLargestPossibleRegion()
                             << std::endl
                             << "Mask = " << mask->GetLargestPossibleRegion());
  }

  // reset weights list
  mapOfLabelsAndWeights.clear();

  // loop
  typedef itk::ImageRegionConstIterator<InputImageType> ConstInputIteratorType;
  ConstInputIteratorType pi(input, input->GetLargestPossibleRegion());
  typedef itk::ImageRegionConstIterator<MaskImageType> ConstMaskIteratorType;
  ConstMaskIteratorType pm(mask, mask->GetLargestPossibleRegion());
  pi.Begin();
  pm.Begin();
  while (!pi.IsAtEnd()) {
    mapOfLabelsAndWeights[pm.Get()][pi.Get()]++;
    ++pi;
    ++pm;
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType1, class ImageType2>
bool HaveSameSpacing(typename ImageType1::ConstPointer A,
                     typename ImageType2::ConstPointer B)
{
  if (A->GetImageDimension() != B->GetImageDimension()) return false;
  for(unsigned int i=0; i<A->GetImageDimension(); i++) {
    if (A->GetSpacing()[i] != B->GetSpacing()[i]) return false;
  }
  return true;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType1, class ImageType2>
bool HaveSameSpacing(typename ImageType1::Pointer A,
                     typename ImageType2::Pointer B)
{
  if (A->GetImageDimension() != B->GetImageDimension()) return false;
  for(unsigned int i=0; i<A->GetImageDimension(); i++) {
    if (A->GetSpacing()[i] != B->GetSpacing()[i]) return false;
  }
  return true;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType1, class ImageType2>
bool HaveSameSize(typename ImageType1::ConstPointer A,
                  typename ImageType2::ConstPointer B)
{
  if (A->GetImageDimension() != B->GetImageDimension()) return false;
  for(unsigned int i=0; i<A->GetImageDimension(); i++) {
    if (A->GetLargestPossibleRegion().GetSize()[i] != B->GetLargestPossibleRegion().GetSize()[i]) return false;
  }
  return true;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType1, class ImageType2>
bool HaveSameSize(typename ImageType1::Pointer A,
                  typename ImageType2::Pointer B)
{
  if (A->GetImageDimension() != B->GetImageDimension()) return false;
  for(unsigned int i=0; i<A->GetImageDimension(); i++) {
    if (A->GetLargestPossibleRegion().GetSize()[i] != B->GetLargestPossibleRegion().GetSize()[i]) return false;
  }
  return true;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType1, class ImageType2>
bool HaveSameSizeAndSpacing(typename ImageType1::ConstPointer A,
                            typename ImageType2::ConstPointer B)
{
  return ( HaveSameSize<ImageType1, ImageType2>(A, B) &&
           HaveSameSpacing<ImageType1, ImageType2>(A, B) );
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType1, class ImageType2>
bool HaveSameSizeAndSpacing(typename ImageType1::Pointer A,
                            typename ImageType2::Pointer B)
{
  return ( HaveSameSize<ImageType1, ImageType2>(A, B) &&
           HaveSameSpacing<ImageType1, ImageType2>(A, B) );
}
//--------------------------------------------------------------------

#endif /* end #define CLITKIMAGECOMMON_TXX */

