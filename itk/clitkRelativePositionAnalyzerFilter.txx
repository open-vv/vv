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
  itk::ImageToImageFilter<ImageType, ImageType>()
{
  this->SetNumberOfRequiredInputs(3); // Input : support, object, target
  SetBackgroundValue(0);
  SetForegroundValue(1);
  SetNumberOfBins(100);
  SetAreaLossTolerance(0.01);
  SetSupportSize(0);
  SetTargetSize(0);
  SetSizeWithThreshold(0);
  SetSizeWithReverseThreshold(0);
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
GenerateData() 
{
  ImagePointer temp = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  m_Object = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(1));
  m_Target = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(2));

  // Remove object from support (keep initial image)
  m_Support = clitk::Clone<ImageType>(temp);
  clitk::AndNot<ImageType>(m_Support, m_Object, GetBackgroundValue());
  
  // Define filter to compute statics on mask image
  typedef itk::LabelStatisticsImageFilter<ImageType, ImageType> StatFilterType;
  typename StatFilterType::Pointer statFilter = StatFilterType::New();

  // Compute the initial support size
  statFilter->SetInput(m_Support);
  statFilter->SetLabelInput(m_Support);
  statFilter->Update();
  SetSupportSize(statFilter->GetCount(GetForegroundValue()));
  // DD(GetSupportSize());
  
  // Compute the initial target size
  ImagePointer s = clitk::ResizeImageLike<ImageType>(m_Support, m_Target, GetBackgroundValue());
  statFilter->SetInput(s);
  statFilter->SetLabelInput(m_Target);
  statFilter->Update();
  SetTargetSize(statFilter->GetCount(GetForegroundValue()));
  // DD(GetTargetSize());

  //
  int bins = GetNumberOfBins();
  double tolerance = GetAreaLossTolerance();

  // Compute Fuzzy map
  double angle = GetDirection().angle1;
  typename FloatImageType::Pointer map = ComputeFuzzyMap(m_Object, m_Target, m_Support, angle);
  writeImage<FloatImageType>(map, "fuzzy_"+toString(clitk::rad2deg(angle))+".mha");

  // Compute the optimal thresholds (direct and inverse)
  double mThreshold=0.0;
  double mReverseThreshold=1.0;
  ComputeOptimalThresholds(map, m_Target, bins, tolerance, mThreshold, mReverseThreshold);

  // Use the threshold to compute new support
  int s1 = GetSupportSize();
  if (mThreshold > 0.0) {
    ImagePointer support1 = 
      clitk::SliceBySliceRelativePosition<ImageType>(m_Support, m_Object, 2, 
                                                     mThreshold,
                                                     angle,false, // inverseFlag
                                                     false,  // uniqueConnectedComponent
                                                     -1, true, 
                                                     false);//singleObjectCCL
    // Compute the new support size
    statFilter->SetInput(support1);
    statFilter->SetLabelInput(support1);
    statFilter->Update();
    s1 = statFilter->GetCount(GetForegroundValue());
  }
  
  int s2 = GetSupportSize();
  if (mReverseThreshold < 1.0) {
    ImagePointer support2 = 
      clitk::SliceBySliceRelativePosition<ImageType>(m_Support, m_Object, 2, 
                                                     mReverseThreshold, 
                                                     angle,true,// inverseFlag
                                                     false, // uniqueConnectedComponent
                                                     -1, true, 
                                                     false); //singleObjectCCL
    // Compute the new support size
    statFilter = StatFilterType::New();
    statFilter->SetInput(support2);
    statFilter->SetLabelInput(support2);
    statFilter->Update();
    s2 = statFilter->GetCount(GetForegroundValue());
  }
  
  // Set results values
  m_Info.threshold = mThreshold;
  m_Info.sizeAfterThreshold = s1;
  m_Info.sizeBeforeThreshold = GetSupportSize();
  m_Info.sizeReference = GetTargetSize();
  m_InfoReverse.threshold = mReverseThreshold;
  m_InfoReverse.sizeAfterThreshold = s2;
  m_InfoReverse.sizeBeforeThreshold = GetSupportSize();
  m_InfoReverse.sizeReference = GetTargetSize();  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
typename clitk::RelativePositionAnalyzerFilter<ImageType>::FloatImageType::Pointer
clitk::RelativePositionAnalyzerFilter<ImageType>::
ComputeFuzzyMap(ImageType * object, ImageType * target, ImageType * support, double angle)
{
  typedef clitk::SliceBySliceRelativePositionFilter<ImageType> SliceRelPosFilterType;
  typedef typename SliceRelPosFilterType::FloatImageType FloatImageType;
  typename SliceRelPosFilterType::Pointer sliceRelPosFilter = SliceRelPosFilterType::New();
  sliceRelPosFilter->VerboseStepFlagOff();
  sliceRelPosFilter->WriteStepFlagOff();
  sliceRelPosFilter->SetInput(support);
  sliceRelPosFilter->SetInputObject(object);
  sliceRelPosFilter->SetDirection(2);
  sliceRelPosFilter->SetIntermediateSpacingFlag(false);
  //sliceRelPosFilter->AddOrientationTypeString(orientation);
  sliceRelPosFilter->AddAnglesInRad(angle, 0.0);
  sliceRelPosFilter->FuzzyMapOnlyFlagOn(); // do not threshold, only compute the fuzzy map
  // sliceRelPosFilter->PrintOptions();
  sliceRelPosFilter->Update();
  typename FloatImageType::Pointer map = sliceRelPosFilter->GetFuzzyMap();

  // Resize map like object to allow SetBackground
  map = clitk::ResizeImageLike<FloatImageType>(map, object, GetBackgroundValue());
  
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
      if (j==0) threshold = h->GetBinMin(0,j);
      else threshold = h->GetBinMin(0,j-1); // the last before reaching the threshold
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
      if (j==bins-1) reverseThreshold = h->GetBinMax(0,j);
      else reverseThreshold = h->GetBinMax(0,j+1);
      found = true;
    }
  }
}
//--------------------------------------------------------------------

