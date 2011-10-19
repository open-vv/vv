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
  this->SetNumberOfRequiredInputs(3); // support, object, target
  VerboseFlagOff();
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
  this->SetNthInput(1, const_cast<ImageType *>(image));
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
GenerateData() 
{
  DD("Update");
  // Get input pointer
  m_Support = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  m_Object = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(1));
  m_Target = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(2));
  static const unsigned int dim = ImageType::ImageDimension;

  /*
    Prerequisite:
    - target fully inside support ? 
  */
  
  //for(int i=0; i<m_ListOfOrientation.size(); i++) {
  //DD(i);
    
    // Compute Fuzzy map

    // Get minimal value in the target area

    // Or -> analyze floating point values inside the target area (histo ?)

    // Set threshold and compute new support
    
    // Compute ratio of area before/after
    // http://www.itk.org/Doxygen/html/classitk_1_1LabelStatisticsImageFilter.html#details
    /*
    typedef itk::LabelStatisticsImageFilter<ImageType, ImageType> StatisticsImageFilterType;
    typename StatisticsImageFilterType::Pointer statisticsFilter = StatisticsImageFilterType::New();
    statisticsFilter->SetInput(m_Input);
    statisticsFilter->SetLabelInput(m_Input);
    statisticsFilter->Update();
    int n = labelStatisticsImageFilter->GetCount(GetForegroundValue());
    DD(n);
    statisticsFilter = StatisticsImageFilterType::New();
    statisticsFilter->SetInput(m_Output);
    statisticsFilter->SetLabelInput(m_Output);
    statisticsFilter->Update();
    int m = labelStatisticsImageFilter->GetCount(GetForegroundValue());
    DD(m);
    */
    
    // Print results
    
  //}

  
  // Final Step -> set output TODO
  // this->SetNthOutput(0, working_image);
  //  this->GraftOutput(working_image);
}
//--------------------------------------------------------------------

