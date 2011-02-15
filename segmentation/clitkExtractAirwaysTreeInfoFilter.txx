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

  // Crop just abov lungs ?

  // read skeleton if already exist
  bool foundSkeleton = true;
  try {
    skeleton = GetAFDB()->template GetImage <ImageType>("skeleton");
  }
  catch (clitk::ExceptionObject e) {
    DD("did not found skeleton");
    foundSkeleton = false;
  }

  // Extract skeleton
  if (!foundSkeleton) {
    StartNewStep("Thinning filter (skeleton)");
    typedef itk::BinaryThinningImageFilter3D<ImageType, ImageType> ThinningFilterType;
    typename ThinningFilterType::Pointer thinningFilter = ThinningFilterType::New();
    thinningFilter->SetInput(input); // input = trachea
    thinningFilter->Update();
    skeleton = thinningFilter->GetOutput();
    StopCurrentStep<ImageType>(skeleton);
    writeImage<ImageType>(skeleton, "skeleton.mhd");
  }

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

  if (0) {
    // Initialize neighborhooditerator
    typedef itk::NeighborhoodIterator<ImageType> NeighborhoodIteratorType;
    typename NeighborhoodIteratorType::SizeType radius;
    radius.Fill(1);
    NeighborhoodIteratorType nit(radius, skeleton, skeleton->GetLargestPossibleRegion());
  }
    
  // Find a label number different from BG and FG
  typename ImageType::PixelType label = GetForegroundValue()+1;
  while ((label == GetBackgroundValue()) || (label == GetForegroundValue())) { label++; }
  DD((int)label);

  /*
    Tracking ? 
    Goal : find position of C, RUL, RML, RLL, LUL, LLL bronchus
    Carina : ok "easy", track, slice by slice until 2 path into different label
    -> follow at Right
       - 
    -> follow at Left
   */

  // NEW Track from the first point
  StartNewStep("Start tracking");
  FullTreeNodeType n;
  n.index = index;
  skeleton->TransformIndexToPhysicalPoint(n.index, n.point); // à mettre dans TrackFromThisIndex2
  mFullSkeletonTree.set_head(n);
  StructuralTreeNodeType sn;
  sn.index = index;
  skeleton->TransformIndexToPhysicalPoint(sn.index, sn.point);
  mStructuralSkeletonTree.set_head(sn);
  TrackFromThisIndex2(index, skeleton, label, mFullSkeletonTree.begin(), mStructuralSkeletonTree.begin());
  StopCurrentStep();

  // Reput FG instead of label in the skeleton image
  skeleton = clitk::SetBackground<ImageType, ImageType>(skeleton, skeleton, label, GetForegroundValue(), true);        

  // Debug 
  typename StructuralTreeType::iterator sit = mStructuralSkeletonTree.begin();
  while (sit != mStructuralSkeletonTree.end()) {
    DD(sit->point);
    ++sit;
  }

  // compute weight : n longueurs à chaque bifurfaction.
  // parcours de FullTreeNodeType, à partir de leaf, remonter de proche en proche la distance eucl. 
  // par post order

  // Init
  typename FullTreeType::iterator fit = mFullSkeletonTree.begin();
  while (fit != mFullSkeletonTree.end()) {
    fit->weight = 0.0;
    ++fit;
  }
  
  DD("compute weight");
  typename FullTreeType::post_order_iterator pit = mFullSkeletonTree.begin_post();
  while (pit != mFullSkeletonTree.end_post()) {
    //DD(pit->point);
    /*
    if (pit != mFullSkeletonTree.begin()) {
      typename FullTreeType::iterator parent = mFullSkeletonTree.parent(pit);
      double d = pit->point.EuclideanDistanceTo(parent->point);
      // DD(parent->weight);
      //DD(d);
      parent->weight += d;
      if (pit.number_of_children() > 1) {
	DD(pit.number_of_children());
	DD(pit->point);
	DD(pit->weight);
	DD(parent->weight);
	DD(mFullSkeletonTree.child(pit, 0)->weight);
	DD(mFullSkeletonTree.child(pit, 1)->weight);
      }
    }
    */
    double previous = pit->weight;
    for(uint i=0; i<pit.number_of_children(); i++) {
      double d = pit->point.EuclideanDistanceTo(mFullSkeletonTree.child(pit, i)->point);
      //      pit->weight = pit->weight + mFullSkeletonTree.child(pit, i)->weight + d;
      if (i==0)
	pit->weight = pit->weight + mFullSkeletonTree.child(pit, i)->weight + d;
      if (i>0) {
	DD(pit.number_of_children());
	DD(pit->point);
	DD(pit->weight);
	DD(mFullSkeletonTree.child(pit, 0)->weight);
	DD(mFullSkeletonTree.child(pit, 1)->weight);
	pit->weight = std::max(pit->weight, previous+mFullSkeletonTree.child(pit, i)->weight + d);
      }
    }
    ++pit;
  }

  DD("end");
  fit = mFullSkeletonTree.begin();
  while (fit != mFullSkeletonTree.end()) {
    std::cout << "p = " << fit->point 
	      << " " << fit->weight
	      << " child= " << fit.number_of_children()
	      << " -> ";
    for(uint i=0; i<fit.number_of_children(); i++) {
      std::cout << " " << mFullSkeletonTree.child(fit, i)->weight;
    }
    std::cout << std::endl;
    ++fit;
  }

  /* Selection criteria ? 
     - at least 2 child
     - from top to bottom
     - equilibr ? 
   */
  DD("=========================================");
  fit = mFullSkeletonTree.begin();
  while (fit != mFullSkeletonTree.end()) {
    if (fit.number_of_children() > 1) {
      std::cout << "ppp = " << fit->point 
		<< " " << fit->weight << std::endl;
      for(uint i=1; i<fit.number_of_children(); i++) {
	double w1 = mFullSkeletonTree.child(fit, i-1)->weight;
	double w2 = mFullSkeletonTree.child(fit, i)->weight;
	// if (w1 <= 0.1) break;
	// if (w2 <= 0.1) break;
	DD(w1);DD(w2);
	if (w1>w2) {
	  double sw=w1;
	  w1=w2; w2=sw;
	}
	DD(w1/w2);
	if (w1/w2 > 0.7) {
	  DD("KEEP IT");
	}
      }
    }
    ++fit;
  }


  if (0) {
    // Track from the first point
    StartNewStep("Start tracking OLD");
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
      

  DD("REDO !!!!!!!!!!!!");
  /**
     => chercher la bif qui a les plus important sous-arbres
     - iterate sur m_SkeletonTree depth first, remonter au parent, add value = distance

     NONNNNN m_SkeletonTree n'est pas la structure mais l'ensemble avec ts le skeleton

  TreeIterator itt = m_SkeletonTree.begin();
  while (itt != m_SkeletonTree.end()) {
    itt->weight = 0.0;
    ++itt;
  }

  typename tree<NodeType>::post_order_iterator pit = m_SkeletonTree.begin_post();
  while (pit != m_SkeletonTree.end_post()) {
    typename tree<NodeType>::iterator parent = m_SkeletonTree.parent(pit);
    double d = pit->point.EuclideanDistanceTo(parent);
    DD(parent.weight);
    DD(d);
    parent.weight += d;
    ++it;
  }

  itt = m_SkeletonTree.begin();
  while (itt != m_SkeletonTree.end()) {
    DD(itt->weight);
    ++itt;
  }
   **/


  // search for carina
  bool stop = false;
  int slice_index = listOfBifurcations[0].index[2]; // first slice from carina in skeleton
  int i=0;
  TreeIterator firstIter = m_SkeletonTree.child(listOfBifurcations[0].treeIter, 0);
  TreeIterator secondIter = m_SkeletonTree.child(listOfBifurcations[0].treeIter, 1);
  DD(firstIter.number_of_children());
  DD(secondIter.number_of_children());
  typename SliceType::IndexType in1;
  typename SliceType::IndexType in2;
  while (!stop) {
    //  Labelize the current slice
    typename SliceType::Pointer temp = Labelize<SliceType>(mInputSlices[slice_index],
							   GetBackgroundValue(), 
							   true, 
							   0); // min component size=0
    DD(*firstIter);
    DD(*secondIter);    
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
  if (GetVerboseStepFlag()) {
    std::cout << "\t Found carina at " << carina_position << " mm" << std::endl;
  }
  GetAFDB()->SetPoint3D("carina", carina_position);
      
  // Write bifurcation (debug)
  for(uint i=0; i<listOfBifurcations.size(); i++) {
    GetAFDB()->SetPoint3D("Bif"+toString(i), listOfBifurcations[i].point);
  }
  }

  // Set the output (skeleton);
  this->GraftOutput(skeleton); // not SetNthOutput
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/**
   Start from the pixel "index" in the image "skeleton" and track the
   path from neighbor pixels. Put the tracked path in the tree at the
   currentNode position. Label is used to mark the FG pixel as already
   visited. Progress recursively when several neighbors are found.
 **/
template <class ImageType>
void 
clitk::ExtractAirwaysTreeInfoFilter<ImageType>::
TrackFromThisIndex2(ImageIndexType index, ImagePointer skeleton, 
                    ImagePixelType label, 
		    typename FullTreeType::iterator currentNode, 
                    typename StructuralTreeType::iterator currentSNode) 
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
        if (nit.GetPixel(i) == GetForegroundValue()) { 
	  // if pixel value is FG, we keep this point
          listOfTrackedPoint.push_back(nit.GetIndex(i));
        }
      }
    }
    // If only neighbor is found, we keep the point and continue
    if (listOfTrackedPoint.size() == 1) {
      FullTreeNodeType n;
      index = n.index = listOfTrackedPoint[0];
      skeleton->TransformIndexToPhysicalPoint(n.index, n.point);
      currentNode = mFullSkeletonTree.append_child(currentNode, n);
      skeleton->SetPixel(n.index, label); // change label in skeleton image
    }
    else {
      if (listOfTrackedPoint.size() >= 2) {
	for(uint i=0; i<listOfTrackedPoint.size(); i++) {
	  FullTreeNodeType n;
          n.index = listOfTrackedPoint[i];
	  skeleton->TransformIndexToPhysicalPoint(n.index, n.point);
          typename FullTreeType::iterator node = mFullSkeletonTree.append_child(currentNode, n);
	  StructuralTreeNodeType sn;
          sn.index = listOfTrackedPoint[i];
	  skeleton->TransformIndexToPhysicalPoint(sn.index, sn.point);
          typename StructuralTreeType::iterator snode = mStructuralSkeletonTree.append_child(currentSNode, sn);
	  TrackFromThisIndex2(listOfTrackedPoint[i], skeleton, label, node, snode);
        }
      }
      stop = true; // this it the end of the tracking
    } // end else
  } // end while stop
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
        DD("BifurcationType");
        DD(listOfTrackedPoint[0]);
        DD(listOfTrackedPoint[1]);
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
