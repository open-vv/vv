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

#ifndef CLITKEXTRACTLYMPHSTATIONSSGENERICFILTER_TXX
#define CLITKEXTRACTLYMPHSTATIONSSGENERICFILTER_TXX

#include "clitkImageCommon.h"
  
//--------------------------------------------------------------------
template<class ArgsInfoType>
clitk::ExtractLymphStationsGenericFilter<ArgsInfoType>::ExtractLymphStationsGenericFilter():
  ImageToImageGenericFilter<Self>("ExtractLymphStations") 
{
  // Default values
  cmdline_parser_clitkExtractLymphStations_init(&mArgsInfo);
  InitializeImageType<3>(); // Only for 3D images
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void clitk::ExtractLymphStationsGenericFilter<ArgsInfoType>::InitializeImageType() 
{  
  ADD_IMAGE_TYPE(Dim, short); // Can add float later
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void clitk::ExtractLymphStationsGenericFilter<ArgsInfoType>::SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
  if (mArgsInfo.input_given) AddInputFilename(mArgsInfo.input_arg);
  if (mArgsInfo.output_given) AddOutputFilename(mArgsInfo.output_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class FilterType>
void 
clitk::ExtractLymphStationsGenericFilter<ArgsInfoType>::
SetOptionsFromArgsInfoToFilter(FilterType * f)
{
  f->SetVerboseOption(mArgsInfo.verbose_flag);
  f->SetVerboseStep(mArgsInfo.verboseStep_flag);
  f->SetWriteStep(mArgsInfo.writeStep_flag);
  f->SetAFDBFilename(mArgsInfo.afdb_arg);  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class ImageType>
void clitk::ExtractLymphStationsGenericFilter<ArgsInfoType>::UpdateWithInputImageType() 
{ 
  // Reading input
  typename ImageType::Pointer input = this->template GetInput<ImageType>(0);

  // Create filter
  typedef clitk::ExtractLymphStationsFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
    
  // Set global Options 
  filter->SetInput(input);
  SetOptionsFromArgsInfoToFilter<FilterType>(filter);

  // Go !
  filter->Update();

  // Write/Save results
  typedef uchar MaskImagePixelType;
  typedef itk::Image<MaskImagePixelType, 3> OutputImageType;
  typename OutputImageType::Pointer output = filter->GetOutput();
  this->template SetNextOutput<OutputImageType>(output); 
}
//--------------------------------------------------------------------

#endif //#define CLITKEXTRACTLYMPHSTATIONSSGENERICFILTER_TXX
