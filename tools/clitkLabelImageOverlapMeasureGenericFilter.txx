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

//--------------------------------------------------------------------
template<class ArgsInfoType>
clitk::LabelImageOverlapMeasureGenericFilter<ArgsInfoType>::
LabelImageOverlapMeasureGenericFilter():
  ImageToImageGenericFilter<Self>("LabelImageOverlapMeasure")
{
  // Default values
  cmdline_parser_clitkLabelImageOverlapMeasure_init(&mArgsInfo);
  //InitializeImageType<2>();
  InitializeImageType<3>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void clitk::LabelImageOverlapMeasureGenericFilter<ArgsInfoType>::
InitializeImageType() 
{  
  ADD_IMAGE_TYPE(Dim, uchar);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void clitk::LabelImageOverlapMeasureGenericFilter<ArgsInfoType>::
SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
  if (mArgsInfo.input1_given) AddInputFilename(mArgsInfo.input1_arg);
  if (mArgsInfo.input2_given) AddInputFilename(mArgsInfo.input2_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class FilterType>
void clitk::LabelImageOverlapMeasureGenericFilter<ArgsInfoType>::
SetOptionsFromArgsInfoToFilter(FilterType * f) 
{
  f->SetLabel1(mArgsInfo.label1_arg);  
  f->SetLabel2(mArgsInfo.label2_arg);  
}

//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class ImageType>
void clitk::LabelImageOverlapMeasureGenericFilter<ArgsInfoType>::
UpdateWithInputImageType() 
{ 
  // Reading input
  typename ImageType::Pointer input1 = this->template GetInput<ImageType>(0);
  typename ImageType::Pointer input2 = this->template GetInput<ImageType>(1);

  // Create filter
  typedef clitk::LabelImageOverlapMeasureFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  
  // Set global Options 
  filter->SetInput(0, input1);
  filter->SetInput(1, input2);
  SetOptionsFromArgsInfoToFilter<FilterType>(filter);

  // Go !
  filter->Update();
  
  // Write/Save results
  // typename ImageType::Pointer output = filter->GetOutput();
  // this->template SetNextOutput<ImageType>(output); 
}
//--------------------------------------------------------------------


