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

#include "clitkImageCommon.h"

//--------------------------------------------------------------------
template<class ArgsInfoType>
clitk::ExtractPatientGenericFilter<ArgsInfoType>::ExtractPatientGenericFilter():
  ImageToImageGenericFilter<Self>("ExtractPatient") 
{
  // Default values
  cmdline_parser_clitkExtractPatient_init(&mArgsInfo);
  InitializeImageType<3>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void clitk::ExtractPatientGenericFilter<ArgsInfoType>::InitializeImageType() 
{  
  ADD_IMAGE_TYPE(Dim, short);
  // ADD_IMAGE_TYPE(Dim, int);
  // ADD_IMAGE_TYPE(Dim, float);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void clitk::ExtractPatientGenericFilter<ArgsInfoType>::SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
  if (mArgsInfo.input_given)  SetInputFilename(mArgsInfo.input_arg);
  if (mArgsInfo.output_given)  AddOutputFilename(mArgsInfo.output_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class FilterType>
void 
clitk::ExtractPatientGenericFilter<ArgsInfoType>::
SetOptionsFromArgsInfoToFilter(FilterType * f)
{
  f->SetVerboseOptionFlag(mArgsInfo.verbose_flag);
  f->SetVerboseStepFlag(mArgsInfo.verboseStep_flag);
  f->SetWriteStepFlag(mArgsInfo.writeStep_flag);
  f->SetVerboseWarningFlag(!mArgsInfo.verboseWarningOff_flag);
  f->SetVerboseMemoryFlag(mArgsInfo.verboseMemory_flag);

  if (mArgsInfo.afdb_given)
    f->SetAFDBFilename(mArgsInfo.afdb_arg);  
  
  f->SetOutputPatientFilename(mArgsInfo.output_arg);

  f->SetUpperThreshold(mArgsInfo.upper_arg);
  f->SetLowerThreshold(mArgsInfo.lower_arg);

  f->SetDecomposeAndReconstructDuringFirstStep(mArgsInfo.erode1_flag);

  // Convert to SizeType
  typename FilterType::InputImageSizeType s;
  if (mArgsInfo.radius1_given) {
    ConvertOptionMacro(mArgsInfo.radius1, s, 3, false);
    f->SetRadius1(s);
  }

  f->SetMaximumNumberOfLabels1(mArgsInfo.max1_arg);
  f->SetNumberOfNewLabels1(mArgsInfo.new1_arg);

  f->SetDecomposeAndReconstructDuringSecondStep(mArgsInfo.erode2_flag);

  // Convert to SizeType
  if (mArgsInfo.radius2_given) {
    ConvertOptionMacro(mArgsInfo.radius2, s, 3, false);
    f->SetRadius2(s);
  }

  f->SetMaximumNumberOfLabels2(mArgsInfo.max2_arg);
  f->SetNumberOfNewLabels2(mArgsInfo.new2_arg);
  
  f->SetFirstKeep(mArgsInfo.firstKeep_arg);
  f->SetLastKeep(mArgsInfo.lastKeep_arg);

  f->SetFinalOpenClose(mArgsInfo.openClose_flag);
  f->SetAutoCrop(!mArgsInfo.noAutoCrop_flag);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class ImageType>
void clitk::ExtractPatientGenericFilter<ArgsInfoType>::UpdateWithInputImageType() 
{ 
  // Output image type
  typedef itk::Image<uchar, ImageType::ImageDimension> OutputImageType;

  // Reading input
  typename ImageType::Pointer input = this->template GetInput<ImageType>(0);

  // Create filter
  typedef clitk::ExtractPatientFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
    
  // Set the filter (needed for example for threaded monitoring)
  this->SetFilterBase(filter);
    
  // Set global Options 
  filter->SetInput(input);
  SetOptionsFromArgsInfoToFilter<FilterType>(filter);

  // Go !
  filter->Update();

  // Write/Save results
  typename OutputImageType::Pointer output = filter->GetOutput();
  this->template SetNextOutput<OutputImageType>(output); 
}
//--------------------------------------------------------------------


