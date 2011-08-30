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

#ifndef CLITKBOOLEANOPERATORLABELIMAGEGENERICFILTER_TXX
#define CLITKBOOLEANOPERATORLABELIMAGEGENERICFILTER_TXX

#include "clitkImageCommon.h"
  
//--------------------------------------------------------------------
template<class ArgsInfoType>
clitk::BooleanOperatorLabelImageGenericFilter<ArgsInfoType>::BooleanOperatorLabelImageGenericFilter():
  ImageToImageGenericFilter<Self>("BooleanOperatorLabelImage") 
{
  // Default values
  cmdline_parser_clitkBool_init(&mArgsInfo);
  InitializeImageType<3>(); // Only for 3D images
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void clitk::BooleanOperatorLabelImageGenericFilter<ArgsInfoType>::InitializeImageType() 
{  
  ADD_IMAGE_TYPE(Dim, uchar); // Can add float later
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void clitk::BooleanOperatorLabelImageGenericFilter<ArgsInfoType>::SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
  if (mArgsInfo.input_given) AddInputFilename(mArgsInfo.input_arg);
  if (mArgsInfo.input2_given) AddInputFilename(mArgsInfo.input2_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class FilterType>
void 
clitk::BooleanOperatorLabelImageGenericFilter<ArgsInfoType>::
SetOptionsFromArgsInfoToFilter(FilterType * f)
{
  // Operation type
  f->SetOperationType(typename FilterType::OperationTypeEnumeration(mArgsInfo.type_arg));
 
  // Output filename
  this->AddOutputFilename(mArgsInfo.output_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class ImageType>
void clitk::BooleanOperatorLabelImageGenericFilter<ArgsInfoType>::UpdateWithInputImageType() 
{ 
  // Reading input
  typename ImageType::Pointer input1 = this->template GetInput<ImageType>(0);
  typename ImageType::Pointer input2 = this->template GetInput<ImageType>(1);

  // Create filter
  typedef clitk::BooleanOperatorLabelImageFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
    
  // Set global Options 
  filter->SetInput1(input1);
  filter->SetInput2(input2);
  SetOptionsFromArgsInfoToFilter<FilterType>(filter);

  // Go !
  filter->Update();

  // Write/Save results
  typename ImageType::Pointer output = filter->GetOutput();
  this->template SetNextOutput<ImageType>(output); 
}
//--------------------------------------------------------------------

#endif //#define CLITKBOOLEANOPERATORLABELIMAGEGENERICFILTER_TXX
