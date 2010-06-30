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
  InitializeImageType<3>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void clitk::ExtractLymphStationsGenericFilter<ArgsInfoType>::InitializeImageType() 
{  
  ADD_IMAGE_TYPE(Dim, uchar);
  // ADD_IMAGE_TYPE(Dim, short);
  // ADD_IMAGE_TYPE(Dim, int);
  // ADD_IMAGE_TYPE(Dim, float);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void clitk::ExtractLymphStationsGenericFilter<ArgsInfoType>::SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
  if (mArgsInfo.mediastinum_given) AddInputFilename(mArgsInfo.mediastinum_arg);
  if (mArgsInfo.trachea_given) AddInputFilename(mArgsInfo.trachea_arg);
  if (mArgsInfo.output_given)  AddOutputFilename(mArgsInfo.output_arg);
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
  typename ImageType::Pointer mediastinum = this->template GetInput<ImageType>(0);
  typename ImageType::Pointer trachea = this->template GetInput<ImageType>(1);

  // Create filter
  typedef clitk::ExtractLymphStationsFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
    
  // Set global Options 
  filter->SetInputMediastinumLabelImage(mediastinum, 0); // change 0 with BG 
  filter->SetInputTracheaLabelImage(trachea, 0); // change 0 with BG 
  filter->SetArgsInfo(mArgsInfo);

  // Go !
  filter->Update();
  
  // Check if error
  if (filter->HasError()) {
    SetLastError(filter->GetLastError());
    // No output
    return;
  }

  // Write/Save results
  typename ImageType::Pointer output = filter->GetOutput();
  this->template SetNextOutput<ImageType>(output); 
}
//--------------------------------------------------------------------

#endif //#define CLITKEXTRACTLYMPHSTATIONSSGENERICFILTER_TXX
