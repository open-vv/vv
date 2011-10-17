
#include <itkBinaryDilateImageFilter.h>
#include <itkMirrorPadImageFilter.h>

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStationSupports()
{
  // Get initial Mediastinum
  m_Working_Support = m_Mediastinum = this->GetAFDB()->template GetImage<MaskImageType>("Mediastinum", true);

  // Consider sup/inf to Carina
  double m_CarinaZ = FindCarina();
  MaskImagePointer m_Support_Superior_to_Carina = 
    clitk::CropImageRemoveLowerThan<MaskImageType>(m_Working_Support, 2, 
                                                   m_CarinaZ, true, GetBackgroundValue());
  MaskImagePointer m_Support_Inferior_to_Carina = 
    clitk::CropImageRemoveGreaterThan<MaskImageType>(m_Working_Support, 2, 
                                                     m_CarinaZ, true, GetBackgroundValue());
  m_ListOfSupports["Support_Superior_to_Carina"] = m_Support_Superior_to_Carina;
  m_ListOfSupports["Support_Inferior_to_Carina"] = m_Support_Inferior_to_Carina;
  writeImage<MaskImageType>(m_Support_Inferior_to_Carina, "seg/Support_Inf_Carina.mha");
  this->GetAFDB()->SetImageFilename("Support_Inf_Carina", "seg/Support_Inf_Carina.mha");
  writeImage<MaskImageType>(m_Support_Superior_to_Carina, "seg/Support_Sup_Carina.mha");
  this->GetAFDB()->SetImageFilename("Support_Sup_Carina", "seg/Support_Sup_Carina.mha");

  // S1RL
  Support_SupInf_S1RL();
  Support_LeftRight_S1R_S1L();

  // S2RL
  Support_SupInf_S2R_S2L();
  Support_LeftRight_S2R_S2L();

  // S4RL
  Support_SupInf_S4R_S4L();
  Support_LeftRight_S4R_S4L();
  
  // Post limits of S1,S2,S4
  Support_Post_S1S2S4();

  // S3AP
  Support_S3P();
  Support_S3A();
  
  // S5, S6
  Support_S5();
  Support_S6();
  
  // Below Carina S7,8,9,10
  m_ListOfSupports["S7"] = clitk::Clone<MaskImageType>(m_Support_Inferior_to_Carina);
  m_ListOfSupports["S8"] = clitk::Clone<MaskImageType>(m_Support_Inferior_to_Carina);
  m_ListOfSupports["S9"] = clitk::Clone<MaskImageType>(m_Support_Inferior_to_Carina);
  m_ListOfSupports["S10"] = clitk::Clone<MaskImageType>(m_Support_Inferior_to_Carina);
  m_ListOfSupports["S11"] = clitk::Clone<MaskImageType>(m_Support_Inferior_to_Carina);

  // Store image filenames into AFDB 
  writeImage<MaskImageType>(m_ListOfSupports["S1R"], "seg/Support_S1R.mha");
  this->GetAFDB()->SetImageFilename("Support_S1R", "seg/Support_S1R.mha");
  writeImage<MaskImageType>(m_ListOfSupports["S1L"], "seg/Support_S1L.mha");
  this->GetAFDB()->SetImageFilename("Support_S1L", "seg/Support_S1L.mha");

  writeImage<MaskImageType>(m_ListOfSupports["S2L"], "seg/Support_S2L.mha");
  this->GetAFDB()->SetImageFilename("Support_S2L", "seg/Support_S2L.mha");
  writeImage<MaskImageType>(m_ListOfSupports["S2R"], "seg/Support_S2R.mha");
  this->GetAFDB()->SetImageFilename("Support_S2R", "seg/Support_S2R.mha");

  writeImage<MaskImageType>(m_ListOfSupports["S3P"], "seg/Support_S3P.mha");
  this->GetAFDB()->SetImageFilename("Support_S3P", "seg/Support_S3P.mha");
  writeImage<MaskImageType>(m_ListOfSupports["S3A"], "seg/Support_S3A.mha");
  this->GetAFDB()->SetImageFilename("Support_S3A", "seg/Support_S3A.mha");

  writeImage<MaskImageType>(m_ListOfSupports["S4L"], "seg/Support_S4L.mha");
  this->GetAFDB()->SetImageFilename("Support_S4L", "seg/Support_S4L.mha");
  writeImage<MaskImageType>(m_ListOfSupports["S4R"], "seg/Support_S4R.mha");
  this->GetAFDB()->SetImageFilename("Support_S4R", "seg/Support_S4R.mha");

  writeImage<MaskImageType>(m_ListOfSupports["S5"], "seg/Support_S5.mha");
  this->GetAFDB()->SetImageFilename("Support_S5", "seg/Support_S5.mha");
  writeImage<MaskImageType>(m_ListOfSupports["S6"], "seg/Support_S6.mha");
  this->GetAFDB()->SetImageFilename("Support_S6", "seg/Support_S6.mha");

  writeImage<MaskImageType>(m_ListOfSupports["S7"], "seg/Support_S7.mha");
  this->GetAFDB()->SetImageFilename("Support_S7", "seg/Support_S7.mha");

  writeImage<MaskImageType>(m_ListOfSupports["S8"], "seg/Support_S8.mha");
  this->GetAFDB()->SetImageFilename("Support_S8", "seg/Support_S8.mha");

  writeImage<MaskImageType>(m_ListOfSupports["S9"], "seg/Support_S9.mha");
  this->GetAFDB()->SetImageFilename("Support_S9", "seg/Support_S9.mha");

  writeImage<MaskImageType>(m_ListOfSupports["S10"], "seg/Support_S10.mha");
  this->GetAFDB()->SetImageFilename("Support_S10", "seg/Support_S10.mha");  

  writeImage<MaskImageType>(m_ListOfSupports["S11"], "seg/Support_S11.mha");
  this->GetAFDB()->SetImageFilename("Support_S11", "seg/Support_S11.mha");  
  WriteAFDB();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
Support_SupInf_S1RL()
{
  // Step : S1RL
  StartNewStep("[Support] Sup-Inf S1RL");
  /*
    2R: Upper border: apex of the right lung and pleural space, and in
    the midline, the upper border of the manubrium
    
    2L: Upper border: apex of the left lung and pleural space, and in the
    midline, the upper border of the manubrium

    => apex / manubrium = up Sternum
  */
  m_Working_Support = m_ListOfSupports["Support_Superior_to_Carina"];
  MaskImagePointer Sternum = this->GetAFDB()->template GetImage <MaskImageType>("Sternum");
  MaskImagePointType p;
  p[0] = p[1] = p[2] =  0.0; // to avoid warning
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Sternum, GetBackgroundValue(), 2, false, p);
  MaskImagePointer S1RL = 
    clitk::CropImageRemoveLowerThan<MaskImageType>(m_Working_Support, 2, 
                                                   p[2], true, GetBackgroundValue());
  m_Working_Support = 
    clitk::CropImageRemoveGreaterThan<MaskImageType>(m_Working_Support, 2, 
                                                     p[2], true, GetBackgroundValue());
  m_ListOfSupports["S1RL"] = S1RL;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
Support_LeftRight_S1R_S1L()
{
  // Step S1RL : Left-Right
  StartNewStep("[Support] Left-Right S1R S1L");
  std::vector<ImagePointType> A;
  std::vector<ImagePointType> B;
  // Search for centroid positions of trachea
  MaskImagePointer Trachea = this->GetAFDB()->template GetImage <MaskImageType>("Trachea");
  MaskImagePointer S1RL = m_ListOfSupports["S1RL"];
  Trachea = clitk::ResizeImageLike<MaskImageType>(Trachea, S1RL, GetBackgroundValue());
  std::vector<MaskSlicePointer> slices;
  clitk::ExtractSlices<MaskImageType>(Trachea, 2, slices);
  for(uint i=0; i<slices.size(); i++) {
    slices[i] = Labelize<MaskSliceType>(slices[i], 0, false, 10);
    std::vector<typename MaskSliceType::PointType> c;
    clitk::ComputeCentroids<MaskSliceType>(slices[i], GetBackgroundValue(), c);
    ImagePointType a,b;
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(c[1], Trachea, i, a);
    A.push_back(a);
    b = a; 
    b[1] += 50;
    B.push_back(b);
  }
  clitk::WriteListOfLandmarks<MaskImageType>(A, "S1LR_A.txt");
  clitk::WriteListOfLandmarks<MaskImageType>(B, "S1LR_B.txt");

  // Clone support
  MaskImagePointer S1R = clitk::Clone<MaskImageType>(S1RL);
  MaskImagePointer S1L = clitk::Clone<MaskImageType>(S1RL);

  // Right part
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(S1R, A, B, 
                                                                    GetBackgroundValue(), 0, -10);
  S1R = clitk::AutoCrop<MaskImageType>(S1R, GetBackgroundValue());
  m_ListOfSupports["S1R"] = S1R;

  // Left part
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(S1L, A, B, 
                                                                    GetBackgroundValue(), 0, 10);
  S1L = clitk::AutoCrop<MaskImageType>(S1L, GetBackgroundValue());
  m_ListOfSupports["S1L"] = S1L;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
Support_SupInf_S2R_S2L()
{
  // Step : S2RL Sup-Inf limits
  /*
    2R Lower border: intersection of caudal margin of innominate vein with
    the trachea
    2L Lower border: superior border of the aortic arch
  */
  StartNewStep("[Support] Sup-Inf S2RL");
  m_Working_Support = m_ListOfSupports["Support_Superior_to_Carina"];
  
  // S2R Caudal Margin Of Left BrachiocephalicVein
  MaskImagePointer BrachioCephalicVein = this->GetAFDB()->template GetImage<MaskImageType>("BrachioCephalicVein");
  MaskImagePointType p;
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(BrachioCephalicVein, GetBackgroundValue(), 2, true, p);

  // I add slightly more than a slice --> NO !!
  double CaudalMarginOfLeftBrachiocephalicVeinZ=p[2];//+ 1.1*m_Working_Support->GetSpacing()[2];

  this->GetAFDB()->SetDouble("CaudalMarginOfLeftBrachiocephalicVeinZ", CaudalMarginOfLeftBrachiocephalicVeinZ);
  MaskImagePointer S2R = 
    clitk::CropImageRemoveLowerThan<MaskImageType>(m_Working_Support, 2, 
                                                   CaudalMarginOfLeftBrachiocephalicVeinZ, true,
                                                   GetBackgroundValue());
  // S2L : Top Of Aortic Arch
  MaskImagePointer Aorta = this->GetAFDB()->template GetImage<MaskImageType>("Aorta");
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Aorta, GetBackgroundValue(), 2, false, p);

  // Save the TopOfAorticArchZ
  this->GetAFDB()->SetDouble("TopOfAorticArchZ", p[2]);

  // I substract slightly more than a slice to respect delineation
  double TopOfAorticArchZ=p[2]- 1.1*m_Working_Support->GetSpacing()[2];
  //  this->GetAFDB()->SetDouble("TopOfAorticArchZ", TopOfAorticArchZ);

  MaskImagePointer S2L = 
    clitk::CropImageRemoveLowerThan<MaskImageType>(m_Working_Support, 2, 
                                                   TopOfAorticArchZ, true,
                                                   GetBackgroundValue());

  /*
  // S2RL: Superior support, I use inferior part of S1RL
  MaskImagePointer S1L = m_ListOfSupports["S1L"];
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(S1L, GetBackgroundValue(), 2, true, p);
  DD(p);
  S2L = 
    clitk::CropImageRemoveGreaterThan<MaskImageType>(S2L, 2, 
                                                     p[2], true,
                                                     GetBackgroundValue());

  MaskImagePointer S1R = m_ListOfSupports["S1R"];
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(S1R, GetBackgroundValue(), 2, true, p);
  DD(p);
  S2R = 
    clitk::CropImageRemoveGreaterThan<MaskImageType>(S2R, 2, 
                                                     p[2], true,
                                                     GetBackgroundValue());          
  */

  // Superior limits, use Sternum (but not strictly inf to S1RL
  MaskImagePointer Sternum = this->GetAFDB()->template GetImage <MaskImageType>("Sternum");
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Sternum, GetBackgroundValue(), 2, false, p);
  // Add one slice
  p[2] = p[2] +  m_Working_Support->GetSpacing()[2];
  S2L = 
    clitk::CropImageRemoveGreaterThan<MaskImageType>(S2L, 2, 
                                                     p[2], true, GetBackgroundValue());
  S2R = 
    clitk::CropImageRemoveGreaterThan<MaskImageType>(S2R, 2, 
                                                     p[2], true, GetBackgroundValue());

  // The is the end
  m_ListOfSupports["S2L"] = S2L;
  m_ListOfSupports["S2R"] = S2R;
}
//--------------------------------------------------------------------



//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
Support_LeftRight_S2R_S2L()
{
  // ---------------------------------------------------------------------------
  /* Step : S2RL LeftRight
     As for lymph node station 4R, 2R includes nodes extending to the
     left lateral border of the trachea
     Rod says:  "For station 2 there is a shift, dividing 2R from 2L, from midline
     to the left paratracheal border."
  */
  StartNewStep("[Support] Separate 2R/2L according to Trachea");
  MaskImagePointer S2R = m_ListOfSupports["S2R"];
  MaskImagePointer S2L = m_ListOfSupports["S2L"];
  S2R = LimitsWithTrachea(S2R, 0, 1, -10);
  S2L = LimitsWithTrachea(S2L, 0, 1, 10);
  m_ListOfSupports["S2R"] = S2R;
  m_ListOfSupports["S2L"] = S2L;  
  this->GetAFDB()->template ReleaseImage<MaskImageType>("Trachea");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
Support_SupInf_S4R_S4L()
{
  // ---------------------------------------------------------------------------
  /* Step : S4RL Sup-Inf
     - start at the end of 2R and 2L
     - stop ?
     - 4R
     Rod says : "The inferior border is at the lower border of the azygous vein."
     Rod says : difficulties
     (was : "ends at the upper lobe bronchus or where the right pulmonary artery
     crosses the midline of the mediastinum ")
     - 4L
     Rod says : "The lower border is to upper margin of the left main pulmonary artery."
     (was LLL bronchus)
  */
  StartNewStep("[Support] Sup-Inf limits of 4R/4L");

  // Start from the support
  MaskImagePointer S4RL = clitk::Clone<MaskImageType>(m_Working_Support);
  MaskImagePointer S4R = clitk::Clone<MaskImageType>(S4RL);
  MaskImagePointer S4L = clitk::Clone<MaskImageType>(S4RL);

  // Keep only what is lower than S2
  MaskImagePointer S2R = m_ListOfSupports["S2R"];
  MaskImagePointer S2L = m_ListOfSupports["S2L"];
  MaskImagePointType p;
  // Right part
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(S2R, GetBackgroundValue(), 
                                                          2, true, p);
  S4R = clitk::CropImageRemoveGreaterThan<MaskImageType>(S4R, 2, 
                                                       p[2], true, GetBackgroundValue());
  // Left part
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(S2L, GetBackgroundValue(), 
                                                          2, true, p);
  S4L = clitk::CropImageRemoveGreaterThan<MaskImageType>(S4L, 2, 
                                                         p[2], true, GetBackgroundValue());

  // Get AzygousVein and limit according to LowerBorderAzygousVein
  MaskImagePointer LowerBorderAzygousVein 
    = this->GetAFDB()->template GetImage<MaskImageType>("LowerBorderAzygousVein");
  std::vector<MaskImagePointType> c;
  clitk::ComputeCentroids<MaskImageType>(LowerBorderAzygousVein, GetBackgroundValue(), c);
  S4R = clitk::CropImageRemoveLowerThan<MaskImageType>(S4R, 2, 
                                                       c[1][2], true, GetBackgroundValue());
  S4R = clitk::AutoCrop<MaskImageType>(S4R, GetBackgroundValue());
  m_ListOfSupports["S4R"] = S4R;


  // Limit according to LeftPulmonaryArtery
  MaskImagePointer LeftPulmonaryArtery 
    = this->GetAFDB()->template GetImage<MaskImageType>("LeftPulmonaryArtery");
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(LeftPulmonaryArtery, GetBackgroundValue(), 
                                                          2, false, p);
  S4L = clitk::CropImageRemoveLowerThan<MaskImageType>(S4L, 2, 
                                                       p[2], true, GetBackgroundValue());
  S4L = clitk::AutoCrop<MaskImageType>(S4L, GetBackgroundValue());
  m_ListOfSupports["S4L"] = S4L;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
Support_LeftRight_S4R_S4L()
{
  // ---------------------------------------------------------------------------
  /* Step : S4RL LeftRight 
     
     - 4R: includes right paratracheal nodes, and pretracheal nodes
     extending to the left lateral border of trachea
     
     - 4L: includes nodes to the left of the left lateral border of
     the trachea, medial to the ligamentum arteriosum
     
     => same than 2RL
  */
  StartNewStep("[Support] Left Right separation of 4R/4L");

  MaskImagePointer S4R = m_ListOfSupports["S4R"];
  MaskImagePointer S4L = m_ListOfSupports["S4L"];
  S4R = LimitsWithTrachea(S4R, 0, 1, -10);
  S4L = LimitsWithTrachea(S4L, 0, 1, 10);
  m_ListOfSupports["S4R"] = S4R;
  m_ListOfSupports["S4L"] = S4L;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
typename clitk::ExtractLymphStationsFilter<ImageType>::MaskImagePointer
clitk::ExtractLymphStationsFilter<ImageType>::
LimitsWithTrachea(MaskImageType * input, int extremaDirection, int lineDirection, 
                  double offset) 
{
  MaskImagePointType min, max;
  GetMinMaxBoundary<MaskImageType>(input, min, max);
  return LimitsWithTrachea(input, extremaDirection, lineDirection, offset, max[2]);
}
template <class ImageType>
typename clitk::ExtractLymphStationsFilter<ImageType>::MaskImagePointer
clitk::ExtractLymphStationsFilter<ImageType>::
LimitsWithTrachea(MaskImageType * input, int extremaDirection, int lineDirection, 
                  double offset, double maxSupPosition)
{
  /*
    Take the input mask, consider the trachea and limit according to
    Left border of the trachea. Keep at Left or at Right according to
    the offset
  */
  // Read the trachea
  MaskImagePointer Trachea = this->GetAFDB()->template GetImage<MaskImageType>("Trachea");

  // Find extrema post positions
  std::vector<MaskImagePointType> tracheaLeftPositionsA;
  std::vector<MaskImagePointType> tracheaLeftPositionsB;

  // Crop Trachea only on the Sup-Inf axes, without autocrop
  //  Trachea = clitk::ResizeImageLike<MaskImageType>(Trachea, input, GetBackgroundValue());
  MaskImagePointType min, max;
  GetMinMaxBoundary<MaskImageType>(input, min, max);
  Trachea = clitk::CropImageAlongOneAxis<MaskImageType>(Trachea, 2, min[2], max[2], 
                                                        false, GetBackgroundValue()); 
  
  // Select the main CCL (because of bronchus)
  Trachea = SliceBySliceKeepMainCCL<MaskImageType>(Trachea, GetBackgroundValue(), GetForegroundValue());

  // Slice by slice, build the separation line 
  clitk::SliceBySliceBuildLineSegmentAccordingToExtremaPosition<MaskImageType>(Trachea, 
                                                                               GetBackgroundValue(), 2, 
                                                                               extremaDirection, false, // Left
                                                                               lineDirection, // Vertical line 
                                                                               -1, // margins 
                                                                               tracheaLeftPositionsA, 
                                                                               tracheaLeftPositionsB);
  // Do not consider trachea above the limit
  int indexMax=tracheaLeftPositionsA.size();
  for(uint i=0; i<tracheaLeftPositionsA.size(); i++) {
    if (tracheaLeftPositionsA[i][2] > maxSupPosition) {
      indexMax = i;
      i = tracheaLeftPositionsA.size(); // stop loop
    }
  }  
  tracheaLeftPositionsA.erase(tracheaLeftPositionsA.begin()+indexMax, tracheaLeftPositionsA.end());
  tracheaLeftPositionsB.erase(tracheaLeftPositionsB.begin()+indexMax, tracheaLeftPositionsB.end());

  // Cut post to this line 
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(input, 
                                                                    tracheaLeftPositionsA,
                                                                    tracheaLeftPositionsB,
                                                                    GetBackgroundValue(), 
                                                                    extremaDirection, offset); 
  MaskImagePointer output = clitk::AutoCrop<MaskImageType>(input, GetBackgroundValue());
  return output;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void
clitk::ExtractLymphStationsFilter<ImageType>::
Support_Post_S1S2S4()
{
  StartNewStep("[Support] Post limits of S1RL, S2RL, S4RL");
  
  double m_ApexOfTheChest = FindApexOfTheChest();
  
  // Post limits with Trachea 
  MaskImagePointer S1R = m_ListOfSupports["S1R"];
  MaskImagePointer S1L = m_ListOfSupports["S1L"];
  MaskImagePointer S2R = m_ListOfSupports["S2R"];
  MaskImagePointer S2L = m_ListOfSupports["S2L"];
  MaskImagePointer S4R = m_ListOfSupports["S4R"];
  MaskImagePointer S4L = m_ListOfSupports["S4L"];
  S1L = LimitsWithTrachea(S1L, 1, 0, -10, m_ApexOfTheChest);
  S1R = LimitsWithTrachea(S1R, 1, 0, -10, m_ApexOfTheChest);
  S2R = LimitsWithTrachea(S2R, 1, 0, -10, m_ApexOfTheChest);
  S2L = LimitsWithTrachea(S2L, 1, 0, -10, m_ApexOfTheChest);
  S4R = LimitsWithTrachea(S4R, 1, 0, -10, m_ApexOfTheChest);
  S4L = LimitsWithTrachea(S4L, 1, 0, -10, m_ApexOfTheChest);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void
clitk::ExtractLymphStationsFilter<ImageType>::
Support_S3P()
{
  StartNewStep("[Support] Ant limits of S3P and Post limits of S1RL, S2RL, S4RL");
  
  // Initial S3P support
  MaskImagePointer S3P = clitk::Clone<MaskImageType>(m_ListOfSupports["Support_Superior_to_Carina"]);

  // Stop at Lung Apex
  double m_ApexOfTheChest = FindApexOfTheChest();
  S3P = 
    clitk::CropImageRemoveGreaterThan<MaskImageType>(S3P, 2, 
                                                     m_ApexOfTheChest, true,
                                                     GetBackgroundValue());
  // Ant limits with Trachea
  S3P = LimitsWithTrachea(S3P, 1, 0, 10);
  m_ListOfSupports["S3P"] = S3P;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void
clitk::ExtractLymphStationsFilter<ImageType>::
Support_S3A()
{
  StartNewStep("[Support] Sup-Inf and Post limits for S3A");

  // Initial S3A support
  MaskImagePointer S3A = clitk::Clone<MaskImageType>(m_ListOfSupports["Support_Superior_to_Carina"]);

  // Stop at Lung Apex or like S2/S1 (upper border Sternum - manubrium) ?

  //double m_ApexOfTheChest = FindApexOfTheChest();

  MaskImagePointer Sternum = this->GetAFDB()->template GetImage <MaskImageType>("Sternum");
  MaskImagePointType p;
  p[0] = p[1] = p[2] =  0.0; // to avoid warning
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Sternum, GetBackgroundValue(), 2, false, p);
  p[2] += Sternum->GetSpacing()[2]; // we add one slice to stop 3A at the same slice than Sternum stop
  S3A = 
    clitk::CropImageRemoveGreaterThan<MaskImageType>(S3A, 2, 
                                                     //m_ApexOfTheChest
                                                     p[2], true,
                                                     GetBackgroundValue());
  // Ant limits with Trachea
  S3A = LimitsWithTrachea(S3A, 1, 0, -10);
  m_ListOfSupports["S3A"] = S3A;  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void
clitk::ExtractLymphStationsFilter<ImageType>::
Support_S5()
{
  StartNewStep("[Support] Sup-Inf limits S5 with Aorta and MainPulmonaryArtery");

  // Initial S5 support
  MaskImagePointer S5 = 
    clitk::Clone<MaskImageType>(this->GetAFDB()->template GetImage<MaskImageType>("Mediastinum", true));

  // Sup limits with Aorta
  double sup = FindInferiorBorderOfAorticArch();
  
  // Inf limits with "upper rim of the left main pulmonary artery"
  // For the moment only, it will change.
  MaskImagePointer MainPulmonaryArtery = this->GetAFDB()->template GetImage<MaskImageType>("MainPulmonaryArtery");
  MaskImagePointType p;
  p[0] = p[1] = p[2] =  0.0; // to avoid warning
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(MainPulmonaryArtery, GetBackgroundValue(), 2, false, p);
  p[2] += MainPulmonaryArtery->GetSpacing()[2];
  
  // Cut Sup/Inf
  S5 = clitk::CropImageAlongOneAxis<MaskImageType>(S5, 2, p[2], sup, true, GetBackgroundValue());

  m_ListOfSupports["S5"] = S5; 
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
void
clitk::ExtractLymphStationsFilter<ImageType>::
Support_S6()
{
  StartNewStep("[Support] Sup-Inf limits S6 with aorta");

  // Initial S6 support like S3A
  MaskImagePointer S6 = clitk::Clone<MaskImageType>(m_ListOfSupports["S3A"]);

  // Inf Sup limits with Aorta
  double sup = FindSuperiorBorderOfAorticArch();
  double inf = FindInferiorBorderOfAorticArch();
  
  // Cut Sup/Inf
  S6 = clitk::CropImageAlongOneAxis<MaskImageType>(S6, 2, inf, sup, true, GetBackgroundValue());

  m_ListOfSupports["S6"] = S6;  
}
//--------------------------------------------------------------------

