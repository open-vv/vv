
#include <itkBinaryDilateImageFilter.h>
#include <itkMirrorPadImageFilter.h>

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3P_SetDefaultValues()
{
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3P_SI_Limits() 
{
  /*
    Apex of the chest & Carina.
   */
  StartNewStep("[Station 3P] Inf/Sup limits with apex of the chest and carina");

  writeImage<MaskImageType>(m_Working_Support, "support.mhd");

  // Get Carina position (has been determined in Station8)
  m_CarinaZ = GetAFDB()->GetDouble("CarinaZ");
  DD(m_CarinaZ);
  
  // Get Apex of the Chest
  MaskImagePointer Lungs = GetAFDB()->template GetImage<MaskImageType>("Lungs");
  DD("lung ok");
  MaskImagePointType p;
  bool found = clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Lungs, 
                                                                       GetBackgroundValue(), 
                                                                       1, true, p);
  DD(found);
  DD(p);
  double m_ApexOfTheChest = p[2];
  DD(m_ApexOfTheChest);

  /* Crop support :
     Superior limit = carina
     Inferior limit = Apex of the chest */
  m_Working_Support = 
    clitk::CropImageAlongOneAxis<MaskImageType>(m_Working_Support, 2, 
                                                m_ApexOfTheChest,
                                                m_CarinaZ, true,
                                                GetBackgroundValue());

  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["3P"] = m_Working_Support;
}
//--------------------------------------------------------------------
