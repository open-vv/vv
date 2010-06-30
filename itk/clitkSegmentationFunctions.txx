/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  ======================================================================-====*/

// clitk
#include "clitkSetBackgroundImageFilter.h"

// itk
#include <itkConnectedComponentImageFilter.h>
#include <itkRelabelComponentImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>

//--------------------------------------------------------------------
template<class TImageType, class TMaskImageType>
typename TImageType::Pointer
clitk::SetBackground(typename TImageType::ConstPointer input, 
                     typename TMaskImageType::ConstPointer mask, 
                     typename TMaskImageType::PixelType maskBG,
                     typename TImageType::PixelType outValue) {
  typedef clitk::SetBackgroundImageFilter<TImageType, TMaskImageType, TImageType> SetBackgroundImageFilterType;
  typename SetBackgroundImageFilterType::Pointer setBackgroundFilter = SetBackgroundImageFilterType::New();
  setBackgroundFilter->SetInput(input);
  setBackgroundFilter->SetInput2(mask);
  setBackgroundFilter->SetMaskValue(maskBG);
  setBackgroundFilter->SetOutsideValue(outValue);
  setBackgroundFilter->Update();
  return setBackgroundFilter->GetOutput();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class TImageType>
typename TImageType::Pointer
clitk::Labelize(typename TImageType::Pointer input, 
                typename TImageType::PixelType BG, 
                bool isFullyConnected, 
                int minimalComponentSize) {

  // Connected Component label 
  typedef itk::ConnectedComponentImageFilter<TImageType, TImageType> ConnectFilterType;
  typename ConnectFilterType::Pointer connectFilter = ConnectFilterType::New();
  connectFilter->SetInput(input);
  connectFilter->SetBackgroundValue(BG);
  connectFilter->SetFullyConnected(isFullyConnected);
  
  // Sort by size and remove too small area.
  typedef itk::RelabelComponentImageFilter<TImageType, TImageType> RelabelFilterType;
  typename RelabelFilterType::Pointer relabelFilter = RelabelFilterType::New();
  relabelFilter->InPlaceOn();
  relabelFilter->SetInput(connectFilter->GetOutput());
  relabelFilter->SetMinimumObjectSize(minimalComponentSize);
  relabelFilter->Update();

  // Return result
  return relabelFilter->GetOutput();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class TImageType>
typename TImageType::Pointer
clitk::RemoveLabels(typename TImageType::Pointer input, 
                    typename TImageType::PixelType BG,
                    std::vector<typename TImageType::PixelType> & labelsToRemove) {
  typename TImageType::Pointer working_image = input;
  for (unsigned int i=0; i <labelsToRemove.size(); i++) {
    typedef clitk::SetBackgroundImageFilter<TImageType, TImageType> SetBackgroundImageFilterType;
    typename SetBackgroundImageFilterType::Pointer setBackgroundFilter = SetBackgroundImageFilterType::New();
    setBackgroundFilter->SetInput(input);
    setBackgroundFilter->SetInput2(input);
    setBackgroundFilter->SetMaskValue(labelsToRemove[i]);
    setBackgroundFilter->SetOutsideValue(BG);
    setBackgroundFilter->Update();
    working_image = setBackgroundFilter->GetOutput();
  }
  return working_image;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class TImageType>
typename TImageType::Pointer
clitk::KeepLabels(typename TImageType::Pointer input, 
                  typename TImageType::PixelType BG, 
                  typename TImageType::PixelType FG, 
                  typename TImageType::PixelType firstKeep, 
                  typename TImageType::PixelType lastKeep, 
                  bool useLastKeep) {
  typedef itk::BinaryThresholdImageFilter<TImageType, TImageType> BinarizeFilterType; 
  typename BinarizeFilterType::Pointer binarizeFilter = BinarizeFilterType::New();
  binarizeFilter->SetInput(input);
  binarizeFilter->SetLowerThreshold(firstKeep);
  if (useLastKeep) binarizeFilter->SetUpperThreshold(lastKeep);
  binarizeFilter->SetInsideValue(FG);
  binarizeFilter->SetOutsideValue(BG);
  binarizeFilter->Update();
  return binarizeFilter->GetOutput();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class TImageType>
typename TImageType::Pointer
clitk::LabelizeAndSelectLabels(typename TImageType::Pointer input,
                               typename TImageType::PixelType BG, 
                               typename TImageType::PixelType FG, 
                               bool isFullyConnected,
                               int minimalComponentSize,
                               LabelizeParameters<typename TImageType::PixelType> * param)
{
  typename TImageType::Pointer working_image;
  working_image = Labelize<TImageType>(input, BG, isFullyConnected, minimalComponentSize);
  working_image = RemoveLabels<TImageType>(working_image, BG, param->GetLabelsToRemove());
  working_image = KeepLabels<TImageType>(working_image, 
                                         BG, FG, 
                                         param->GetFirstKeep(), 
                                         param->GetLastKeep(), 
                                         param->GetUseLastKeep());
  return working_image;
}
//--------------------------------------------------------------------
