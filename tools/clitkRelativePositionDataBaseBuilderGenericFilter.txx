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
clitk::RelativePositionDataBaseBuilderGenericFilter<ArgsInfoType>::
RelativePositionDataBaseBuilderGenericFilter():
  ImageToImageGenericFilter<Self>("RelativePositionDataBaseBuilder")
{
  // Default values
  cmdline_parser_clitkRelativePositionDataBaseBuilder_init(&mArgsInfo);
  InitializeImageType<3>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void clitk::RelativePositionDataBaseBuilderGenericFilter<ArgsInfoType>::
InitializeImageType() 
{  
  ADD_IMAGE_TYPE(Dim, uchar);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void clitk::RelativePositionDataBaseBuilderGenericFilter<ArgsInfoType>::
SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class FilterType>
void clitk::RelativePositionDataBaseBuilderGenericFilter<ArgsInfoType>::
SetOptionsFromArgsInfoToFilter(FilterType * f) 
{
  f->SetAFDBFilename(mArgsInfo.afdb_arg);
  f->SetAFDBPath(mArgsInfo.afdb_path_arg);
  f->SetNumberOfBins(mArgsInfo.bins_arg);
  f->SetNumberOfAngles(mArgsInfo.nb_arg);
  f->SetAreaLossTolerance(mArgsInfo.tol_arg);
  f->SetSupportName(mArgsInfo.supportName_arg);
  f->SetTargetName(mArgsInfo.targetName_arg);
  for(int i=0; i<mArgsInfo.objectName_given; i++)
    f->AddObjectName(mArgsInfo.objectName_arg[i]);  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class ImageType>
void clitk::RelativePositionDataBaseBuilderGenericFilter<ArgsInfoType>::
UpdateWithInputImageType() 
{ 
  // Create filter
  typedef clitk::RelativePositionDataBaseBuilderFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  
  // Set global Options 
  SetOptionsFromArgsInfoToFilter<FilterType>(filter);

  // Go !
  filter->Update();
  
  // Write/Save results
  //  typename ImageType::Pointer output = filter->GetOutput();
  //this->template SetNextOutput<ImageType>(output); 

}
//--------------------------------------------------------------------


