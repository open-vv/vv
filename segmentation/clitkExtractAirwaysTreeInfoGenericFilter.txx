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

#ifndef CLITKEXTRACTAIRWAYTREEINFOSGENERICFILTER_TXX
#define CLITKEXTRACTAIRWAYTREEINFOSGENERICFILTER_TXX

#include "clitkImageCommon.h"
  
//--------------------------------------------------------------------
template<class ArgsInfoType>
clitk::ExtractAirwaysTreeInfoGenericFilter<ArgsInfoType>::ExtractAirwaysTreeInfoGenericFilter():
  ImageToImageGenericFilter<Self>("ExtractAirwaysTreeInfo") 
{
  this->SetFilterBase(NULL);
  // Default values
  cmdline_parser_clitkExtractAirwaysTreeInfo_init(&mArgsInfo);
  InitializeImageType<3>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void clitk::ExtractAirwaysTreeInfoGenericFilter<ArgsInfoType>::InitializeImageType() 
{  
  ADD_IMAGE_TYPE(Dim, uchar);
  // ADD_IMAGE_TYPE(Dim, int);
  // ADD_IMAGE_TYPE(Dim, float);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void clitk::ExtractAirwaysTreeInfoGenericFilter<ArgsInfoType>::SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
  if (mArgsInfo.input_given)  AddInputFilename(mArgsInfo.input_arg);
  if (mArgsInfo.output_given) AddOutputFilename(mArgsInfo.output_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class ImageType>
void clitk::ExtractAirwaysTreeInfoGenericFilter<ArgsInfoType>::UpdateWithInputImageType() 
{ 
  // Reading input
  typename ImageType::Pointer input = this->template GetInput<ImageType>(0);

  // Create filter
  typedef clitk::ExtractAirwaysTreeInfoFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();

  // Set the filter (needed for example for threaded monitoring)
  this->SetFilterBase(filter);
    
  // Set global Options 
  filter->SetArgsInfo(mArgsInfo);
  filter->SetInput(input);

  // Go !
  filter->Update();

  // Write/Save results
  typename ImageType::Pointer output = filter->GetOutput();
  this->template SetNextOutput<ImageType>(output);
  filter->WriteAFDB();
}
//--------------------------------------------------------------------

#endif //#define CLITKEXTRACTAIRWAYTREEINFOSGENERICFILTER_TXX
