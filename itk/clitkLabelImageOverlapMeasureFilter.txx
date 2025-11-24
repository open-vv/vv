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
clitk::LabelImageOverlapMeasureFilter<ImageType>::
LabelImageOverlapMeasureFilter():
  clitk::FilterBase(),
  itk::ImageToImageFilter<ImageType, ImageType>()
{
  this->SetNumberOfRequiredInputs(2);
  SetLabelFlag(false);
  m_BackgroundValue = 0;
  SetVerboseFlag(false);
  SetLongFlag(false);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void
clitk::LabelImageOverlapMeasureFilter<ImageType>::
GenerateOutputInformation()
{
  // DD("GenerateOutputInformation");
  //ImagePointer input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  // ImagePointer outputImage = this->GetOutput(0);
  // outputImage->SetRegions(outputImage->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void
clitk::LabelImageOverlapMeasureFilter<ImageType>::
GenerateInputRequestedRegion()
{
  // Call default
  itk::ImageToImageFilter<ImageType, ImageType>::GenerateInputRequestedRegion();
  // Get input pointers and set requested region to common region
  ImagePointer input1 = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  ImagePointer input2 = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(1));
  input1->SetRequestedRegion(input1->GetLargestPossibleRegion());
  input2->SetRequestedRegion(input2->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
void
clitk::LabelImageOverlapMeasureFilter<ImageType>::
GenerateData()
{
  // Get input pointer
  m_Input1 = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  m_Input2 = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(1));
  static const unsigned int dim = ImageType::ImageDimension;

  // Compute union of bounding boxes
  typedef itk::BoundingBox<unsigned long, dim> BBType;
  typename BBType::Pointer bb1 = BBType::New();
  ComputeBBFromImageRegion<ImageType>(m_Input1, m_Input1->GetLargestPossibleRegion(), bb1);
  typename BBType::Pointer bb2 = BBType::New();
  ComputeBBFromImageRegion<ImageType>(m_Input2, m_Input2->GetLargestPossibleRegion(), bb2);
  typename BBType::Pointer bbo = BBType::New();
  ComputeBBUnion<dim>(bbo, bb1, bb2);

  // Resize like the union
  ImagePointer input1 = clitk::ResizeImageLike<ImageType>(m_Input1, bbo, GetBackgroundValue());
  ImagePointer input2 = clitk::ResizeImageLike<ImageType>(m_Input2, bbo, GetBackgroundValue());
  /*
    if (GetVerboseFlag()) {
    std::cout << "Resize images to the union of bounding boxes: "
    <<  input1->GetLargestPossibleRegion().GetSize() << std::endl;
    }
  */

  /*
  int width = 6;
  std::cout << std::setw(width) << in1 << " "
            << std::setw(width) << in2 << " "
            << std::setw(width) << inter  << " "
            << std::setw(width) << u  << " "
            << std::setw(width) << l1notIn2  << " "
            << std::setw(width) << l2notIn1  << " "
            << std::setw(width) << dice << " "; //std::endl;
  */

  // Compute overlap, dice
  typedef itk::LabelOverlapMeasuresImageFilter<ImageType> FilterType;
  typename FilterType::Pointer diceFilter = FilterType::New();
  diceFilter->SetSourceImage(input1);
  diceFilter->SetTargetImage(input2);
  diceFilter->Update();

  // Display information
  const int prec = 5;
  if (!GetLongFlag()) {
    if (GetVerboseFlag()) {
      std::cout << "Dice" << std::endl;
    }
    if (GetLabelFlag()) {
        std::cout << std::fixed << setprecision(prec) << diceFilter->GetDiceCoefficient(m_Label) << std::endl;
    } else {
        std::cout << std::fixed << setprecision(prec) << diceFilter->GetDiceCoefficient() << std::endl;
    }
  }
  else { // long option
    const int width = 10;
    if (GetVerboseFlag()) {
      std::cout << "Dice"
                << std::setw(13) << "Jaccard"
                << std::setw(11) << "Inter/#T"
                << std::setw(6) <<  "FNeg"
                << std::setw(10) << "FPos"
                << std::setw(22) << "2(#S-#T)/(#S+#T)"
                << std::endl;
    }
    if (GetLabelFlag()) {
        std::cout << std::fixed << setprecision(prec) << diceFilter->GetDiceCoefficient(m_Label)
                  << std::setw(width) << setprecision(prec) << diceFilter->GetJaccardCoefficient(m_Label)
                  << std::setw(width) << setprecision(prec) << diceFilter->GetTargetOverlap(m_Label)
                  << std::setw(width) << setprecision(prec) << diceFilter->GetFalseNegativeError(m_Label)
                  << std::setw(width) << setprecision(prec) << diceFilter->GetFalsePositiveError(m_Label)
                  << std::setw(width) << setprecision(prec) << diceFilter->GetVolumeSimilarity(m_Label)
                  << std::endl;
    } else {
        std::cout << std::fixed << setprecision(prec) << diceFilter->GetDiceCoefficient()
                  << std::setw(width) << setprecision(prec) << diceFilter->GetJaccardCoefficient()
                  << std::setw(width) << setprecision(prec) << diceFilter->GetTotalOverlap()
                  << std::setw(width) << setprecision(prec) << diceFilter->GetFalseNegativeError()
                  << std::setw(width) << setprecision(prec) << diceFilter->GetFalsePositiveError()
                  << std::setw(width) << setprecision(prec) << diceFilter->GetVolumeSimilarity()
                  << std::endl;
    }
  }

}
//--------------------------------------------------------------------
