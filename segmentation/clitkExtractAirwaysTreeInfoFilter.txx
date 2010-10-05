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

#ifndef CLITKEXTRACTAIRWAYSTREEINFOSFILTER_TXX
#define CLITKEXTRACTAIRWAYSTREEINFOSFILTER_TXX

// clitk
#include "clitkImageCommon.h"
#include "clitkSetBackgroundImageFilter.h"
#include "clitkSegmentationUtils.h"
#include "clitkAutoCropFilter.h"
#include "clitkExtractSliceFilter.h"

// itk
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkOtsuThresholdImageFilter.h"
#include "itkBinaryThinningImageFilter3D.h"
#include "itkImageIteratorWithIndex.h"


//--------------------------------------------------------------------
template <class ImageType>
clitk::ExtractAirwaysTreeInfoFilter<ImageType>::
ExtractAirwaysTreeInfoFilter():
  clitk::FilterBase(),
  clitk::FilterWithAnatomicalFeatureDatabaseManagement(),
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
clitk::ExtractAirwaysTreeInfoFilter<ImageType>::
SetInput(const ImageType * image) 
{
  this->SetNthInput(0, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
template<class ArgsInfoType>
void 
clitk::ExtractAirwaysTreeInfoFilter<ImageType>::
SetArgsInfo(ArgsInfoType mArgsInfo)
{
  SetVerboseOption_GGO(mArgsInfo);
  SetVerboseStep_GGO(mArgsInfo);
  SetWriteStep_GGO(mArgsInfo);
  SetVerboseWarningOff_GGO(mArgsInfo);
  SetAFDBFilename_GGO(mArgsInfo);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractAirwaysTreeInfoFilter<ImageType>::
GenerateOutputInformation() 
{ 
  Superclass::GenerateOutputInformation();
  //this->GetOutput(0)->SetRequestedRegion(this->GetOutput(0)->GetLargestPossibleRegion());

  // Get input pointer
  input   = dynamic_cast<const ImageType*>(itk::ProcessObject::GetInput(0));

  // Try to load the DB
  try {
    LoadAFDB();
  } catch (clitk::ExceptionObject e) {
    // Do nothing if not found, it will be used anyway to write the result
  }
  

}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractAirwaysTreeInfoFilter<ImageType>::
GenerateData() 
{
  StartNewStep("Thinning filter (skeleton)");
  // Extract skeleton
  typedef itk::BinaryThinningImageFilter3D<ImageType, ImageType> ThinningFilterType;
  typename ThinningFilterType::Pointer thinningFilter = ThinningFilterType::New();
  thinningFilter->SetInput(input); // input = trachea
  thinningFilter->Update();
  skeleton = thinningFilter->GetOutput();
  StopCurrentStep<ImageType>(skeleton);

  // Find first point for tracking
  StartNewStep("Find first point for tracking");
  typedef itk::ImageRegionConstIteratorWithIndex<ImageType> IteratorType;
  IteratorType it(skeleton, skeleton->GetLargestPossibleRegion());
  it.GoToReverseBegin();
  while ((!it.IsAtEnd()) && (it.Get() == GetBackgroundValue())) { 
    --it;
  }
  if (it.IsAtEnd()) {
    clitkExceptionMacro("first point in the trachea skeleton not found.");
  }
  typename ImageType::IndexType index = it.GetIndex();
  DD(index);

  // Initialize neighborhooditerator
  typedef itk::NeighborhoodIterator<ImageType> NeighborhoodIteratorType;
  typename NeighborhoodIteratorType::SizeType radius;
  radius.Fill(1);
  NeighborhoodIteratorType nit(radius, skeleton, skeleton->GetLargestPossibleRegion());
    
  // Find first label number (must be different from BG and FG)
  typename ImageType::PixelType label = GetForegroundValue()+1;
  while ((label == GetBackgroundValue()) || (label == GetForegroundValue())) { label++; }
  DD(label);

  /*
    Tracking ? 
    Goal : find position of C, RUL, RML, RLL, LUL, LLL bronchus
    Carina : ok "easy", track, slice by slice until 2 path into different label
    -> follow at Right
       - 
    -> follow at Left
   */


  // Track from the first point
  StartNewStep("Start tracking");
  std::vector<BifurcationType> listOfBifurcations;
  m_SkeletonTree.set_head(index);
  TrackFromThisIndex(listOfBifurcations, skeleton, index, label, m_SkeletonTree.begin());
  DD("end track");
      
  // Convert index into physical point coordinates
  for(unsigned int i=0; i<listOfBifurcations.size(); i++) {
    skeleton->TransformIndexToPhysicalPoint(listOfBifurcations[i].index, 
					    listOfBifurcations[i].point);
  }

  // Search for the first slice that separate the bronchus
  // (carina). Labelize slice by slice, stop when the two points of
  // the skeleton ar not in the same connected component
  StartNewStep("Search for carina position");
  typedef clitk::ExtractSliceFilter<ImageType> ExtractSliceFilterType;
  typename ExtractSliceFilterType::Pointer extractSliceFilter = ExtractSliceFilterType::New();
  extractSliceFilter->SetInput(input);
  extractSliceFilter->SetDirection(2);
  extractSliceFilter->Update();
  typedef typename ExtractSliceFilterType::SliceType SliceType;
  std::vector<typename SliceType::Pointer> mInputSlices;
  extractSliceFilter->GetOutputSlices(mInputSlices);
  DD(mInputSlices.size());
      
  bool stop = false;
  int slice_index = listOfBifurcations[0].index[2]; // first slice from carina in skeleton
  int i=0;
  TreeIterator firstIter = m_SkeletonTree.child(listOfBifurcations[0].treeIter, 0);
  TreeIterator secondIter = m_SkeletonTree.child(listOfBifurcations[0].treeIter, 1);
  typename SliceType::IndexType in1;
  typename SliceType::IndexType in2;
  while (!stop) {
    //  Labelize the current slice
    typename SliceType::Pointer temp = Labelize<SliceType>(mInputSlices[slice_index],
							   GetBackgroundValue(), 
							   true, 
							   0); // min component size=0
    // Check the value of the two skeleton points;
    in1[0] = (*firstIter)[0];
    in1[1] = (*firstIter)[1];
    typename SliceType::PixelType v1 = temp->GetPixel(in1);
    in2[0] = (*secondIter)[0];
    in2[1] = (*secondIter)[1];
    typename SliceType::PixelType v2 = temp->GetPixel(in2);

    // Check the label value of the two points
    DD(slice_index);
    if (v1 != v2) {
      stop = true; // We found it !
    }
    else {
      // Check error
      if (slice_index == (int)(mInputSlices.size()-1)) {
	clitkExceptionMacro("Error while searching for carina, the two skeleton points are always in the same CC ... ???");
      }
      // Iterate
      i++;
      --slice_index;
      ++firstIter;
      ++secondIter;
    }
  }
  ImageIndexType carina_index; // middle position in X/Y
  carina_index[0] = lrint(in2[0] + in1[0])/2.0;
  carina_index[1] = lrint(in2[1] + in1[1])/2.0;
  carina_index[2] = slice_index;
  // Get physical coordinates
  ImagePointType carina_position;
  skeleton->TransformIndexToPhysicalPoint(carina_index,
					  carina_position);

  // Set and save Carina position      
  if (GetVerboseStep()) {
    std::cout << "\t Found carina at " << carina_position << " mm" << std::endl;
  }
  GetAFDB()->SetPoint3D("Carina", carina_position);
      
  // Write bifurcation (debug)
  for(uint i=0; i<listOfBifurcations.size(); i++) {
    GetAFDB()->SetPoint3D("Bif"+toString(i), listOfBifurcations[i].point);
  }

  // Set the output (skeleton);
  this->GraftOutput(skeleton); // not SetNthOutput
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractAirwaysTreeInfoFilter<ImageType>::
TrackFromThisIndex(std::vector<BifurcationType> & listOfBifurcations, 
                   ImagePointer skeleton, 
                   ImageIndexType index,
                   ImagePixelType label, 
		   TreeIterator currentNode) 
{
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
    nit.SetCenterPixel(label);
    listOfTrackedPoint.clear();
    for(unsigned int i=0; i<nit.Size(); i++) {
      if (i != nit.GetCenterNeighborhoodIndex ()) { // Do not observe the current point
        if (nit.GetPixel(i) == GetForegroundValue()) { // if this is foreground, we continue the tracking
          listOfTrackedPoint.push_back(nit.GetIndex(i));
        }
      }
    }
    if (listOfTrackedPoint.size() == 1) {
      // Add this point to the current path
      currentNode = m_SkeletonTree.append_child(currentNode, listOfTrackedPoint[0]);
      index = listOfTrackedPoint[0];
      skeleton->SetPixel(index, label); // change label in skeleton image
    }
    else {
      if (listOfTrackedPoint.size() == 2) {
        // m_SkeletonTree->Add(listOfTrackedPoint[0], index); // the parent is 'index'
        // m_SkeletonTree->Add(listOfTrackedPoint[1], index); // the parent is 'index'
        BifurcationType bif(index, label, label+1, label+2);
	bif.treeIter = currentNode;
        listOfBifurcations.push_back(bif);
	TreeIterator firstNode = m_SkeletonTree.append_child(currentNode, listOfTrackedPoint[0]);
        TreeIterator secondNode = m_SkeletonTree.append_child(currentNode, listOfTrackedPoint[1]);
        TrackFromThisIndex(listOfBifurcations, skeleton, listOfTrackedPoint[0], label+1, firstNode);
        TrackFromThisIndex(listOfBifurcations, skeleton, listOfTrackedPoint[1], label+2, secondNode);
      }
      else {
        //DD(listOfTrackedPoint.size());
        if (listOfTrackedPoint.size() > 2) {
          //clitkExceptionMacro("error while tracking trachea bifurcation. Too much bifurcation points ... ?");
          stop = true; // this it the end of the tracking
        }
        // Else this it the end of the tracking
      }
      stop = true;
    }
  }
}
//--------------------------------------------------------------------

/*TrackFromThisIndex(std::vector<BifurcationType> & listOfBifurcations, 
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
*/
//--------------------------------------------------------------------


#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
