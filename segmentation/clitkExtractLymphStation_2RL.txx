

// vtk
#include <vtkAppendPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkCellArray.h>

// clitk
#include "clitkMeshToBinaryImageFilter.h"

// itk
#include <itkImageDuplicator.h>

//--------------------------------------------------------------------
template<class PointType>
class comparePointsX {
public:
  bool operator() (PointType i, PointType j) { return (i[0]<j[0]); }
};
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class PairType>
class comparePointsWithAngle {
public:
  bool operator() (PairType i, PairType j) { return (i.second < j.second); } 
};
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<int Dim>
void HypercubeCorners(std::vector<itk::Point<double, Dim> > & out) {
  std::vector<itk::Point<double, Dim-1> > previous;
  HypercubeCorners<Dim-1>(previous);
  out.resize(previous.size()*2);
  for(unsigned int i=0; i<out.size(); i++) {
    itk::Point<double, Dim> p;
    if (i<previous.size()) p[0] = 0; 
    else p[0] = 1;
    for(int j=0; j<Dim-1; j++) 
      {
        p[j+1] = previous[i%previous.size()][j];
      }
    out[i] = p;
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<>
void HypercubeCorners<1>(std::vector<itk::Point<double, 1> > & out) {
  out.resize(2);
  out[0][0] = 0;
  out[1][0] = 1;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void ComputeImageBoundariesCoordinates(typename ImageType::Pointer image, 
                                       std::vector<typename ImageType::PointType> & bounds) 
{
  // Get image max/min coordinates
  const unsigned int dim=ImageType::ImageDimension;
  typedef typename ImageType::PointType PointType;
  typedef typename ImageType::IndexType IndexType;
  PointType min_c, max_c;
  IndexType min_i, max_i;
  min_i = image->GetLargestPossibleRegion().GetIndex();
  for(unsigned int i=0; i<dim; i++)
    max_i[i] = image->GetLargestPossibleRegion().GetSize()[i] + min_i[i];
  image->TransformIndexToPhysicalPoint(min_i, min_c);
  image->TransformIndexToPhysicalPoint(max_i, max_c);
  
  // Get corners coordinates
  HypercubeCorners<ImageType::ImageDimension>(bounds);
  for(unsigned int i=0; i<bounds.size(); i++) {
    for(unsigned int j=0; j<dim; j++) {
      if (bounds[i][j] == 0) bounds[i][j] = min_c[j];
      if (bounds[i][j] == 1) bounds[i][j] = max_c[j];
    }
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_SetDefaultValues()
{
  SetFuzzyThreshold("2RL", "CommonCarotidArtery", 0.7);
  SetFuzzyThreshold("2RL", "BrachioCephalicArtery", 0.7);
  SetFuzzyThreshold("2RL", "BrachioCephalicVein", 0.3);
  SetFuzzyThreshold("2RL", "Aorta", 0.7);
  SetFuzzyThreshold("2RL", "SubclavianArteryRight", 0.5);
  SetFuzzyThreshold("2RL", "SubclavianArteryLeft", 0.8);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_SI_Limits() 
{
  // Apex of the chest or Sternum & Carina.
  StartNewStep("[Station 2RL] Inf/Sup limits with Sternum and TopOfAorticArch/CaudalMarginOfLeftBrachiocephalicVein");

  /* Rod says: "For the inferior border, unlike in Atlas – UM, there
   is now a difference between 2R and 2L.  2R stops at the
   intersection of the caudal margin of the innominate vein with the
   trachea.  2L extends less inferiorly to the superior border of the
   aortic arch." */

  /* Get TopOfAorticArch and CaudalMarginOfLeftBrachiocephalicVein 
     - TopOfAorticArch -> can be obtain from Aorta -> most sup part.  
     - CaudalMarginOfLeftBrachiocephalicVein -> must inf part of BrachicephalicVein
   */
  MaskImagePointer Aorta = GetAFDB()->template GetImage<MaskImageType>("Aorta");
  MaskImagePointType p = Aorta->GetOrigin(); // initialise to avoid warning 
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Aorta, GetBackgroundValue(), 2, false, p);
  double TopOfAorticArchZ=p[2];
  GetAFDB()->SetDouble("TopOfAorticArchZ", TopOfAorticArchZ);

  MaskImagePointer BrachioCephalicVein = GetAFDB()->template GetImage<MaskImageType>("BrachioCephalicVein");
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(BrachioCephalicVein, GetBackgroundValue(), 2, true, p);
  double CaudalMarginOfLeftBrachiocephalicVeinZ=p[2];
  GetAFDB()->SetDouble("CaudalMarginOfLeftBrachiocephalicVeinZ", CaudalMarginOfLeftBrachiocephalicVeinZ);
  
  // First, cut on the most inferior part. Add one slice because this
  // inf slice should not be included.
  double inf = std::min(CaudalMarginOfLeftBrachiocephalicVeinZ, TopOfAorticArchZ) + m_Working_Support->GetSpacing()[2];

  // Get Sternum and search for the upper position
  MaskImagePointer Sternum = GetAFDB()->template GetImage<MaskImageType>("Sternum");

  // Search most sup point
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Sternum, GetBackgroundValue(), 2, false, p);
  double m_SternumZ = p[2];
  // +Sternum->GetSpacing()[2]; // One more slice, because it is below this point // NOT HERE ?? WHY DIFFERENT FROM 3A ??

  //* Crop support :
  m_Working_Support = 
    clitk::CropImageAlongOneAxis<MaskImageType>(m_Working_Support, 2, 
                                                inf, m_SternumZ, true,
                                                GetBackgroundValue());

  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_Ant_Limits() 
{
  // -----------------------------------------------------
  /* Rod says: "The anterior border, as with the Atlas – UM, is
    posterior to the vessels (right subclavian vein, left
    brachiocephalic vein, right brachiocephalic vein, left subclavian
    artery, left common carotid artery and brachiocephalic trunk).
    These vessels are not included in the nodal station.  The anterior
    border is drawn to the midpoint of the vessel and an imaginary
    line joins the middle of these vessels.  Between the vessels,
    station 2 is in contact with station 3a." */

  // -----------------------------------------------------
  StartNewStep("[Station 2RL] Ant limits with CommonCarotidArtery");

  // Get CommonCarotidArtery
  MaskImagePointer CommonCarotidArtery = GetAFDB()->template GetImage<MaskImageType>("CommonCarotidArtery");
  
  // Remove Ant to CommonCarotidArtery
  typedef SliceBySliceRelativePositionFilter<MaskImageType> SliceRelPosFilterType;
  typename SliceRelPosFilterType::Pointer sliceRelPosFilter = SliceRelPosFilterType::New();
  sliceRelPosFilter->VerboseStepFlagOff();
  sliceRelPosFilter->WriteStepFlagOff();
  sliceRelPosFilter->SetInput(m_Working_Support);
  sliceRelPosFilter->SetInputObject(CommonCarotidArtery);
  sliceRelPosFilter->SetDirection(2);
  sliceRelPosFilter->SetFuzzyThreshold(GetFuzzyThreshold("2RL", "CommonCarotidArtery"));
  sliceRelPosFilter->AddOrientationTypeString("NotAntTo");
  sliceRelPosFilter->IntermediateSpacingFlagOn();
  sliceRelPosFilter->SetIntermediateSpacing(2);
  sliceRelPosFilter->UniqueConnectedComponentBySliceOff();
  sliceRelPosFilter->UseASingleObjectConnectedComponentBySliceFlagOff();
  sliceRelPosFilter->AutoCropFlagOn(); 
  sliceRelPosFilter->IgnoreEmptySliceObjectFlagOn();
  sliceRelPosFilter->RemoveObjectFlagOff();
  sliceRelPosFilter->Update();
  m_Working_Support = sliceRelPosFilter->GetOutput();

  // End
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;

  // -----------------------------------------------------
  // Remove Ant to H line from the Ant most part of the
  // CommonCarotidArtery until we reach the first slice of
  // BrachioCephalicArtery
  StartNewStep("[Station 2RL] Ant limits with CommonCarotidArtery, H line");

  // First, find the first slice of BrachioCephalicArtery
  MaskImagePointer BrachioCephalicArtery = GetAFDB()->template GetImage<MaskImageType>("BrachioCephalicArtery");
  MaskImagePointType p = BrachioCephalicArtery->GetOrigin(); // initialise to avoid warning 
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(BrachioCephalicArtery, GetBackgroundValue(), 2, false, p);
  double TopOfBrachioCephalicArteryZ=p[2] + BrachioCephalicArtery->GetSpacing()[2]; // Add one slice

  // Remove CommonCarotidArtery below this point
  CommonCarotidArtery = clitk::CropImageRemoveLowerThan<MaskImageType>(CommonCarotidArtery, 2, TopOfBrachioCephalicArteryZ, true, GetBackgroundValue());  

  // Find most Ant points
  std::vector<MaskImagePointType> ccaAntPositionA;
  std::vector<MaskImagePointType> ccaAntPositionB;
  clitk::SliceBySliceBuildLineSegmentAccordingToExtremaPosition<MaskImageType>(CommonCarotidArtery, 
                                                                               GetBackgroundValue(), 2,
                                                                               1, true, // Ant
                                                                               0, // Horizontal line
                                                                               -3, // margin
                                                                               ccaAntPositionA, 
                                                                               ccaAntPositionB);
  // Cut ant to this line
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    ccaAntPositionA,
                                                                    ccaAntPositionB,
                                                                    GetBackgroundValue(), 1, 10); 

  // End
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;

  // -----------------------------------------------------
  // Ant limit with the BrachioCephalicArtery
  StartNewStep("[Station 2RL] Ant limits with BrachioCephalicArtery line");

  // Remove Ant to BrachioCephalicArtery
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, BrachioCephalicArtery, 2, 
                                                       GetFuzzyThreshold("2RL", "BrachioCephalicArtery"), "NotAntTo", false, 2, true, false);
  // End
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;

  // -----------------------------------------------------
  // Ant limit with the BrachioCephalicArtery H line
  StartNewStep("[Station 2RL] Ant limits with BrachioCephalicArtery, Horizontal line");
  
  // Find most Ant points
  std::vector<MaskImagePointType> bctAntPositionA;
  std::vector<MaskImagePointType> bctAntPositionB;
  clitk::SliceBySliceBuildLineSegmentAccordingToExtremaPosition<MaskImageType>(BrachioCephalicArtery, 
                                                                               GetBackgroundValue(), 2,
                                                                               1, true, // Ant
                                                                               0, // Horizontal line
                                                                               -1, // margin
                                                                               bctAntPositionA, 
                                                                               bctAntPositionB);
  // Cut ant to this line
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    bctAntPositionA,
                                                                    bctAntPositionB,
                                                                    GetBackgroundValue(), 1, 10); 
 // End
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;

  // -----------------------------------------------------
  // Ant limit with the BrachioCephalicVein
  StartNewStep("[Station 2RL] Ant limits with BrachioCephalicVein");

  // Remove Ant to BrachioCephalicVein
  MaskImagePointer BrachioCephalicVein = GetAFDB()->template GetImage<MaskImageType>("BrachioCephalicVein");
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, BrachioCephalicVein, 2, 
                                                       GetFuzzyThreshold("2RL", "BrachioCephalicVein"), "NotAntTo", false, 2, true, false);
  // End
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_Ant_Limits2() 
{
  // -----------------------------------------------------
  /* Rod says: "The anterior border, as with the Atlas – UM, is
    posterior to the vessels (right subclavian vein, left
    brachiocephalic vein, right brachiocephalic vein, left subclavian
    artery, left common carotid artery and brachiocephalic trunk).
    These vessels are not included in the nodal station.  The anterior
    border is drawn to the midpoint of the vessel and an imaginary
    line joins the middle of these vessels.  Between the vessels,
    station 2 is in contact with station 3a." */

  // -----------------------------------------------------
  StartNewStep("[Station 2RL] Ant limits with vessels centroids");

  /* Here, we consider the vessels form a kind of anterior barrier. We
     link all vessels centroids and remove what is post to it.
    - select the list of structure
            vessel1 = BrachioCephalicArtery
            vessel2 = BrachioCephalicVein (warning several CCL, keep most at Right)
            vessel3 = CommonCarotidArtery
            vessel4 = SubclavianArtery
            other   = Thyroid
            other   = Aorta 
     - crop images as needed
     - slice by slice, choose the CCL and extract centroids
     - slice by slice, sort according to polar angle wrt Trachea centroid.
     - slice by slice, link centroids and close contour
     - remove outside this contour
     - merge with support 
  */

  // Read structures
  MaskImagePointer BrachioCephalicArtery = GetAFDB()->template GetImage<MaskImageType>("BrachioCephalicArtery");
  MaskImagePointer BrachioCephalicVein = GetAFDB()->template GetImage<MaskImageType>("BrachioCephalicVein");
  MaskImagePointer CommonCarotidArtery = GetAFDB()->template GetImage<MaskImageType>("CommonCarotidArtery");
  MaskImagePointer SubclavianArtery = GetAFDB()->template GetImage<MaskImageType>("SubclavianArtery");
  MaskImagePointer Thyroid = GetAFDB()->template GetImage<MaskImageType>("Thyroid");
  MaskImagePointer Aorta = GetAFDB()->template GetImage<MaskImageType>("Aorta");
  MaskImagePointer Trachea = GetAFDB()->template GetImage<MaskImageType>("Trachea");
  
  // Resize all structures like support
  BrachioCephalicArtery = 
    clitk::ResizeImageLike<MaskImageType>(BrachioCephalicArtery, m_Working_Support, GetBackgroundValue());
  CommonCarotidArtery = 
    clitk::ResizeImageLike<MaskImageType>(CommonCarotidArtery, m_Working_Support, GetBackgroundValue());
  SubclavianArtery = 
    clitk::ResizeImageLike<MaskImageType>(SubclavianArtery, m_Working_Support, GetBackgroundValue());
  Thyroid = 
    clitk::ResizeImageLike<MaskImageType>(Thyroid, m_Working_Support, GetBackgroundValue());
  Aorta = 
    clitk::ResizeImageLike<MaskImageType>(Aorta, m_Working_Support, GetBackgroundValue());
  BrachioCephalicVein = 
    clitk::ResizeImageLike<MaskImageType>(BrachioCephalicVein, m_Working_Support, GetBackgroundValue());
  Trachea = 
    clitk::ResizeImageLike<MaskImageType>(Trachea, m_Working_Support, GetBackgroundValue());

  // Extract slices
  std::vector<MaskSlicePointer> slices_BrachioCephalicArtery;
  clitk::ExtractSlices<MaskImageType>(BrachioCephalicArtery, 2, slices_BrachioCephalicArtery);
  std::vector<MaskSlicePointer> slices_BrachioCephalicVein;
  clitk::ExtractSlices<MaskImageType>(BrachioCephalicVein, 2, slices_BrachioCephalicVein);
  std::vector<MaskSlicePointer> slices_CommonCarotidArtery;
  clitk::ExtractSlices<MaskImageType>(CommonCarotidArtery, 2, slices_CommonCarotidArtery);
  std::vector<MaskSlicePointer> slices_SubclavianArtery;
  clitk::ExtractSlices<MaskImageType>(SubclavianArtery, 2, slices_SubclavianArtery);
  std::vector<MaskSlicePointer> slices_Thyroid;
  clitk::ExtractSlices<MaskImageType>(Thyroid, 2, slices_Thyroid);
  std::vector<MaskSlicePointer> slices_Aorta;
  clitk::ExtractSlices<MaskImageType>(Aorta, 2, slices_Aorta);
  std::vector<MaskSlicePointer> slices_Trachea;
  clitk::ExtractSlices<MaskImageType>(Trachea, 2, slices_Trachea);
  unsigned int n= slices_BrachioCephalicArtery.size();
  
  // Get the boundaries of one slice
  std::vector<MaskSlicePointType> bounds;
  ComputeImageBoundariesCoordinates<MaskSliceType>(slices_BrachioCephalicArtery[0], bounds);

  // For all slices, for all structures, find the centroid and build the contour
  // List of 3D points (for debug)
  std::vector<MaskImagePointType> p3D;

  vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
  for(unsigned int i=0; i<n; i++) {
    // Labelize the slices
    slices_CommonCarotidArtery[i] = Labelize<MaskSliceType>(slices_CommonCarotidArtery[i], 
                                                            GetBackgroundValue(), true, 1);
    slices_SubclavianArtery[i] = Labelize<MaskSliceType>(slices_SubclavianArtery[i], 
                                                         GetBackgroundValue(), true, 1);
    slices_BrachioCephalicArtery[i] = Labelize<MaskSliceType>(slices_BrachioCephalicArtery[i], 
                                                             GetBackgroundValue(), true, 1);
    slices_BrachioCephalicVein[i] = Labelize<MaskSliceType>(slices_BrachioCephalicVein[i], 
                                                            GetBackgroundValue(), true, 1);
    slices_Thyroid[i] = Labelize<MaskSliceType>(slices_Thyroid[i], 
                                                GetBackgroundValue(), true, 1);
    slices_Aorta[i] = Labelize<MaskSliceType>(slices_Aorta[i], 
                                              GetBackgroundValue(), true, 1);

    // Search centroids
    std::vector<MaskSlicePointType> points2D;
    std::vector<MaskSlicePointType> centroids1;
    std::vector<MaskSlicePointType> centroids2;
    std::vector<MaskSlicePointType> centroids3;
    std::vector<MaskSlicePointType> centroids4;
    std::vector<MaskSlicePointType> centroids5;
    std::vector<MaskSlicePointType> centroids6;
    ComputeCentroids<MaskSliceType>(slices_CommonCarotidArtery[i], GetBackgroundValue(), centroids1);
    ComputeCentroids<MaskSliceType>(slices_SubclavianArtery[i], GetBackgroundValue(), centroids2);
    ComputeCentroids<MaskSliceType>(slices_BrachioCephalicArtery[i], GetBackgroundValue(), centroids3);
    ComputeCentroids<MaskSliceType>(slices_Thyroid[i], GetBackgroundValue(), centroids4);
    ComputeCentroids<MaskSliceType>(slices_Aorta[i], GetBackgroundValue(), centroids5);
    ComputeCentroids<MaskSliceType>(slices_BrachioCephalicVein[i], GetBackgroundValue(), centroids6);

    // BrachioCephalicVein -> when it is separated into two CCL, we
    // only consider the most at Right one
    if (centroids6.size() > 2) {
      if (centroids6[1][0] < centroids6[2][0]) centroids6.erase(centroids6.begin()+2);
      else centroids6.erase(centroids6.begin()+1);
    }
    
    // BrachioCephalicVein -> when SubclavianArtery has 2 CCL
    // (BrachioCephalicArtery is divided) -> forget BrachioCephalicVein
    if ((centroids3.size() ==1) && (centroids2.size() > 2)) {
      centroids6.clear();
    }

    for(unsigned int j=1; j<centroids1.size(); j++) points2D.push_back(centroids1[j]);
    for(unsigned int j=1; j<centroids2.size(); j++) points2D.push_back(centroids2[j]);
    for(unsigned int j=1; j<centroids3.size(); j++) points2D.push_back(centroids3[j]);
    for(unsigned int j=1; j<centroids4.size(); j++) points2D.push_back(centroids4[j]);
    for(unsigned int j=1; j<centroids5.size(); j++) points2D.push_back(centroids5[j]);
    for(unsigned int j=1; j<centroids6.size(); j++) points2D.push_back(centroids6[j]);
    
    // Sort by angle according to trachea centroid and vertical line,
    // in polar coordinates :
    // http://en.wikipedia.org/wiki/Polar_coordinate_system
    std::vector<MaskSlicePointType> centroids_trachea;
    ComputeCentroids<MaskSliceType>(slices_Trachea[i], GetBackgroundValue(), centroids_trachea);
    typedef std::pair<MaskSlicePointType, double> PointAngleType;
    std::vector<PointAngleType> angles;
    for(unsigned int j=0; j<points2D.size(); j++) {
      //double r = centroids_trachea[1].EuclideanDistanceTo(points2D[j]);
      double x = (points2D[j][0]-centroids_trachea[1][0]); // X : Right to Left
      double y = (centroids_trachea[1][1]-points2D[j][1]); // Y : Post to Ant
      double angle = 0;
      if (x>0) angle = atan(y/x);
      if ((x<0) && (y>=0)) angle = atan(y/x)+M_PI;
      if ((x<0) && (y<0)) angle = atan(y/x)-M_PI;
      if (x==0) {
        if (y>0) angle = M_PI/2.0;
        if (y<0) angle = -M_PI/2.0;
        if (y==0) angle = 0;
      }
      angle = clitk::rad2deg(angle);
      // Angle is [-180;180] wrt the X axis. We change the X axis to
      // be the vertical line, because we want to sort from Right to
      // Left from Post to Ant.
      if (angle>0) 
        angle = (270-angle);
      if (angle<0) {
        angle = -angle-90;
        if (angle<0) angle = 360-angle;
      }
      PointAngleType a;
      a.first = points2D[j];
      a.second = angle;
      angles.push_back(a);
    }

    // Do nothing if less than 2 points --> n
    if (points2D.size() < 3) { //continue;
      continue;
    }

    // Sort points2D according to polar angles
    std::sort(angles.begin(), angles.end(), comparePointsWithAngle<PointAngleType>());
    for(unsigned int j=0; j<angles.size(); j++) {
      points2D[j] = angles[j].first;
    }
    //    DDV(points2D, points2D.size());

    /* When vessels are far away, we try to replace the line segment
       with a curved line that join the two vessels but stay
       approximately at the same distance from the trachea centroids
       than the vessels.

       For that: 
       - let a and c be two successive vessels centroids
       - id distance(a,c) < threshold, next point

       TODO HERE
       
       - compute mid position between two successive points -
       compute dist to trachea centroid for the 3 pts - if middle too
       low, add one point
    */
    std::vector<MaskSlicePointType> toadd;
    unsigned int index = 0;
    double dmax = 5;
    while (index<points2D.size()-1) {
      MaskSlicePointType a = points2D[index];
      MaskSlicePointType c = points2D[index+1];

      double dac = a.EuclideanDistanceTo(c);
      if (dac>dmax) {
        
        MaskSlicePointType b;
        b[0] = a[0]+(c[0]-a[0])/2.0;
        b[1] = a[1]+(c[1]-a[1])/2.0;
        
        // Compute distance to trachea centroid
        MaskSlicePointType m = centroids_trachea[1];
        double da = m.EuclideanDistanceTo(a);
        double db = m.EuclideanDistanceTo(b);
        //double dc = m.EuclideanDistanceTo(c);
        
        // Mean distance, find point on the line from trachea centroid
        // to b
        double alpha = (da+db)/2.0;
        MaskSlicePointType v;
        double n = sqrt( pow(b[0]-m[0], 2) + pow(b[1]-m[1], 2));
        v[0] = (b[0]-m[0])/n;
        v[1] = (b[1]-m[1])/n;
        MaskSlicePointType r;
        r[0] = m[0]+alpha*v[0];
        r[1] = m[1]+alpha*v[1];
        points2D.insert(points2D.begin()+index+1, r);
      }
      else {
        index++;
      }
    }
    //    DDV(points2D, points2D.size());

    // Add some points to close the contour 
    // - H line towards Right
    MaskSlicePointType p = points2D[0]; 
    p[0] = bounds[0][0];
    points2D.insert(points2D.begin(), p);
    // - corner Right/Post
    p = bounds[0];
    points2D.insert(points2D.begin(), p);
    // - H line towards Left
    p = points2D.back(); 
    p[0] = bounds[2][0];
    points2D.push_back(p);
    // - corner Right/Post
    p = bounds[2];
    points2D.push_back(p);
    // Close contour with the first point
    points2D.push_back(points2D[0]);
    //    DDV(points2D, points2D.size());
      
    // Build 3D points from the 2D points
    std::vector<ImagePointType> points3D;
    clitk::PointsUtils<MaskImageType>::Convert2DListTo3DList(points2D, i, m_Working_Support, points3D);
    for(unsigned int x=0; x<points3D.size(); x++) p3D.push_back(points3D[x]);

    // Build the mesh from the contour's points
    vtkSmartPointer<vtkPolyData> mesh = Build3DMeshFrom2DContour(points3D);
    append->AddInput(mesh);
  }

  // DEBUG: write points3D
  clitk::WriteListOfLandmarks<MaskImageType>(p3D, "vessels-centroids.txt");

  // Build the final 3D mesh form the list 2D mesh
  append->Update();
  vtkSmartPointer<vtkPolyData> mesh = append->GetOutput();
  
  // Debug, write the mesh
  /*
    vtkSmartPointer<vtkPolyDataWriter> w = vtkSmartPointer<vtkPolyDataWriter>::New();
    w->SetInput(mesh);
    w->SetFileName("bidon.vtk");
    w->Write();    
  */
  
  // Compute a single binary 3D image from the list of contours
  clitk::MeshToBinaryImageFilter<MaskImageType>::Pointer filter = 
    clitk::MeshToBinaryImageFilter<MaskImageType>::New();
  filter->SetMesh(mesh);
  filter->SetLikeImage(m_Working_Support);
  filter->Update();
  MaskImagePointer binarizedContour = filter->GetOutput();  
  
  // Inverse binary mask if needed. We test a point that we know must be in FG. If it is not, inverse
  ImagePointType p = p3D[2]; // This is the first centroid of the first slice
  p[1] += 50; // 50 mm Post from this point must be kept
  ImageIndexType index;
  binarizedContour->TransformPhysicalPointToIndex(p, index);
  bool isInside = (binarizedContour->GetPixel(index) != GetBackgroundValue());

  // remove from support
  typedef clitk::BooleanOperatorLabelImageFilter<MaskImageType> BoolFilterType;
  typename BoolFilterType::Pointer boolFilter = BoolFilterType::New(); 
  boolFilter->InPlaceOn();
  boolFilter->SetInput1(m_Working_Support);
  boolFilter->SetInput2(binarizedContour);
  boolFilter->SetBackgroundValue1(GetBackgroundValue());
  boolFilter->SetBackgroundValue2(GetBackgroundValue());
  if (isInside)
    boolFilter->SetOperationType(BoolFilterType::And);
  else
    boolFilter->SetOperationType(BoolFilterType::AndNot);
  boolFilter->Update();
  m_Working_Support = boolFilter->GetOutput();

  // End
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_Post_Limits() 
{
  StartNewStep("[Station 2RL] Post limits with post wall of Trachea");

  // Get Trachea
  MaskImagePointer Trachea = GetAFDB()->template GetImage<MaskImageType>("Trachea");
  
  // Resize like the current support (to have the same number of slices)
  Trachea = clitk::ResizeImageLike<MaskImageType>(Trachea, m_Working_Support, GetBackgroundValue());

  // Find extrema post positions
  std::vector<MaskImagePointType> tracheaPostPositionsA;
  std::vector<MaskImagePointType> tracheaPostPositionsB;
  clitk::SliceBySliceBuildLineSegmentAccordingToExtremaPosition<MaskImageType>(Trachea, 
                                                                               GetBackgroundValue(), 2, 
                                                                               1, false, // Post
                                                                               0, // Horizontal line 
                                                                               1, 
                                                                               tracheaPostPositionsA, 
                                                                               tracheaPostPositionsB);
  // Cut post to this line
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    tracheaPostPositionsA,
                                                                    tracheaPostPositionsB,
                                                                    GetBackgroundValue(), 1, -10); 
  // END
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Build a vtk mesh from a list of slice number/closed-contours
template <class ImageType>
vtkSmartPointer<vtkPolyData> 
clitk::ExtractLymphStationsFilter<ImageType>::
Build3DMeshFrom2DContour(const std::vector<ImagePointType> & points)
{
  // create a contour, polydata. 
  vtkSmartPointer<vtkPolyData> mesh = vtkSmartPointer<vtkPolyData>::New();
  mesh->Allocate(); //for cell structures
  mesh->SetPoints(vtkPoints::New());
  vtkIdType ids[2];
  int point_number = points.size();
  for (unsigned int i=0; i<points.size(); i++) {
    mesh->GetPoints()->InsertNextPoint(points[i][0],points[i][1],points[i][2]);
    ids[0]=i;
    ids[1]=(ids[0]+1)%point_number; //0-1,1-2,...,n-1-0
    mesh->GetLines()->InsertNextCell(2,ids);
  }
  // Return
  return mesh;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_LR_Limits()
{
  // ---------------------------------------------------------------------------
  StartNewStep("[Station 2RL] Left/Right limits with Aorta");
  MaskImagePointer Aorta = GetAFDB()->template GetImage<MaskImageType>("Aorta");
  //  DD(GetFuzzyThreshold("2RL", "BrachioCephalicVein"));
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, Aorta, 2, 
                                                       GetFuzzyThreshold("2RL", "Aorta"),
                                                       "RightTo", false, 2, true, false);  
  // END
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;

  // ---------------------------------------------------------------------------
  StartNewStep("[Station 2RL] Left/Right limits with SubclavianArtery (Right)");
  
  // SliceBySliceRelativePosition + select CCL most at Right
  MaskImagePointer SubclavianArtery = GetAFDB()->template GetImage<MaskImageType>("SubclavianArtery");
  typedef SliceBySliceRelativePositionFilter<MaskImageType> SliceRelPosFilterType;
  typename SliceRelPosFilterType::Pointer sliceRelPosFilter = SliceRelPosFilterType::New();
  sliceRelPosFilter->VerboseStepFlagOff();
  sliceRelPosFilter->WriteStepFlagOff();
  sliceRelPosFilter->SetInput(m_Working_Support);
  sliceRelPosFilter->SetInputObject(SubclavianArtery);
  sliceRelPosFilter->SetDirection(2);
  sliceRelPosFilter->SetFuzzyThreshold(GetFuzzyThreshold("2RL", "SubclavianArteryRight"));
  sliceRelPosFilter->AddOrientationTypeString("NotRightTo");
  sliceRelPosFilter->IntermediateSpacingFlagOn();
  sliceRelPosFilter->SetIntermediateSpacing(2);
  sliceRelPosFilter->UniqueConnectedComponentBySliceOff();
  sliceRelPosFilter->UseASingleObjectConnectedComponentBySliceFlagOff();

  sliceRelPosFilter->CCLSelectionFlagOn(); // select one CCL by slice
  sliceRelPosFilter->SetCCLSelectionDimension(0); // select according to X (0) axis
  sliceRelPosFilter->SetCCLSelectionDirection(-1); // select most at Right
  sliceRelPosFilter->CCLSelectionIgnoreSingleCCLFlagOn(); // ignore if only one CCL

  sliceRelPosFilter->AutoCropFlagOn(); 
  sliceRelPosFilter->IgnoreEmptySliceObjectFlagOn();
  sliceRelPosFilter->RemoveObjectFlagOff();
  sliceRelPosFilter->Update();
  m_Working_Support = sliceRelPosFilter->GetOutput();

  // END
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;


  // ---------------------------------------------------------------------------
  StartNewStep("[Station 2RL] Left/Right limits with SubclavianArtery (Left)");
  
  // SliceBySliceRelativePosition + select CCL most at Right
   sliceRelPosFilter = SliceRelPosFilterType::New();
  sliceRelPosFilter->VerboseStepFlagOff();
  sliceRelPosFilter->WriteStepFlagOff();
  sliceRelPosFilter->SetInput(m_Working_Support);
  sliceRelPosFilter->SetInputObject(SubclavianArtery);
  sliceRelPosFilter->SetDirection(2);
  sliceRelPosFilter->SetFuzzyThreshold(GetFuzzyThreshold("2RL", "SubclavianArteryLeft"));
  sliceRelPosFilter->AddOrientationTypeString("NotLeftTo");
  sliceRelPosFilter->IntermediateSpacingFlagOn();
  sliceRelPosFilter->SetIntermediateSpacing(2);
  sliceRelPosFilter->UniqueConnectedComponentBySliceOff();
  sliceRelPosFilter->UseASingleObjectConnectedComponentBySliceFlagOff();

  sliceRelPosFilter->CCLSelectionFlagOn(); // select one CCL by slice
  sliceRelPosFilter->SetCCLSelectionDimension(0); // select according to X (0) axis
  sliceRelPosFilter->SetCCLSelectionDirection(+1); // select most at Left
  sliceRelPosFilter->CCLSelectionIgnoreSingleCCLFlagOff(); // do not ignore if only one CCL

  sliceRelPosFilter->AutoCropFlagOn(); 
  sliceRelPosFilter->IgnoreEmptySliceObjectFlagOn();
  sliceRelPosFilter->RemoveObjectFlagOff();
  sliceRelPosFilter->Update();
  m_Working_Support = sliceRelPosFilter->GetOutput();

  // END
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
void
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_Remove_Structures()
{
  Remove_Structures("2RL", "BrachioCephalicVein");
  Remove_Structures("2RL", "CommonCarotidArtery");
  Remove_Structures("2RL", "SubclavianArtery");
  Remove_Structures("2RL", "Thyroid");
  Remove_Structures("2RL", "Aorta");
  
  // END
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_SeparateRL()
{
  // ---------------------------------------------------------------------------
  StartNewStep("[Station 2RL] Separate 2R/2L according to Trachea");

  /*Rod says: 
    
   "For station 2 there is a shift, dividing 2R from 2L, from midline
   to the left paratracheal border."

   Algo: 
    - Consider Trachea SliceBySlice
    - find extrema at Left
    - add margins towards Right
    - remove what is at Left of this line
   */

  // Get Trachea
  MaskImagePointer Trachea = GetAFDB()->template GetImage<MaskImageType>("Trachea");
  
  // Resize like the current support (to have the same number of slices)
  Trachea = clitk::ResizeImageLike<MaskImageType>(Trachea, m_Working_Support, GetBackgroundValue());

  // Find extrema post positions
  std::vector<MaskImagePointType> tracheaLeftPositionsA;
  std::vector<MaskImagePointType> tracheaLeftPositionsB;
  clitk::SliceBySliceBuildLineSegmentAccordingToExtremaPosition<MaskImageType>(Trachea, 
                                                                               GetBackgroundValue(), 2, 
                                                                               0, false, // Left
                                                                               1, // Vertical line 
                                                                               1, // margins 
                                                                               tracheaLeftPositionsA, 
                                                                               tracheaLeftPositionsB);
  // Copy support for R and L
  typedef itk::ImageDuplicator<MaskImageType> DuplicatorType;
  DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(m_Working_Support);
  duplicator->Update();
  MaskImageType::Pointer m_Working_Support2 = duplicator->GetOutput();
  
  // Cut post to this line for Right part
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    tracheaLeftPositionsA,
                                                                    tracheaLeftPositionsB,
                                                                    GetBackgroundValue(), 0, -10); 
  writeImage<MaskImageType>(m_Working_Support, "R.mhd");

  // Cut post to this line for Left part
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support2, 
                                                                    tracheaLeftPositionsA,
                                                                    tracheaLeftPositionsB,
                                                                    GetBackgroundValue(), 0, +10); 
  writeImage<MaskImageType>(m_Working_Support2, "L.mhd");

  // END
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support2;
}
//--------------------------------------------------------------------
