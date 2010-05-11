/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/

#ifndef CLITKRESAMPLEIMAGEGENERICFILTER_TXX
#define CLITKRESAMPLEIMAGEGENERICFILTER_TXX

// clitk
#include "clitkResampleImageWithOptionsFilter.h"

namespace clitk {

  //--------------------------------------------------------------------
  template<class args_info_type>
  ResampleImageGenericFilter<args_info_type>::ResampleImageGenericFilter():
    ImageToImageGenericFilter<Self>("Resample") {
    InitializeImageType<2>();
    InitializeImageType<3>();
    InitializeImageType<4>();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class args_info_type>
  template<unsigned int Dim>
  void ResampleImageGenericFilter<args_info_type>::InitializeImageType() {      
    ADD_DEFAULT_IMAGE_TYPES(Dim);
    //ADD_IMAGE_TYPE(Dim, short);
  }
  //--------------------------------------------------------------------
  

  //--------------------------------------------------------------------
  template<class args_info_type>
  void ResampleImageGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a) {
    mArgsInfo=a;
    if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
    SetIOVerbose(mArgsInfo.verbose_flag);
    if (mArgsInfo.input_given) {
      SetInputFilename(mArgsInfo.input_arg);
    }
    if (mArgsInfo.output_given) {
      SetOutputFilename(mArgsInfo.output_arg);
    }
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //--------------------------------------------------------------------
  template<class args_info_type>
  template<class InputImageType>
  void 
  ResampleImageGenericFilter<args_info_type>::UpdateWithInputImageType() {

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
    if (mArgsInfo.spacing_given == 1) {
      filter->SetOutputIsoSpacing(mArgsInfo.spacing_arg[0]);
    }
    else {
      if ((mArgsInfo.spacing_given != 0) && (mArgsInfo.size_given != 0)) {
         std::cerr << "Error: use spacing or size, not both." << std::endl;
         exit(0);
      }

      if (!((mArgsInfo.spacing_given == 0) && (mArgsInfo.size_given == 0))) {

	if ((mArgsInfo.spacing_given != 0) && (mArgsInfo.spacing_given != dim)) {
	  std::cerr << "Error: spacing should have one or " << dim << " values." << std::endl;
	  exit(0);
	}
	if ((mArgsInfo.size_given != 0) && (mArgsInfo.size_given != dim)) {
	  std::cerr << "Error: size should have " << dim << " values." << std::endl;
	  exit(0);
	}
        if (mArgsInfo.spacing_given)
          for(unsigned int i=0; i<dim; i++) 
            spacing[i] = mArgsInfo.spacing_arg[i];
        if (mArgsInfo.size_given)
          for(unsigned int i=0; i<dim; i++) 
	  size[i] = mArgsInfo.size_arg[i];
	filter->SetOutputSpacing(spacing);
	filter->SetOutputSize(size);
      }
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
    }
    else {
      if (interp == "linear") {
        filter->SetInterpolationType(ResampleImageFilterType::Linear);
      }
      else {
        if (interp == "bspline") {
          filter->SetInterpolationType(ResampleImageFilterType::BSpline);
        }
        else {
          if (interp == "blut") {
            filter->SetInterpolationType(ResampleImageFilterType::B_LUT);
          }
          else {
            std::cerr << "Error. I do not know interpolation '" << mArgsInfo.interp_arg 
                      << "'. Choose among: nn, linear, bspline, blut" << std::endl;
            exit(0);
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

}

#endif /* end #define CLITKRESAMPLEIMAGEGENERICFILTER_TXX */

