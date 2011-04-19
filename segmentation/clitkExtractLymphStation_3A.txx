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
===========================================================================*/

#include <itkBinaryDilateImageFilter.h>
#include <itkMirrorPadImageFilter.h>

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3A_SetDefaultValues()
{
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3A_SI_Limits() 
{
  // Apex of the chest or Sternum & Carina.
  StartNewStep("[Station 3A] Inf/Sup limits with Sternum and Carina");

  // Get Carina position (has been determined in Station8)
  m_CarinaZ = GetAFDB()->GetDouble("CarinaZ");
  
  // Get Sternum and search for the upper position
  MaskImagePointer Sternum = GetAFDB()->template GetImage<MaskImageType>("Sternum");

  // Search most sup point
  MaskImagePointType ps = Sternum->GetOrigin(); // initialise to avoid warning 
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Sternum, GetBackgroundValue(), 2, false, ps);
  double m_SternumZ = ps[2]+Sternum->GetSpacing()[2]; // One more slice, because it is below this point

  //* Crop support :
  m_Working_Support = 
    clitk::CropImageAlongOneAxis<MaskImageType>(m_Working_Support, 2, 
                                                m_CarinaZ, m_SternumZ, true,
                                                GetBackgroundValue());

  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["3A"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3A_Ant_Limits() 
{
  StartNewStep("[Station 3A] Ant limits with Sternum");

  // Get Sternum, keep posterior part.
  MaskImagePointer Sternum = GetAFDB()->template GetImage<MaskImageType>("Sternum");
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, Sternum, 2, 
                                                       0.5, "PostTo", 
                                                       false, 3, true, false);
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["3A"] = m_Working_Support;
}
//--------------------------------------------------------------------


