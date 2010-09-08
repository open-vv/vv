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

#ifndef CLITKEXTRACTAIRWAYTREEINFOSFILTER_TXX
#define CLITKEXTRACTAIRWAYTREEINFOSFILTER_TXX

// clitk
#include "clitkImageCommon.h"
#include "clitkSetBackgroundImageFilter.h"
#include "clitkSegmentationUtils.h"
#include "clitkAutoCropFilter.h"

// itk
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkOtsuThresholdImageFilter.h"
#include "itkBinaryThinningImageFilter3D.h"
#include "itkImageIteratorWithIndex.h"


//--------------------------------------------------------------------
template <class ImageType>
clitk::ExtractAirwayTreeInfoFilter<ImageType>::
ExtractAirwayTreeInfoFilter():
  clitk::FilterBase(),
  itk::ImageToImageFilter<ImageType, ImageType>()
{
  // Default global options
  this->SetNumberOfRequiredInputs(1);
  SetBackgroundValue(0);
  SetForegroundValue(1);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractAirwayTreeInfoFilter<ImageType>::
SetInput(const ImageType * image) 
{
  this->SetNthInput(0, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
template<class ArgsInfoType>
void 
clitk::ExtractAirwayTreeInfoFilter<ImageType>::
SetArgsInfo(ArgsInfoType mArgsInfo)
{
  SetVerboseOption_GGO(mArgsInfo);
  SetVerboseStep_GGO(mArgsInfo);
  SetWriteStep_GGO(mArgsInfo);
  SetVerboseWarningOff_GGO(mArgsInfo);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractAirwayTreeInfoFilter<ImageType>::
GenerateOutputInformation() 
{ 
  Superclass::GenerateOutputInformation();
  //this->GetOutput(0)->SetRequestedRegion(this->GetOutput(0)->GetLargestPossibleRegion());

  // Get input pointer
  input   = dynamic_cast<const ImageType*>(itk::ProcessObject::GetInput(0));

  StartNewStepOrStop("Find bronchial bifurcations");
  // Step 1 : extract skeleton
  // Define the thinning filter
  typedef itk::BinaryThinningImageFilter3D<ImageType, ImageType> ThinningFilterType;
  typename ThinningFilterType::Pointer thinningFilter = ThinningFilterType::New();
  thinningFilter->SetInput(input);
  thinningFilter->Update();
  skeleton = thinningFilter->GetOutput();
  writeImage<ImageType>(skeleton, "skeleton.mhd");
  
  // Step 2 : tracking
  DD("tracking");
  
  // Step 2.1 : find first point for tracking
  typedef itk::ImageRegionConstIteratorWithIndex<ImageType> IteratorType;
  IteratorType it(skeleton, skeleton->GetLargestPossibleRegion());
  it.GoToReverseBegin();
  while ((!it.IsAtEnd()) && (it.Get() == GetBackgroundValue())) { 
    --it;
  }
  if (it.IsAtEnd()) {
    this->SetLastError("ERROR: first point in the skeleton not found ! Abort");
    return;
  }
  DD(skeleton->GetLargestPossibleRegion().GetIndex());
  typename ImageType::IndexType index = it.GetIndex();
  skeleton->TransformIndexToPhysicalPoint(index, m_FirstTracheaPoint);
  DD(index);
  DD(m_FirstTracheaPoint);
    
  // Step 2.2 : initialize neighborhooditerator
  typedef itk::NeighborhoodIterator<ImageType> NeighborhoodIteratorType;
  typename NeighborhoodIteratorType::SizeType radius;
  radius.Fill(1);
  NeighborhoodIteratorType nit(radius, skeleton, skeleton->GetLargestPossibleRegion());
  DD(nit.GetSize());
  DD(nit.Size());
    
  // Find first label number (must be different from BG and FG)
  typename ImageType::PixelType label = GetForegroundValue()+1;
  while ((label == GetBackgroundValue()) || (label == GetForegroundValue())) { label++; }
  DD(label);

  // Track from the first point
  std::vector<BifurcationType> listOfBifurcations;
  TrackFromThisIndex(listOfBifurcations, skeleton, index, label);
  DD("end track");
  DD(listOfBifurcations.size());
  writeImage<ImageType>(skeleton, "skeleton2.mhd");

  for(unsigned int i=0; i<listOfBifurcations.size(); i++) {
    typename ImageType::PointType p;
    skeleton->TransformIndexToPhysicalPoint(listOfBifurcations[i].index, p);
    DD(p);
  }

}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractAirwayTreeInfoFilter<ImageType>::
GenerateData() 
{
  // Do not put some "startnewstep" here, because the object if
  // modified and the filter's pipeline it do two times. But it is
  // required to quit if MustStop was set before.
  if (GetMustStop()) return;
  
  // If everything goes well, set the output
  this->GraftOutput(skeleton); // not SetNthOutput
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractAirwayTreeInfoFilter<ImageType>::
TrackFromThisIndex(std::vector<BifurcationType> & listOfBifurcations, 
                   ImagePointer skeleton, 
                   ImageIndexType index,
                   ImagePixelType label) 
{
  DD("TrackFromThisIndex");
  DD(index);
  DD((int)label);
  // Create NeighborhoodIterator 
  typedef itk::NeighborhoodIterator<ImageType> NeighborhoodIteratorType;
  typename NeighborhoodIteratorType::SizeType radius;
  radius.Fill(1);
  NeighborhoodIteratorType nit(radius, skeleton, skeleton->GetLargestPossibleRegion());
      
  // Track
  std::vector<typename NeighborhoodIteratorType::IndexType> listOfTrackedPoint;
  bool stop = false;
  while (!stop) {
    nit.SetLocation(index);
    // DD((int)nit.GetCenterPixel());
    nit.SetCenterPixel(label);
    listOfTrackedPoint.clear();
    for(unsigned int i=0; i<nit.Size(); i++) {
      if (i != nit.GetCenterNeighborhoodIndex ()) { // Do not observe the current point
        //          DD(nit.GetIndex(i));
        if (nit.GetPixel(i) == GetForegroundValue()) { // if this is foreground, we continue the tracking
          // DD(nit.GetIndex(i));
          listOfTrackedPoint.push_back(nit.GetIndex(i));
        }
      }
    }
    // DD(listOfTrackedPoint.size());
    if (listOfTrackedPoint.size() == 1) {
      index = listOfTrackedPoint[0];
    }
    else {
      if (listOfTrackedPoint.size() == 2) {
        BifurcationType bif(index, label, label+1, label+2);
        listOfBifurcations.push_back(bif);
        TrackFromThisIndex(listOfBifurcations, skeleton, listOfTrackedPoint[0], label+1);
        TrackFromThisIndex(listOfBifurcations, skeleton, listOfTrackedPoint[1], label+2);
      }
      else {
        if (listOfTrackedPoint.size() > 2) {
          std::cerr << "too much bifurcation points ... ?" << std::endl;
          exit(0);
        }
        // Else this it the end of the tracking
      }
      stop = true;
    }
  }
}
//--------------------------------------------------------------------


#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
