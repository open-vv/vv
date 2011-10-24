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
template <class ImageType>
clitk::RelativePositionAnalyzerFilter<ImageType>::
RelativePositionAnalyzerFilter():
  clitk::FilterBase(),
  clitk::FilterWithAnatomicalFeatureDatabaseManagement(),
  itk::ImageToImageFilter<ImageType, ImageType>()
{
  this->SetNumberOfRequiredInputs(3); // support, object, target
  VerboseFlagOff();
  SetBackgroundValue(0);
  SetForegroundValue(1);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::RelativePositionAnalyzerFilter<ImageType>::
SetInputSupport(const ImageType * image) 
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(0, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::RelativePositionAnalyzerFilter<ImageType>::
SetInputObject(const ImageType * image) 
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(1, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::RelativePositionAnalyzerFilter<ImageType>::
SetInputTarget(const ImageType * image) 
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(2, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::RelativePositionAnalyzerFilter<ImageType>::
PrintOptions() 
{
  DD("TODO");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::RelativePositionAnalyzerFilter<ImageType>::
GenerateOutputInformation() 
{ 
  ImagePointer input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  ImagePointer outputImage = this->GetOutput(0);
  outputImage->SetRegions(outputImage->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::RelativePositionAnalyzerFilter<ImageType>::
GenerateInputRequestedRegion() 
{
  // Call default
  itk::ImageToImageFilter<ImageType, ImageType>::GenerateInputRequestedRegion();
  // Get input pointers and set requested region to common region
  ImagePointer input1 = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  ImagePointer input2 = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(1));
  ImagePointer input3 = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(2));
  input1->SetRequestedRegion(input1->GetLargestPossibleRegion());
  input2->SetRequestedRegion(input2->GetLargestPossibleRegion());
  input3->SetRequestedRegion(input3->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::RelativePositionAnalyzerFilter<ImageType>::
GenerateData() 
{
  this->LoadAFDB();

  // Get input pointer
  m_Support = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  m_Object = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(1));
  m_Target = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(2));
  static const unsigned int dim = ImageType::ImageDimension;

  // Remove object from support
  clitk::AndNot<ImageType>(m_Support, m_Object, GetBackgroundValue());
  
  // Resize object like target (to enable substraction later)
  ImagePointer objectLikeTarget = clitk::ResizeImageLike<ImageType>(m_Object, m_Target, GetBackgroundValue());

  // Define filter to compute statics on mask image
  typedef itk::LabelStatisticsImageFilter<ImageType, ImageType> StatFilterType;
  typename StatFilterType::Pointer statFilter = StatFilterType::New();

  // Compute the initial support size
  statFilter->SetInput(m_Support);
  statFilter->SetLabelInput(m_Support);
  statFilter->Update();
  int m_SupportSize = statFilter->GetCount(GetForegroundValue());
  // DD(m_SupportSize);
  
  // Compute the initial target size
  ImagePointer s = clitk::ResizeImageLike<ImageType>(m_Support, m_Target, GetBackgroundValue());
  statFilter->SetInput(s);
  statFilter->SetLabelInput(m_Target);
  statFilter->Update();
  int m_TargetSize = statFilter->GetCount(GetForegroundValue());
  // DD(m_TargetSize);

  // Build the list of tested orientations
  std::vector<double> m_ListOfAngles;
  int m_NumberOfAngles = this->GetAFDB()->GetDouble("NumberOfAngles");
  for(uint i=0; i<m_NumberOfAngles; i++) {
    double a = i*360.0/m_NumberOfAngles;
    if (a>180) a = 180-a;
    m_ListOfAngles.push_back(clitk::deg2rad(a));
  }

  // Loop on all orientations
  for(int i=0; i<m_ListOfAngles.size(); i++) {
    
    // Compute Fuzzy map
    typename FloatImageType::Pointer map = ComputeFuzzyMap(objectLikeTarget, m_Target, m_ListOfAngles[i]);
    writeImage<FloatImageType>(map, "fuzzy_"+toString(i)+".mha");

    // Compute the optimal thresholds (direct and inverse)
    double mThreshold;
    double mReverseThreshold;
    int bins = this->GetAFDB()->GetDouble("bins");
    double tolerance = this->GetAFDB()->GetDouble("TargetAreaLossTolerance");
    ComputeOptimalThresholds(map, m_Target, bins, tolerance, mThreshold, mReverseThreshold);

    // Use the threshold to compute new support
    int s1;
    if (mThreshold > 0.0) {
      ImagePointer support1 = 
        clitk::SliceBySliceRelativePosition<ImageType>(m_Support, m_Object, 2, 
                                                       mThreshold,
                                                       m_ListOfAngles[i],false,
                                                       false, -1, true, false);
      writeImage<ImageType>(support1, "sup_"+toString(i)+".mha");
      // Compute the new support size
      statFilter->SetInput(support1);
      statFilter->SetLabelInput(support1);
      statFilter->Update();
      s1 = statFilter->GetCount(GetForegroundValue());
    }
    else s1 = m_SupportSize;
      
    int s2;
    if (mReverseThreshold < 1.0) {
      ImagePointer support2 = 
        clitk::SliceBySliceRelativePosition<ImageType>(m_Support, m_Object, 2, 
                                                       mReverseThreshold, 
                                                       m_ListOfAngles[i],true,
                                                       false, -1, true, false);
      writeImage<ImageType>(support2, "sup_rev_"+toString(i)+".mha");
      // Compute the new support size
      statFilter = StatFilterType::New();
      statFilter->SetInput(support2);
      statFilter->SetLabelInput(support2);
      statFilter->Update();
      s2 = statFilter->GetCount(GetForegroundValue());
    }
    else s2 =m_SupportSize;
    
    // Print results
    std::cout << i << " " << clitk::rad2deg(m_ListOfAngles[i]) << "\t" 
              << m_SupportSize << " " << m_TargetSize << "\t"
              << s1/(double)m_SupportSize << " " << s2/(double)m_SupportSize << "\t" 
              << mThreshold << " " << mReverseThreshold << std::endl;
    
  } // end loop on orientations

  
  // Final Step -> set output TODO
  // this->SetNthOutput(0, working_image);
  //  this->GraftOutput(working_image);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
typename clitk::RelativePositionAnalyzerFilter<ImageType>::FloatImageType::Pointer
clitk::RelativePositionAnalyzerFilter<ImageType>::
ComputeFuzzyMap(ImageType * object, ImageType * target, double angle)
{
  typedef clitk::SliceBySliceRelativePositionFilter<ImageType> SliceRelPosFilterType;
  typedef typename SliceRelPosFilterType::FloatImageType FloatImageType;
  typename SliceRelPosFilterType::Pointer sliceRelPosFilter = SliceRelPosFilterType::New();
  sliceRelPosFilter->VerboseStepFlagOff();
  sliceRelPosFilter->WriteStepFlagOff();
  sliceRelPosFilter->SetInput(target);
  sliceRelPosFilter->SetInputObject(object);
  sliceRelPosFilter->SetDirection(2);
  sliceRelPosFilter->SetIntermediateSpacingFlag(false);
  //sliceRelPosFilter->AddOrientationTypeString(orientation);
  sliceRelPosFilter->AddAngles(angle, 0.0);
  sliceRelPosFilter->FuzzyMapOnlyFlagOn(); // do not threshold, only compute the fuzzy map
  // sliceRelPosFilter->PrintOptions();
  sliceRelPosFilter->Update();
  typename FloatImageType::Pointer map = sliceRelPosFilter->GetFuzzyMap();

  // Remove initial object from the fuzzy map
  map = clitk::SetBackground<FloatImageType, ImageType>(map, object, GetForegroundValue(), 0.0, true);
  
  // Resize the fuzzy map like the target, put 2.0 when outside
  map = clitk::ResizeImageLike<FloatImageType>(map, target, 2.0);  // Put 2.0 when out of initial map
  
  // end
  return map;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void
clitk::RelativePositionAnalyzerFilter<ImageType>::
ComputeOptimalThresholds(FloatImageType * map, ImageType * target, int bins, double tolerance, 
                         double & threshold, double & reverseThreshold)
{
  // Get the histogram of fuzzy values inside the target image
  typedef itk::LabelStatisticsImageFilter<FloatImageType, ImageType> FloatStatFilterType;
  typename FloatStatFilterType::Pointer f = FloatStatFilterType::New();
  f->SetInput(map);
  f->SetLabelInput(target);
  f->UseHistogramsOn();
  f->SetHistogramParameters(bins, 0.0, 1.1);
  f->Update();
  int count = f->GetCount(GetForegroundValue());
  typename FloatStatFilterType::HistogramPointer h = f->GetHistogram(GetForegroundValue());

  // Debug : dump histogram
  static int i=0;
  std::ofstream histogramFile(std::string("fuzzy_histo_"+toString(i)+".txt").c_str());
  for(int j=0; j<bins; j++) {
    histogramFile << h->GetMeasurement(j,0) 
                  << "\t" << h->GetFrequency(j) 
                  << "\t" << (double)h->GetFrequency(j)/(double)count << std::endl;
  }
  histogramFile.close();  
  i++;

  // Analyze the histogram (direct)
  double sum = 0.0;
  bool found = false;
  threshold = 0.0;
  for(int j=0; j<bins; j++) {
    sum += ((double)h->GetFrequency(j)/(double)count);
    if ((!found) && (sum > tolerance)) {
      threshold = h->GetBinMin(0,j);
      found = true;
    }
  }

  // Analyze the histogram (reverse)
  sum = 0.0;
  found = false;
  reverseThreshold = 1.0;
  for(int j=bins-1; j>=0; j--) {
    sum += ((double)h->GetFrequency(j)/(double)count);
    if ((!found) && (sum > tolerance)) {
      reverseThreshold = h->GetBinMax(0,j);
      found = true;
    }
  }
}
//--------------------------------------------------------------------

