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

#ifndef CLITKEXTRACTMEDIASTINUMSGENERICFILTER_TXX
#define CLITKEXTRACTMEDIASTINUMSGENERICFILTER_TXX

#include "clitkImageCommon.h"
  
//--------------------------------------------------------------------
template<class ArgsInfoType>
clitk::ExtractMediastinumGenericFilter<ArgsInfoType>::ExtractMediastinumGenericFilter():
  ImageToImageGenericFilter<Self>("ExtractMediastinum") 
{
  // Default values
  cmdline_parser_clitkExtractMediastinum_init(&mArgsInfo);
  InitializeImageType<3>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void clitk::ExtractMediastinumGenericFilter<ArgsInfoType>::InitializeImageType() 
{  
  ADD_IMAGE_TYPE(Dim, uchar);
  // ADD_IMAGE_TYPE(Dim, short);
  // ADD_IMAGE_TYPE(Dim, int);
  // ADD_IMAGE_TYPE(Dim, float);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void clitk::ExtractMediastinumGenericFilter<ArgsInfoType>::SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
  if (mArgsInfo.patient_given) AddInputFilename(mArgsInfo.patient_arg);
  if (mArgsInfo.lung_given) AddInputFilename(mArgsInfo.lung_arg);
  if (mArgsInfo.bones_given) AddInputFilename(mArgsInfo.bones_arg);
  if (mArgsInfo.output_given)  AddOutputFilename(mArgsInfo.output_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class ImageType>
void clitk::ExtractMediastinumGenericFilter<ArgsInfoType>::UpdateWithInputImageType() 
{ 
  // Reading input
  typename ImageType::Pointer patient = this->template GetInput<ImageType>(0);
  typename ImageType::Pointer lung = this->template GetInput<ImageType>(1);
  typename ImageType::Pointer bones = this->template GetInput<ImageType>(2);

  // Create filter
  typedef clitk::ExtractMediastinumFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
    
  // Set global Options 
  filter->SetInputPatientLabelImage(patient, mArgsInfo.patientBG_arg);
  filter->SetInputLungLabelImage(lung, mArgsInfo.lungBG_arg, mArgsInfo.lungRight_arg, mArgsInfo.lungLeft_arg);
  filter->SetInputBonesLabelImage(bones, mArgsInfo.bonesBG_arg);
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

#endif //#define CLITKEXTRACTMEDIASTINUMSGENERICFILTER_TXX
