
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

  // Remove some computed structures
  this->GetAFDB()->RemoveTag("CarinaZ");
  this->GetAFDB()->RemoveTag("ApexOfTheChestZ");  

  // Superior and inferior limits.
  Support_SI_Limit("inferior", "Sup_to_Carina", "inferior", "Carina", 0); 
  Support_SI_Limit("superior", "Inf_to_Carina", "inferior", "Carina", m_Working_Support->GetSpacing()[2]); 

  // Initialise all others supports
  // m_ListOfSupports["S1R"] = m_ListOfSupports["Sup_to_Carina"];
  // m_ListOfSupports["S1L"] = m_ListOfSupports["Sup_to_Carina"];
  // m_ListOfSupports["S2R"] = m_ListOfSupports["Sup_to_Carina"];
  // m_ListOfSupports["S2L"] = m_ListOfSupports["Sup_to_Carina"];
  // m_ListOfSupports["S3A"] = m_ListOfSupports["Sup_to_Carina"];
  // m_ListOfSupports["S3P"] = m_ListOfSupports["Sup_to_Carina"];
  // m_ListOfSupports["S4R"] = m_ListOfSupports["Sup_to_Carina"];
  // m_ListOfSupports["S4L"] = m_ListOfSupports["Sup_to_Carina"];
  // m_ListOfSupports["S5"] = m_Mediastinum; // Not above Carina
  // m_ListOfSupports["S6"] = m_Mediastinum; // Not above Carina
  
  // Read all support limits in a file and apply them
  ReadSupportLimits(GetSupportLimitsFilename());  
  for(unsigned int i=0; i<m_ListOfSupportLimits.size(); i++) {
    SupportLimitsType s = m_ListOfSupportLimits[i];
    Support_SI_Limit(s.station_limit, s.station, s.structure_limit, 
                     s.structure, s.offset*m_Working_Support->GetSpacing()[2]);
  }

  // S1RL
  Support_LeftRight_S1R_S1L();

  // S2RL
  Support_LeftRight_S2R_S2L();

  // S4RL
  Support_LeftRight_S4R_S4L();
  
  // Post limits of S1,S2,S4
  Support_Post_S1S2S4();

  // S3P
  StartNewStep("[Support] Ant limits of S3P with trachea");
  m_ListOfSupports["S3P"] = LimitsWithTrachea(m_ListOfSupports["S3P"], 1, 0, 10);

  // S3A
  StartNewStep("[Support] Ant limits of S3A with trachea");
  m_ListOfSupports["S3A"] = LimitsWithTrachea(m_ListOfSupports["S3A"], 1, 0, -10);
  
  // I will do it later
  // Below Carina S7,8,9,10
  m_ListOfSupports["S7"] = clitk::Clone<MaskImageType>(m_ListOfSupports["Inf_to_Carina"]);
  m_ListOfSupports["S8"] = clitk::Clone<MaskImageType>(m_ListOfSupports["Inf_to_Carina"]);
  m_ListOfSupports["S9"] = clitk::Clone<MaskImageType>(m_ListOfSupports["Inf_to_Carina"]);
  m_ListOfSupports["S10"] = clitk::Clone<MaskImageType>(m_ListOfSupports["Inf_to_Carina"]);
  m_ListOfSupports["S11"] = clitk::Clone<MaskImageType>(m_ListOfSupports["Inf_to_Carina"]);

  // Store image filenames into AFDB 
  WriteImageSupport("S1R"); WriteImageSupport("S1L");
  WriteImageSupport("S2R"); WriteImageSupport("S2L");
  WriteImageSupport("S3A"); WriteImageSupport("S3P");
  WriteImageSupport("S4R"); WriteImageSupport("S4L");
  WriteImageSupport("S5");
  WriteImageSupport("S6");
  WriteImageSupport("S7");
  WriteImageSupport("S8");
  WriteImageSupport("S9");
  WriteImageSupport("S10");
  WriteImageSupport("S11");
  WriteAFDB();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
Support_SI_Limit(const std::string station_limit, const std::string station, 
                 const std::string structure_limit, const std::string structure, 
                 const double offset)
{
  if (!GetCheckSupportFlag()) 
    StartNewStep("[Support] "+station_limit+" limit of "+station+" is "+structure_limit+" limit of "+structure);

  // Check
  if ((station_limit != "superior") && (station_limit != "inferior")) {
    clitkExceptionMacro("Error station_limit must be 'inferior' or 'superior', not '"<< station_limit);
  }
  if ((structure_limit != "superior") && (structure_limit != "inferior")) {
    clitkExceptionMacro("Error structure_limit must be 'inferior' or 'superior', not '"<< structure_limit);
  }

  // Get current support
  if (m_ListOfSupports.find(station) == m_ListOfSupports.end()) {
    // std::cerr << "Warning: support " << station << " not initialized" << std::endl;
    m_ListOfSupports[station] = m_Mediastinum;
  }
  m_Working_Support = m_ListOfSupports[station];
  
  // Get structure or structureZ
  double z=0.;
  int found=0;
  std::string file;

  // Try to load structure and compute extrema point
  if (this->GetAFDB()->TagExist(structure)) {
    MaskImagePointer Structure = this->GetAFDB()->template GetImage <MaskImageType>(structure);
    file = this->GetAFDB()->GetTagValue(structure);
    MaskImagePointType p;
    p[0] = p[1] = p[2] =  0.0; // to avoid warning
    if (structure_limit == "superior") 
      clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Structure, GetBackgroundValue(), 2, false, p);
    else 
      clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Structure, GetBackgroundValue(), 2, true, p);
    z = p[2];
    found=1;
  }

  // Try to load structureZ
  if ((found==0) && (this->GetAFDB()->TagExist(structure+"Z"))) {
    z = this->GetAFDB()->GetDouble(structure+"Z");
    found=2;
  }
  
  // Try to load structurePoint
  if ((found==0) && (this->GetAFDB()->TagExist(structure+"Point"))) {
    MaskImagePointType p;
    this->GetAFDB()->GetPoint3D(structure+"Point", p);
    z = p[2];
    found=3;
  }

  // Try to see if it is an already computed support
  if (found==0) {
    if (m_ListOfSupports.find(structure) != m_ListOfSupports.end()) {
      MaskImagePointer Structure = m_ListOfSupports[structure];
      MaskImagePointType p;
      if (structure_limit == "superior") 
        clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Structure, GetBackgroundValue(), 2, false, p);
      else 
        clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Structure, GetBackgroundValue(), 2, true, p);
      z = p[2];
      found=4;
    }
  }
  
  // Try special case : "FindApexOfTheChest"
  if (structure == "FindApexOfTheChest") {
    z = FindApexOfTheChest();
    found=5;
  }
  if (structure == "FindInferiorBorderOfAorticArch") {
    z = FindInferiorBorderOfAorticArch();
    found=6;
  }
  if (structure == "FindSuperiorBorderOfAorticArch") {
    z = FindSuperiorBorderOfAorticArch();
    found=6;
  }

  // If we find anything
  if (found == 0) {
    std::cerr << "ERROR : I could not find " << structure << " nor " << structure << "Z nor " 
              << structure << "Point" << std::endl;
    exit(EXIT_FAILURE);
  }

  // Apply offset
  z += offset;

  // Remove Lower or greater
  if (station_limit == "inferior") {
    m_Working_Support = 
      clitk::CropImageRemoveLowerThan<MaskImageType>(m_Working_Support, 2, z, true, GetBackgroundValue());
  }
  else {
    m_Working_Support = 
      clitk::CropImageRemoveGreaterThan<MaskImageType>(m_Working_Support, 2, z, true, GetBackgroundValue());
  }
  
  // Check: if reference station is given, display information
  if (GetCheckSupportFlag())  {
    try {
      MaskImagePointer Ref = this->GetAFDB()->template GetImage <MaskImageType>(station+"_Ref");
      MaskImagePointType p_support;
      MaskImagePointType p_ref;
      if (station_limit == "superior") {
        clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Ref, GetBackgroundValue(), 2, false, p_ref);
        clitk::FindExtremaPointInAGivenDirection<MaskImageType>(m_Working_Support, GetBackgroundValue(), 2, false, p_support);
      }
      else {
        clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Ref, GetBackgroundValue(), 2, true, p_ref);
        clitk::FindExtremaPointInAGivenDirection<MaskImageType>(m_Working_Support, GetBackgroundValue(), 2, true, p_support);
      }
      std::ostringstream os;
      os << "[Support] \t" << station << "\t" << station_limit << " "
         << "Z = " << z << std::setprecision(2) << std::fixed
         << "\tSupport = " << p_support[2]
         << "\tRef = " << p_ref[2]
         << "\tdiff = " << p_support[2]-p_ref[2] << "\t"
         << structure << " " << structure_limit;
      if (found==1) os << " (S "+file+")";
      if (found==2) os << " (Z)";
      if (found==3) os << " (P)";
      if (found==4) os << " (p)";
      if (found==5) os << " (Apex)";
      if (found==6) os << " (AorticArch)";
      StartNewStep(os.str());
    } catch(clitk::ExceptionObject e) { }
  }
  
  // Set support
  m_ListOfSupports[station] = m_Working_Support;  
  StopCurrentStep<MaskImageType>(m_Working_Support);  
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
  MaskImagePointer S1RL = m_ListOfSupports["S1R"];
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
                                                                    GetBackgroundValue(), 0, 10);
  S1R = clitk::AutoCrop<MaskImageType>(S1R, GetBackgroundValue());
  m_ListOfSupports["S1R"] = S1R;

  // Left part
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(S1L, A, B, 
                                                                    GetBackgroundValue(), 0, -10);
  S1L = clitk::AutoCrop<MaskImageType>(S1L, GetBackgroundValue());
  m_ListOfSupports["S1L"] = S1L;
  StopCurrentStep<MaskImageType>(m_ListOfSupports["S1L"]);
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
  S2R = clitk::AutoCrop<MaskImageType>(S2R, GetBackgroundValue());
  S2L = clitk::AutoCrop<MaskImageType>(S2L, GetBackgroundValue());
  m_ListOfSupports["S2R"] = S2R;
  m_ListOfSupports["S2L"] = S2L;  
  this->GetAFDB()->template ReleaseImage<MaskImageType>("Trachea");
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
  m_ListOfSupports["S1R"] = LimitsWithTrachea(S1L, 1, 0, -10, m_ApexOfTheChest);
  m_ListOfSupports["S1L"] = LimitsWithTrachea(S1R, 1, 0, -10, m_ApexOfTheChest);
  m_ListOfSupports["S2R"] = LimitsWithTrachea(S2R, 1, 0, -10, m_ApexOfTheChest);
  m_ListOfSupports["S2L"] = LimitsWithTrachea(S2L, 1, 0, -10, m_ApexOfTheChest);
  m_ListOfSupports["S4R"] = LimitsWithTrachea(S4R, 1, 0, -10, m_ApexOfTheChest);
  m_ListOfSupports["S4L"] = LimitsWithTrachea(S4L, 1, 0, -10, m_ApexOfTheChest);
}
//--------------------------------------------------------------------


