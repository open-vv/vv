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
#ifndef clitkExtrudeGenericFilter_txx
#define clitkExtrudeGenericFilter_txx

// itk include
#include <itkImageFileReader.h>
#include <clitkCommon.h>

namespace clitk
{

//--------------------------------------------------------------------
template<class args_info_type>
ExtrudeGenericFilter<args_info_type>::ExtrudeGenericFilter():
  ImageToImageGenericFilter<Self>("Extrude")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dim>
void ExtrudeGenericFilter<args_info_type>::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
void ExtrudeGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a)
{
  mArgsInfo=a;
  this->SetIOVerbose(mArgsInfo.verbose_flag);

  if (mArgsInfo.input_given) {
    this->SetInputFilename(mArgsInfo.input_arg);
  }
  if (mArgsInfo.output_given) {
    this->SetOutputFilename(mArgsInfo.output_arg);
  }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class args_info_type>
template<class InputImageType>
void
ExtrudeGenericFilter<args_info_type>::UpdateWithInputImageType()
{

  // Reading input
  typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);

  // Main filter
  typedef typename InputImageType::PixelType PixelType;
  const int Dim = InputImageType::ImageDimension;
  typedef itk::Image<PixelType,Dim> ImageType;
  typedef itk::Image<PixelType,Dim+1> OutputImageType;

  //Create the output
  typename OutputImageType::IndexType start;
  typename OutputImageType::SizeType size;
  typename OutputImageType::PointType origin;
  typename OutputImageType::SpacingType spacing;
  typename OutputImageType::DirectionType direction;
  typename OutputImageType::Pointer output = OutputImageType::New();

  start.Fill(0);

  //Check if like is given and not size, origin and spacing
  int extrusionSize(1);
  double extrusionOrigin(0.0), extrusionSpacing(1.0);

  if (mArgsInfo.like_given) {
    if (mArgsInfo.size_given || mArgsInfo.spacing_given || mArgsInfo.origin_given) {
      std::cerr << "You cannot set --like and --size, --origin or --spacing at the same time" << std::endl;
      return;
    }

    // Read the input like image
    typedef itk::ImageFileReader<OutputImageType> LikeReaderType;
    typename LikeReaderType::Pointer reader = LikeReaderType::New();
    reader->SetFileName(mArgsInfo.like_arg);
    reader->Update();
    typename OutputImageType::Pointer likeImage = reader->GetOutput();

    extrusionSize = likeImage->GetLargestPossibleRegion().GetSize()[Dim];
    extrusionSpacing = likeImage->GetSpacing()[Dim];
    extrusionOrigin = likeImage->GetOrigin()[Dim];
  } else {
    if (mArgsInfo.size_given) {
      if (mArgsInfo.size_arg > 0)
        extrusionSize = mArgsInfo.size_arg;
      else {
        std::cerr << "The size has to be > 0" << std::endl;
        return;
      }
    }
    if (mArgsInfo.origin_given) {
      extrusionOrigin = mArgsInfo.origin_arg;
    }
    if (mArgsInfo.spacing_given) {
      if (mArgsInfo.spacing_arg > 0)
        extrusionSpacing = mArgsInfo.spacing_arg;
      else {
        std::cerr << "The spacing has to be > 0" << std::endl;
        return;
      }
    }
  }

  for (unsigned int i=0; i<Dim; ++i)
    size[i] = input->GetLargestPossibleRegion().GetSize()[i];
  size[Dim] = extrusionSize;

  for (unsigned int i=0; i<Dim; ++i)
    origin[i] = input->GetOrigin()[i];
  if (mArgsInfo.origin_given)
    origin[Dim] = mArgsInfo.origin_arg;
  else
    origin[Dim] = 0;

  for (unsigned int i=0; i<Dim; ++i)
    spacing[i] = input->GetSpacing()[i];
  if (mArgsInfo.spacing_given)
    spacing[Dim] = mArgsInfo.spacing_arg;
  else
    spacing[Dim] = 1;

  for (unsigned int i=0; i<Dim; ++i) {
    for (unsigned int j=0; j<Dim; ++j)
      direction[i][j] = input->GetDirection()[i][j];
    direction[i][Dim] = 0;
  }
  for (unsigned int i=0; i<Dim; ++i)
    direction[Dim][i] = 0;
  direction[Dim][Dim] = 1;

  typename OutputImageType::RegionType region(start, size);
  output->SetRegions(region);
  output->Allocate();
  output->FillBuffer(0);
  output->SetOrigin(origin);
  output->SetSpacing(spacing);
  output->SetDirection(direction);

  itk::ImageRegionIteratorWithIndex<InputImageType> inputIterator(input, input->GetLargestPossibleRegion());
  while(!inputIterator.IsAtEnd()) {
    typename OutputImageType::IndexType pixelIndex;
    for (unsigned int i=0; i<Dim; ++i)
      pixelIndex[i] = inputIterator.GetIndex()[i];
    for (unsigned int i=0; i<extrusionSize; ++i) {
      pixelIndex[Dim] = i;
      output->SetPixel(pixelIndex, inputIterator.Get());
    }
    ++inputIterator;
  }

  this->template SetNextOutput<OutputImageType>(output);

}
//--------------------------------------------------------------------


}//end clitk

#endif //#define clitkExtrudeGenericFilter_txx
