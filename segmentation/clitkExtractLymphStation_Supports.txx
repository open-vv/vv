
#include <itkBinaryDilateImageFilter.h>
#include <itkMirrorPadImageFilter.h>

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStationSupports()
{
  DD("ExtractStationSupports");

  // Get initial Mediastinum
  m_Working_Support = m_Mediastinum = GetAFDB()->template GetImage<MaskImageType>("Mediastinum", true);

  // Superior limits: CricoidCartilag
  // Inferior limits: lung
  //  (the Mediastinum support already stop at this limit)

  // Consider above Carina
  m_CarinaZ = FindCarinaSlicePosition();
  MaskImagePointer m_Support_Superior_to_Carina = 
    clitk::CropImageRemoveLowerThan<MaskImageType>(m_Working_Support, 2, 
                                                   m_CarinaZ, true, GetBackgroundValue());
  MaskImagePointer m_Support_Inferior_to_Carina = 
    clitk::CropImageRemoveGreaterThan<MaskImageType>(m_Working_Support, 2, 
                                                     m_CarinaZ, true, GetBackgroundValue());

  // Consider only Superior to Carina
  m_Working_Support = m_Support_Superior_to_Carina;

  // Step : S1RL
  StartNewStep("[Support] sup-inf S1RL");
  /*
    Lower border: clavicles bilaterally and, in the midline, the upper
    border of the manubrium, 1R designates right-sided nodes, 1L,
    left-sided nodes in this region
    
    2R: Upper border: apex of the right lung and pleural space, and in
    the midline, the upper border of the manubrium
    
    2L: Upper border: apex of the left lung and pleural space, and in the
    midline, the upper border of the manubrium
  */









  // //  LeftRight cut along trachea
  // MaskImagePointer Trachea = GetAFDB()->GetImage("Trachea");
  // // build a ant-post line for each slice

  // MaskImagePointer m_Support_SupRight = 
  //   clitk::CropImageRemoveLowerThan<MaskImageType>(m_Working_Support, 2, 
  //                                                  m_CarinaZ, true, GetBackgroundValue());





  // Store image filenames into AFDB 
  m_ListOfSupports["S1R"] = m_Working_Support;
  writeImage<MaskImageType>(m_ListOfSupports["S1R"], "seg/Support_S1R.mhd");
  GetAFDB()->SetImageFilename("Support_S1R", "seg/Support_S1R.mhd");
  WriteAFDB();
}
//--------------------------------------------------------------------


