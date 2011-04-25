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
#ifndef clitkCropImageGenericFilter_cxx
#define clitkCropImageGenericFilter_cxx

/* =================================================
 * @file   clitkCropImageGenericFilter.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/

#include "clitkCropImageGenericFilter.h"


namespace clitk
{


  //-----------------------------------------------------------
  // Constructor
  //-----------------------------------------------------------
  CropImageGenericFilter::CropImageGenericFilter():
    ImageToImageGenericFilter<Self>("CropImage")
  {
    cmdline_parser_clitkCropImage_init(&mArgsInfo);
    InitializeImageType<2>();
    InitializeImageType<3>();
    InitializeImageType<4>();
  }

  //--------------------------------------------------------------------
  template<unsigned int Dim>
  void clitk::CropImageGenericFilter::InitializeImageType()
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
  void clitk::CropImageGenericFilter::SetArgsInfo(const args_info_type& a) 
  {
    mArgsInfo=a;
    SetIOVerbose(mArgsInfo.verbose_flag);
    if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
    if (mArgsInfo.input_given)   AddInputFilename(mArgsInfo.input_arg);
    if (mArgsInfo.output_given)  AddOutputFilename(mArgsInfo.output_arg);
  }
  //--------------------------------------------------------------------

  //--------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //--------------------------------------------------------------------
  template<class ImageType>
  void clitk::CropImageGenericFilter::UpdateWithInputImageType() 
  { 
    // Reading input
    typename ImageType::Pointer input = this->template GetInput<ImageType>(0);

    // Check options
    if (mArgsInfo.BG_given && mArgsInfo.like_given)
      clitkExceptionMacro("Do not use --BG and --like at the same time");    

    // Prepare output
    typename ImageType::Pointer output;
    
    // ------------------------------------------------
    if (mArgsInfo.BG_given) { // AutoCrop filter
      if (mArgsInfo.boundingBox_given) 
        clitkExceptionMacro("Do not use --BG and --boundingBox at the same time");    
      if (mArgsInfo.lower_given) 
        clitkExceptionMacro("Do not use --BG and --lower at the same time");    
      if (mArgsInfo.upper_given) 
        clitkExceptionMacro("Do not use --BG and --upper at the same time");    
      typedef clitk::AutoCropFilter<ImageType> FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->SetInput(input);
      filter->SetBackgroundValue(mArgsInfo.BG_arg);
      filter->Update();
      output = filter->GetOutput();
    }
    else {
      // ------------------------------------------------
      if (mArgsInfo.like_given) { // CropLike filter
      if (mArgsInfo.boundingBox_given) 
        clitkExceptionMacro("Do not use --like and --boundingBox at the same time");    
      if (mArgsInfo.lower_given) 
        clitkExceptionMacro("Do not use --like and --lower at the same time");    
      if (mArgsInfo.upper_given) 
        clitkExceptionMacro("Do not use --like and --upper at the same time");    
        typedef clitk::CropLikeImageFilter<ImageType> FilterType;
        typename FilterType::Pointer filter = FilterType::New();
        filter->SetInput(input);
        filter->SetCropLikeFilename(mArgsInfo.like_arg);
        filter->SetBackgroundValue(mArgsInfo.BGLike_arg);
        filter->Update();
        output = filter->GetOutput();
      }
      else {
        // ------------------------------------------------
        typename ImageType::SizeType lSize;
        typename ImageType::SizeType uSize;
        if (mArgsInfo.boundingBox_given) {
          for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
            lSize[i] = mArgsInfo.boundingBox_arg[2*i];
            uSize[i] = input->GetLargestPossibleRegion().GetSize()[i]-mArgsInfo.boundingBox_arg[2*i+1]-1;
          }
        }
        else {
          if (mArgsInfo.lower_given) {
            for(unsigned int i=0; i<ImageType::ImageDimension; i++) 
              lSize[i]=static_cast<unsigned int >(mArgsInfo.lower_arg[i]);
          }
          else lSize.Fill(0);
          if (mArgsInfo.upper_given) {
            for(unsigned int i=0; i<ImageType::ImageDimension; i++)
              uSize[i]=static_cast<unsigned int >(mArgsInfo.upper_arg[i]);
          }
          else uSize.Fill(0);
        }
        typedef  itk::CropImageFilter<ImageType, ImageType> CropImageFilterType;
        typename CropImageFilterType::Pointer filter=CropImageFilterType::New();
        filter->SetInput(input);
        filter->SetLowerBoundaryCropSize(lSize);
        filter->SetUpperBoundaryCropSize(uSize);
        filter->Update();
        output = filter->GetOutput();    
      }
    }

    // Force origin if needed
    if (mArgsInfo.origin_flag) {
      typename ImageType::PointType origin;
      origin.Fill(itk::NumericTraits<double>::Zero);
      output->SetOrigin(origin);
    }

    // Write/Save results
    this->template SetNextOutput<ImageType>(output); 
  }
  //--------------------------------------------------------------------

} //end clitk

#endif  //#define clitkCropImageGenericFilter_cxx
