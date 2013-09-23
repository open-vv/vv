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

#ifndef CLITKRESAMPLEIMAGEGENERICFILTER_TXX
#define CLITKRESAMPLEIMAGEGENERICFILTER_TXX

// clitk
#include "clitkResampleImageWithOptionsFilter.h"

//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class InputImageType>
void
clitk::ResampleImageGenericFilter::UpdateWithInputImageType()
{

  // Reading input
  typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);

  // Main filter
  typedef typename InputImageType::PixelType PixelType;
  typedef InputImageType OutputImageType; // to change to float is user ask it (?)

  // Filter
  typedef clitk::ResampleImageWithOptionsFilter<InputImageType, OutputImageType> ResampleImageFilterType;
  typename ResampleImageFilterType::Pointer filter = ResampleImageFilterType::New();
  filter->SetInput(input);

  // Set Verbose
  filter->SetVerboseOptions(mArgsInfo.verbose_flag);

  // Set size / spacing
  static const unsigned int dim = OutputImageType::ImageDimension;
  typename OutputImageType::SpacingType spacing;
  typename OutputImageType::SizeType size;
  typename OutputImageType::PointType origin;
  typename OutputImageType::DirectionType direction;

  if (mArgsInfo.like_given) {
    itk::ImageIOBase::Pointer header = clitk::readImageHeader(mArgsInfo.like_arg);
    if (header) {
      for(unsigned int i=0; i<dim; i++){
        spacing[i] = header->GetSpacing(i);
        size[i] = header->GetDimensions(i);
        origin[i] = header->GetOrigin(i);
      }
      for(unsigned int i=0; i<dim; i++) {
        for(unsigned int j=0;j<dim;j++) {
            direction(i,j) = header->GetDirection(i)[j];
        }
      }
      filter->SetOutputSpacing(spacing);
      filter->SetOutputSize(size);
      filter->SetOutputOrigin(origin);
      filter->SetOutputDirection(direction);
    }
    else {
      std::cerr << "*** Warning : I could not read '" << mArgsInfo.like_arg << "' ***" << std::endl;
      exit(0);
    }
  }
  else {
    if (mArgsInfo.spacing_given == 1) {
      filter->SetOutputIsoSpacing(mArgsInfo.spacing_arg[0]);
    }
    else if ((mArgsInfo.spacing_given != 0) && (mArgsInfo.size_given != 0)) {
      std::cerr << "Error: use spacing or size, not both." << std::endl;
      exit(0);
    }
    else if (mArgsInfo.spacing_given) {
      if ((mArgsInfo.spacing_given != 0) && (mArgsInfo.spacing_given != dim)) {
        std::cerr << "Error: spacing should have one or " << dim << " values." << std::endl;
        exit(0);
      }
      for(unsigned int i=0; i<dim; i++)
        spacing[i] = mArgsInfo.spacing_arg[i];
      filter->SetOutputSpacing(spacing);
    }
    else if (mArgsInfo.size_given) {
      if ((mArgsInfo.size_given != 0) && (mArgsInfo.size_given != dim)) {
        std::cerr << "Error: size should have " << dim << " values." << std::endl;
        exit(0);
      }
      for(unsigned int i=0; i<dim; i++)
        size[i] = mArgsInfo.size_arg[i];
      filter->SetOutputSize(size);
    }
    itk::ImageIOBase::Pointer header = clitk::readImageHeader(mArgsInfo.input_arg);
    for(unsigned int i=0; i<dim; i++){
      origin[i] = header->GetOrigin(i);
    }
    for(unsigned int i=0; i<dim; i++) {
      for(unsigned int j=0;j<dim;j++) {
          direction(i,j) = header->GetDirection(i)[j];
      }
    }
    filter->SetOutputOrigin(origin);
    filter->SetOutputDirection(direction);
  }

  // Set temporal dimension
  filter->SetLastDimensionIsTime(mArgsInfo.time_flag);

  // Set Gauss
  filter->SetGaussianFilteringEnabled(mArgsInfo.autogauss_flag);
  if (mArgsInfo.gauss_given != 0) {
    typename ResampleImageFilterType::GaussianSigmaType g;
    for(unsigned int i=0; i<dim; i++) {
      g[i] = mArgsInfo.gauss_arg[i];
    }
    filter->SetGaussianSigma(g);
  }

  // Set Interpolation
  std::string interp = std::string(mArgsInfo.interp_arg);
  if (interp == "nn") {
    filter->SetInterpolationType(ResampleImageFilterType::NearestNeighbor);
  } else {
    if (interp == "linear") {
      filter->SetInterpolationType(ResampleImageFilterType::Linear);
    } else {
      if (interp == "bspline") {
        filter->SetInterpolationType(ResampleImageFilterType::BSpline);
      } else {
        if (interp == "blut") {
          filter->SetInterpolationType(ResampleImageFilterType::B_LUT);
        } else {
          if (interp == "windowed sinc") {
            filter->SetInterpolationType(ResampleImageFilterType::WSINC);
          } else {
            std::cerr << "Error. I do not know interpolation '" << mArgsInfo.interp_arg
                      << "'. Choose among: nn, linear, bspline, blut, windowed sinc" << std::endl;
            exit(0);
          }
        }
      }
    }
  }

  // Set default pixel value
  filter->SetDefaultPixelValue(mArgsInfo.default_arg);

  // Set thread
  if (mArgsInfo.thread_given) {
    filter->SetNumberOfThreads(mArgsInfo.thread_arg);
  }

  // Go !
  filter->Update();
  typename OutputImageType::Pointer outputImage = filter->GetOutput();
  this->template SetNextOutput<OutputImageType>(outputImage);
}
//--------------------------------------------------------------------

#endif /* end #define CLITKRESAMPLEIMAGEGENERICFILTER_TXX */

