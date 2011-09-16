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
#ifndef clitkPadImageGenericFilter_cxx
#define clitkPadImageGenericFilter_cxx

/* =================================================
 * @file   clitkPadImageGenericFilter.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/

#include "clitkPadImageGenericFilter.h"

// itk
#include <itkConstantPadImageFilter.h>


namespace clitk
{


  //-----------------------------------------------------------
  // Constructor
  //-----------------------------------------------------------
  PadImageGenericFilter::PadImageGenericFilter():
    ImageToImageGenericFilter<Self>("PadImage")
  {
    cmdline_parser_clitkPadImage_init(&mArgsInfo);
    InitializeImageType<2>();
    InitializeImageType<3>();
    //InitializeImageType<4>();
  }

  //--------------------------------------------------------------------
  template<unsigned int Dim>
  void PadImageGenericFilter::InitializeImageType()
  {
    ADD_DEFAULT_IMAGE_TYPES(Dim);
    //ADD_IMAGE_TYPE(Dim, uchar);
    //ADD_IMAGE_TYPE(Dim, short);
    // ADD_IMAGE_TYPE(Dim, uint);
    //  ADD_IMAGE_TYPE(Dim, ulong);
    // ADD_IMAGE_TYPE(Dim, int);
    // ADD_IMAGE_TYPE(Dim, float);
  }
  //--------------------------------------------------------------------

  //--------------------------------------------------------------------
  void PadImageGenericFilter::SetArgsInfo(const args_info_type& a) 
  {
    mArgsInfo=a;
    SetIOVerbose(mArgsInfo.verbose_flag);
    if (mArgsInfo.input_given)   AddInputFilename(mArgsInfo.input_arg);
    if (mArgsInfo.output_given)  AddOutputFilename(mArgsInfo.output_arg);
  }
  //--------------------------------------------------------------------

  //--------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //--------------------------------------------------------------------
  template<class ImageType>
  void PadImageGenericFilter::UpdateWithInputImageType() 
  { 
    typedef itk::ConstantPadImageFilter<ImageType, ImageType> PadFilterType;
    typedef typename PadFilterType::SizeValueType PadBoundType;

    // Reading input
    typename ImageType::Pointer input = this->template GetInput<ImageType>(0);

    const unsigned int dim = ImageType::ImageDimension;
    PadBoundType pad_lower[dim], pad_upper[dim];
    if (mArgsInfo.like_given) {
      int err = PadLike<ImageType, PadBoundType, ImageType::ImageDimension>(input, pad_lower, pad_upper);
      if (err) {
        std::cerr << "Error processing like image." << std::endl;
        return;
      }
    }
    else {
      if (mArgsInfo.lower_given != dim || mArgsInfo.upper_given != dim)
      {
        std::cerr << "The number of lower and upper padding parameters must be equal to the image dimension." << std::endl;
        return;
      }
      
      for (unsigned int i = 0; i < dim; i++) {
        pad_lower[i] = mArgsInfo.lower_arg[i];
        pad_upper[i] = mArgsInfo.upper_arg[i];
      }
    }
    
    typename PadFilterType::Pointer filter = PadFilterType::New();
    filter->SetPadLowerBound(pad_lower);
    filter->SetPadUpperBound(pad_upper);
    filter->SetInput(input);
    filter->SetConstant(mArgsInfo.value_arg);
    filter->Update();

    // Prepare output
    typename ImageType::Pointer output;
    output = filter->GetOutput();
      
    // Write/Save results
    this->template SetNextOutput<ImageType>(output); 
  }
  //--------------------------------------------------------------------

  //--------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //--------------------------------------------------------------------
  template <class ImageType, class PadBoundType, unsigned int dim>
  int PadImageGenericFilter::PadLike(typename ImageType::Pointer input, PadBoundType* padLower, PadBoundType* padUpper) 
  { 
    if (mArgsInfo.verbose_flag)
      std::cout << "PadLike - IN" << std::endl;
    
    typedef typename ImageType::SpacingType SpacingType;
    typedef typename ImageType::RegionType RegionType;
    typedef typename ImageType::SizeType SizeType;
    typedef typename ImageType::IndexType IndexType;
    typedef typename ImageType::PointType PointType;
    typedef typename ImageType::PointValueType PointValueType;

    if (mArgsInfo.verbose_flag)
      std::cout << "Reading like image: " << mArgsInfo.like_arg << ::endl;
    
    typedef itk::ImageFileReader<ImageType> ImageReaderType;
    typename ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName(mArgsInfo.like_arg);
    reader->Update();
    
    typename ImageType::Pointer like_image = reader->GetOutput();

    if (mArgsInfo.verbose_flag)
      std::cout << "Calculating padding." << std::endl;
    
    SpacingType spacing = input->GetSpacing(), like_spacing = like_image->GetSpacing(); 
    if (spacing != like_spacing) {
      std::cerr << "Like-image must have same spacing as input: " << spacing << " " << like_spacing << std::endl;
      return ERR_NOT_SAME_SPACING;
    }
    
    SizeType size = input->GetLargestPossibleRegion().GetSize(), like_size = like_image->GetLargestPossibleRegion().GetSize();
    PointType origin = input->GetOrigin(), like_origin = like_image->GetOrigin();
    
    PointType lower_bound, like_lower_bound;
    PointType upper_bound, like_upper_bound;
    PointValueType auxl = 0, auxu = 0;
    for (unsigned int i = 0; i < dim; i++) {
      lower_bound[i] = origin[i];
      like_lower_bound[i] = like_origin[i];
      auxl = itk::Math::Round<PointValueType>(((lower_bound[i] - like_lower_bound[i])/spacing[i]));
      
      upper_bound[i] = (lower_bound[i] + size[i]*spacing[i]);
      like_upper_bound[i] = (like_lower_bound[i] + like_size[i]*spacing[i]);
      auxu = itk::Math::Round<PointValueType>(((like_upper_bound[i] - upper_bound[i])/spacing[i]));

      if (auxl < 0 || auxu < 0) {
        std::cerr << "Like-image's bounding box must be larger than input's" << std::endl;
        return ERR_NOT_LIKE_LARGER;
      }

      padLower[i] = (PadBoundType)auxl;
      padUpper[i] = (PadBoundType)auxu;
    }
    
    if (mArgsInfo.verbose_flag)
      std::cout << "PadLike - OUT" << std::endl;
    return ERR_SUCCESS;
  }
} //end clitk

#endif  //#define clitkPadImageGenericFilter_cxx
