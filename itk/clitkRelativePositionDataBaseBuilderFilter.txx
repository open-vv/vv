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
clitk::RelativePositionDataBaseBuilderFilter<ImageType>::
RelativePositionDataBaseBuilderFilter():
  clitk::FilterBase(),
  clitk::FilterWithAnatomicalFeatureDatabaseManagement(),
  itk::ImageToImageFilter<ImageType, ImageType>()
{
  this->SetNumberOfRequiredInputs(0); // support
  VerboseFlagOff();
  SetBackgroundValue(0);
  SetForegroundValue(1);
  SetNumberOfBins(100);
  SetNumberOfAngles(4);
  SetAreaLossTolerance(0.01);
  m_ListOfAngles.clear();
  // SetSupportSize(0);
  // SetTargetSize(0);
  // SetSizeWithThreshold(0);
  // SetSizeWithReverseThreshold(0);  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::RelativePositionDataBaseBuilderFilter<ImageType>::
PrintOptions() 
{
  DD("TODO");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::RelativePositionDataBaseBuilderFilter<ImageType>::
GenerateOutputInformation() 
{ 
  // ImagePointer input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  // ImagePointer outputImage = this->GetOutput(0);
  // outputImage->SetRegions(outputImage->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::RelativePositionDataBaseBuilderFilter<ImageType>::
GenerateInputRequestedRegion() 
{
  // Call default
  // itk::ImageToImageFilter<ImageType, ImageType>::GenerateInputRequestedRegion();
  // // Get input pointers and set requested region to common region
  // ImagePointer input1 = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  // input1->SetRequestedRegion(input1->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::RelativePositionDataBaseBuilderFilter<ImageType>::
GenerateData() 
{
  // Load database of anatomical elements
  this->LoadAFDB();

  // Get some information
  std::string patient = this->GetAFDB()->GetTagValue("PatientID");

  // Get input pointers
  m_Support = this->GetAFDB()->template GetImage <ImageType>(GetSupportName());
  m_Target = this->GetAFDB()->template GetImage <ImageType>(GetTargetName());

  // Build the list of tested directions
  m_ListOfAngles.clear();
  for(int i=0; i<GetNumberOfAngles(); i++) {
    double a = i*360.0/GetNumberOfAngles();
    if (a>180) a = 180-a;
    m_ListOfAngles.push_back(clitk::deg2rad(a));
    RelativePositionDirectionType r;
    r.angle1 = clitk::deg2rad(a);
    r.angle2 = 0;
    r.notFlag = false;
    m_ListOfDirections.push_back(r); // only one direction
  }


  // Perform the RelativePositionAnalyzerFilter for each objects
  typedef typename clitk::RelativePositionAnalyzerFilter<ImageType> FilterType;
  for (int i=0; i<GetNumberOfObjects(); i++) {
    m_Object = this->GetAFDB()->template GetImage <ImageType>(GetObjectName(i));

    for (unsigned int j=0; j<m_ListOfDirections.size(); j++) {
      clitk::RelativePositionDirectionType direction = m_ListOfDirections[j];
      
      // Create the filter
      typename FilterType::Pointer filter = FilterType::New();
      filter->SetInputSupport(m_Support);
      filter->SetInputTarget(m_Target);
      filter->SetInputObject(m_Object); // FIXME do AndNot before + only compute supportSize once.
      filter->SetNumberOfBins(GetNumberOfBins());
      filter->SetAreaLossTolerance(GetAreaLossTolerance());
      filter->SetDirection(direction);
      filter->Update();

      // Results
      std::ostringstream s;
      s << patient << " " 
        << GetSupportName() << " " 
        // << GetTargetName() << " " // No need
        << GetObjectName(i) <<" ";
      // Normal 
      // if (filter->GetInfo().sizeAfterThreshold != filter->GetInfo().sizeBeforeThreshold) {
        std::ostringstream os;
        os << s.str();
        direction.notFlag = false;
        direction.Print(os);
        filter->GetInfo().Print(os);
        std::cout << os.str() << std::endl;
      // }
      // Inverse
      // if (filter->GetInfoReverse().sizeAfterThreshold != filter->GetInfoReverse().sizeBeforeThreshold) {
        std::ostringstream oos;
        oos << s.str();
        direction.notFlag = true;
        direction.Print(oos);
        filter->GetInfoReverse().Print(oos);
        std::cout << oos.str() << std::endl;
      // }
    } // end direction

  } // end object
}
//--------------------------------------------------------------------


