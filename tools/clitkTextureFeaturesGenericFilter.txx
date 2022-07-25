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
#ifndef clitkTextureFeaturesGenericFilter_txx
#define clitkTextureFeaturesGenericFilter_txx

namespace clitk
{

//--------------------------------------------------------------------
template<class args_info_type>
TextureFeaturesGenericFilter<args_info_type>::TextureFeaturesGenericFilter():
  ImageToImageGenericFilter<Self>("TextureFeatures")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dim>
void TextureFeaturesGenericFilter<args_info_type>::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
void TextureFeaturesGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a)
{
  this->m_ArgsInfo=a;
  this->m_Verbose = m_ArgsInfo.verbose_flag;
  this->SetIOVerbose(this->m_Verbose);
  if (m_ArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();

  if (m_ArgsInfo.input_given) {
    this->AddInputFilename(m_ArgsInfo.input_arg);
  }
  if (m_ArgsInfo.mask_given) {
    this->AddInputFilename(m_ArgsInfo.mask_arg);
  }
  if (m_ArgsInfo.nbin_given) {
    this->m_NBin = m_ArgsInfo.nbin_arg;
  } else {
    this->m_NBin = 256;
  }
  if (m_ArgsInfo.minr_given) {
    this->m_minRange = m_ArgsInfo.minr_arg;
  } else {
    this->m_minRange = -1;
  }
  if (m_ArgsInfo.maxr_given) {
    this->m_maxRange = m_ArgsInfo.maxr_arg;
  } else {
    this->m_maxRange = -1;
  }
  this->m_diag = m_ArgsInfo.diag_flag;
  if (m_ArgsInfo.output_given) {
    this->SetOutputFilename(m_ArgsInfo.output_arg);
  }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class args_info_type>
template<class InputImageType>
void
TextureFeaturesGenericFilter<args_info_type>::UpdateWithInputImageType()
{
  // Reading inputs
  typedef itk::Image<double, InputImageType::ImageDimension> OutputImageType;
  typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);
  typename OutputImageType::Pointer maskImage = NULL;
  if (m_ArgsInfo.mask_given) {
      maskImage = this->template GetInput<OutputImageType>(1);
  } else {
      maskImage = OutputImageType::New();
      typename OutputImageType::IndexType start;
      for(unsigned i=0;i<OutputImageType::ImageDimension;i++) {
          start[i] = 0;  // first index on X
      }
      typename OutputImageType::SizeType  size;
      for(unsigned i=0;i<OutputImageType::ImageDimension;i++) {
          size[i] = input->GetLargestPossibleRegion().GetSize()[i];  // size along i
      }
      typename OutputImageType::RegionType region;
      region.SetSize(size);
      region.SetIndex(start);
      maskImage->SetRegions(region);
      maskImage->SetOrigin(input->GetOrigin());
      maskImage->SetSpacing(input->GetSpacing());
      maskImage->SetDirection(input->GetDirection());
      maskImage->Allocate();
      maskImage->FillBuffer(1);
  }
  typedef itk::MaskImageFilter< InputImageType, OutputImageType, OutputImageType > MaskFilterType;
  typename MaskFilterType::Pointer maskFilter = MaskFilterType::New();
  maskFilter->SetInput(input);
  maskFilter->SetOutsideValue(-itk::NumericTraits< double >::max());
  maskFilter->SetMaskImage(maskImage);
  //Find the max first
  typedef itk::MinimumMaximumImageFilter<OutputImageType> MinimumMaximumImageFilterType;
  typename MinimumMaximumImageFilterType::Pointer maxFilter=MinimumMaximumImageFilterType::New();
  maxFilter->SetInput(maskFilter->GetOutput());
  maxFilter->Update();
  typename OutputImageType::PixelType maxImg = maxFilter->GetMaximum();
  //
  typedef itk::MaskImageFilter< InputImageType, OutputImageType, OutputImageType > MaskFilterType;
  typename MaskFilterType::Pointer maskFilter2 = MaskFilterType::New();
  maskFilter2->SetInput(input);
  maskFilter2->SetOutsideValue(itk::NumericTraits< double >::max());
  maskFilter2->SetMaskImage(maskImage);
  //Find the min
  typename MinimumMaximumImageFilterType::Pointer minFilter=MinimumMaximumImageFilterType::New();
  minFilter->SetInput(maskFilter2->GetOutput());
  minFilter->Update();
  typename OutputImageType::PixelType minImg = minFilter->GetMinimum();
  if(m_Verbose) {
      std::cout<<"Min of input image = "<<minImg<<std::endl;
      std::cout<<"Max of input image = "<<maxImg<<std::endl;
  }
  if(this->m_minRange == -1) {
      this->m_minRange = std::floor(minImg*10000.0)/10000.0-1.0/10000.0;
  } else {
      if(minImg <= this->m_minRange) {
          itk::ImageRegionIterator<InputImageType> itII(input,input->GetLargestPossibleRegion());
          itII.GoToBegin();
          while(!itII.IsAtEnd()) {
              if(itII.Get() <= this->m_minRange) {
                  itII.Set(this->m_minRange+1.0/10000.0);
                  //itII.Set(this->m_minRange);
              }
              ++itII;
          }
          //this->m_minRange = this->m_minRange-1.0/10000.0;
      }
  }
  if(this->m_maxRange == -1) {
      this->m_maxRange = std::ceil(maxImg*10000.0)/10000.0+1.0/10000.0;
  } else {
      if(maxImg >= this->m_maxRange) {
          itk::ImageRegionIterator<InputImageType> itII(input,input->GetLargestPossibleRegion());
          itII.GoToBegin();
          while(!itII.IsAtEnd()) {
              if(itII.Get() >= this->m_maxRange) {
                  itII.Set(this->m_maxRange-1.0/10000.0);
                  //itII.Set(this->m_maxRange);
              }
              ++itII;
          }
          //this->m_maxRange = this->m_maxRange+1.0/10000.0;
      }
  }
  if(m_Verbose) {
      std::cout<<"Min range is set to "<<this->m_minRange<<std::endl;
      std::cout<<"Max range is set to "<<this->m_maxRange<<std::endl;
  }
  //Before using ScalarImageToTextureFeaturesFilter, let's do the rescaling + rounding
  typedef itk::SubtractImageFilter <InputImageType, OutputImageType, OutputImageType> SubtractImageFilterType;
  typename SubtractImageFilterType::Pointer subtractConstantFromImageFilter = SubtractImageFilterType::New();
  subtractConstantFromImageFilter->SetInput(input);
  subtractConstantFromImageFilter->SetConstant2(this->m_minRange);
  //Let's do the multiplication
  typedef itk::MultiplyImageFilter<OutputImageType, OutputImageType, OutputImageType> MultiplyImageFilterType;
  typename MultiplyImageFilterType::Pointer multiplytFilter = MultiplyImageFilterType::New();
  multiplytFilter->SetInput(subtractConstantFromImageFilter->GetOutput());
  multiplytFilter->SetConstant((double) (this->m_NBin) / (this->m_maxRange - this->m_minRange));
  //Let's round the result - floor
  multiplytFilter->Update();
  typename OutputImageType::Pointer floorImg = multiplytFilter->GetOutput();
  itk::ImageRegionIterator<OutputImageType> itSI(floorImg,floorImg->GetLargestPossibleRegion());
  itSI.GoToBegin();
  while(!itSI.IsAtEnd()) {
    itSI.Set(std::floor(itSI.Get()));
    ++itSI;
  }
  // Save the floorImg if output given
  if(m_ArgsInfo.output_given) {
    this->template SetNextOutput<OutputImageType>(floorImg);
  }
  // Filter
  typedef itk::Statistics::ScalarImageToTextureFeaturesFilter<OutputImageType> ScalarImageToTextureFeaturesFilterType;
  typename ScalarImageToTextureFeaturesFilterType::Pointer textureFeaturesFilter=ScalarImageToTextureFeaturesFilterType::New();
  // Set filter members
  textureFeaturesFilter->SetInput(floorImg);
  if (m_ArgsInfo.mask_given) {
      textureFeaturesFilter->SetMaskImage(maskImage);
  }
  textureFeaturesFilter->SetPixelValueMinMax(0, this->m_NBin-1);
  textureFeaturesFilter->SetNumberOfBinsPerAxis(this->m_NBin);
  typedef typename itk::Statistics::ScalarImageToTextureFeaturesFilter< InputImageType >::OffsetVector OffsetVectorType;
  typename OffsetVectorType::Pointer directions = OffsetVectorType::New();
  if (this->m_diag && m_ArgsInfo.offset_given) {
      std::cerr << "You must provide either -d or -o not both at the same time"
                << std::endl;
      exit(1);
  }
  //No diag
  if (this->m_diag)
  {
      directions->Reserve(InputImageType::ImageDimension);
      typename OffsetVectorType::Iterator dirItr = directions->Begin();
      //Offset
      typedef typename itk::Statistics::ScalarImageToTextureFeaturesFilter< InputImageType >::OffsetType InputOffsetType;
      InputOffsetType offset1;
      offset1.Fill(0);
      offset1[0]=1;
      dirItr->Value() = offset1;
      if(InputImageType::ImageDimension > 1) {
          dirItr++;
          InputOffsetType offset2;
          offset2.Fill(0);
          offset2[1]=1;
          dirItr->Value() = offset2;
      }
      if(InputImageType::ImageDimension > 2) {
          dirItr++;
          InputOffsetType offset3;
          offset3.Fill(0);
          offset3[2]=1;
          dirItr->Value() = offset3;
      }
      textureFeaturesFilter->SetOffsets(directions);
  }
  else if (m_ArgsInfo.offset_given) {
      if(m_ArgsInfo.offset_given != InputImageType::ImageDimension) {
          std::cerr << "You must provide " << InputImageType::ImageDimension
                    << " values for --offset."
                    << std::endl;
          exit(1);
      }
      directions->Reserve(1);
      typename OffsetVectorType::Iterator dirItr = directions->Begin();
      //Offset
      typedef typename itk::Statistics::ScalarImageToTextureFeaturesFilter< InputImageType >::OffsetType InputOffsetType;
      InputOffsetType offset;
      for(unsigned int i=0; i<InputImageType::ImageDimension; i++)
          offset[i] = m_ArgsInfo.offset_arg[i];
      dirItr->Value() = offset;
      textureFeaturesFilter->SetOffsets(directions);
      if(this->m_Verbose) {
          std::cout<<"The offset is set to "<<offset<<std::endl;
      }
  }
  textureFeaturesFilter->Update();
  //if(m_Verbose) {
  //    textureFeaturesFilter->Print(std::cout);
  //}
  //Output
  const typename ScalarImageToTextureFeaturesFilterType::FeatureNameVector* tfN = textureFeaturesFilter->GetRequestedFeatures();
  typename ScalarImageToTextureFeaturesFilterType::FeatureValueVector::Pointer tfMean = textureFeaturesFilter->GetFeatureMeans();
  typename ScalarImageToTextureFeaturesFilterType::FeatureValueVector::Pointer tfStd = textureFeaturesFilter->GetFeatureStandardDeviations();
  // Print the results
  typename ScalarImageToTextureFeaturesFilterType::FeatureNameVector::ConstIterator it = tfN->Begin();
  typename ScalarImageToTextureFeaturesFilterType::FeatureValueVector::Iterator itM = tfMean->Begin();
  typename ScalarImageToTextureFeaturesFilterType::FeatureValueVector::Iterator itStd = tfStd->Begin();
  while(it != tfN->End()) {
      if(it->Value() == ScalarImageToTextureFeaturesFilterType::TextureFeaturesFilterType::TextureFeatureName::Energy) {
          std::cout << "Energy" << ": ";
      } else if (it->Value() == ScalarImageToTextureFeaturesFilterType::TextureFeaturesFilterType::TextureFeatureName::Entropy) {
          std::cout << "Entropy" << ": ";
      } else if (it->Value() == ScalarImageToTextureFeaturesFilterType::TextureFeaturesFilterType::TextureFeatureName::Correlation) {
          std::cout << "Correlation" << ": ";
      } else if (it->Value() == ScalarImageToTextureFeaturesFilterType::TextureFeaturesFilterType::TextureFeatureName::InverseDifferenceMoment) {
          std::cout << "InverseDifferenceMoment" << ": ";
      } else if (it->Value() == ScalarImageToTextureFeaturesFilterType::TextureFeaturesFilterType::TextureFeatureName::Inertia) {
          std::cout << "Inertia" << ": ";
      } else if (it->Value() == ScalarImageToTextureFeaturesFilterType::TextureFeaturesFilterType::TextureFeatureName::ClusterShade) {
          std::cout << "ClusterShade" << ": ";
      } else if (it->Value() == ScalarImageToTextureFeaturesFilterType::TextureFeaturesFilterType::TextureFeatureName::ClusterProminence) {
          std::cout << "ClusterProminence" << ": ";
      } else if (it->Value() == ScalarImageToTextureFeaturesFilterType::TextureFeaturesFilterType::TextureFeatureName::HaralickCorrelation) {
          std::cout << "HaralickCorrelation" << ": ";
      } else if (it->Value() == ScalarImageToTextureFeaturesFilterType::TextureFeaturesFilterType::TextureFeatureName::InvalidFeatureName) {
          std::cout << "InvalidFeatureName" << ": ";
      }
      std::cout << "mean = " << itM->Value() << " - ";
      std::cout << "std = "  << itStd->Value() << std::endl;
      it++;
      itM++;
      itStd++;
  }
}
//--------------------------------------------------------------------


}//end clitk

#endif //#define clitkTextureFeaturesGenericFilter_txx
