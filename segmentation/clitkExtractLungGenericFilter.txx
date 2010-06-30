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

#ifndef CLITKEXTRACTLUNGSGENERICFILTER_TXX
#define CLITKEXTRACTLUNGSGENERICFILTER_TXX

#include "clitkImageCommon.h"
  
//--------------------------------------------------------------------
template<class ArgsInfoType>
clitk::ExtractLungGenericFilter<ArgsInfoType>::ExtractLungGenericFilter():
  ImageToImageGenericFilter<Self>("ExtractLung") 
{
  // Default values
  cmdline_parser_clitkExtractLung_init(&mArgsInfo);
  InitializeImageType<3>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void clitk::ExtractLungGenericFilter<ArgsInfoType>::InitializeImageType() 
{  
  ADD_IMAGE_TYPE(Dim, short);
  // ADD_IMAGE_TYPE(Dim, int);
  // ADD_IMAGE_TYPE(Dim, float);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void clitk::ExtractLungGenericFilter<ArgsInfoType>::SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
  if (mArgsInfo.input_given)   AddInputFilename(mArgsInfo.input_arg);
  if (mArgsInfo.patient_given) AddInputFilename(mArgsInfo.patient_arg);
  if (mArgsInfo.output_given)  AddOutputFilename(mArgsInfo.output_arg);
  if (mArgsInfo.outputTrachea_given)  AddOutputFilename(mArgsInfo.outputTrachea_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class ImageType>
void clitk::ExtractLungGenericFilter<ArgsInfoType>::UpdateWithInputImageType() 
{ 
  // Mask & output image type
  typedef itk::Image<uchar, ImageType::ImageDimension> OutputImageType;
  typedef itk::Image<uchar, ImageType::ImageDimension> MaskImageType;

  // Reading input
  typename ImageType::Pointer input = this->template GetInput<ImageType>(0);
  typename MaskImageType::Pointer patient = this->template GetInput<MaskImageType>(1);

  // Create filter
  typedef clitk::ExtractLungFilter<ImageType, MaskImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
    
  // Set global Options 
  filter->SetArgsInfo(mArgsInfo);
  filter->SetInput(input);
  filter->SetInputPatientMask(patient, mArgsInfo.patientBG_arg);

  // Go !
  filter->Update();
  
  // Check if error
  if (filter->HasError()) {
    SetLastError(filter->GetLastError());
    // No output
    return;
  }

  // Write/Save results
  typename OutputImageType::Pointer output = filter->GetOutput();
  this->template SetNextOutput<OutputImageType>(output); 
  if (mArgsInfo.outputTrachea_given) 
    this->template SetNextOutput<typename FilterType::MaskImageType>(filter->GetTracheaImage()); 
}
//--------------------------------------------------------------------

#endif //#define CLITKEXTRACTLUNGSGENERICFILTER_TXX
