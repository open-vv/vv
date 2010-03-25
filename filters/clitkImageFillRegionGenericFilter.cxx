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
#ifndef CLITKIMAGEFILLREGIONGENERICFILTER_CXX
#define CLITKIMAGEFILLREGIONGENERICFILTER_CXX
/**
 -------------------------------------------------------------------
 * @file   clitkImageFillRegionGenericFilter.cxx
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   23 Feb 2008

 * @brief  
 -------------------------------------------------------------------*/

#include "clitkImageFillRegionGenericFilter.h"

//--------------------------------------------------------------------
clitk::ImageFillRegionGenericFilter::ImageFillRegionGenericFilter():
  clitk::ImageToImageGenericFilter<Self>("ImageFillRegion") {
  InitializeImageType<2>();
  InitializeImageType<3>();   
  mPixelValue = 0;
  m_IsCentered=false;
  mSphericRegion=false;  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<unsigned int Dim>
void clitk::ImageFillRegionGenericFilter::InitializeImageType() {      
  // ADD_IMAGE_TYPE(Dim, char);
  ADD_IMAGE_TYPE(Dim, short);
  // ADD_IMAGE_TYPE(Dim, unsigned short);
//   ADD_IMAGE_TYPE(Dim, int);
  ADD_IMAGE_TYPE(Dim, float);
  // ADD_IMAGE_TYPE(Dim, double);
}
//--------------------------------------------------------------------



//--------------------------------------------------------------------
template<class ImageType>
void clitk::ImageFillRegionGenericFilter::UpdateWithInputImageType() {

  // Typedef
  typedef typename ImageType::PixelType PixelType;
  static unsigned int Dim = ImageType::ImageDimension;

  // Spheric region
  if (mSphericRegion) return Update_WithDimAndPixelType_SphericRegion<ImageType::ImageDimension,PixelType>();

  // Read input
  typename ImageType::Pointer input = GetInput<ImageType>(0);

  // Get pixel value in correct type
  PixelType value = PixelTypeDownCast<double, PixelType>(mPixelValue); 

  // Get region
  typedef typename ImageType::RegionType RegionType;
  typedef typename ImageType::SizeType SizeType;
  typedef typename ImageType::IndexType IndexType;
  RegionType region;
  SizeType size;
  IndexType start;
  for(unsigned int i=0; i<Dim; i++) {
    size[i] = mSize[i];
    start[i] = mStart[i];
  }
  region.SetSize(size);
  region.SetIndex(start);

  // Build iterator
  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  IteratorType it(input, region);
  it.GoToBegin();
  while (!it.IsAtEnd()) {
    it.Set(value);
    ++it;
  }

  // Write results
  SetNextOutput<ImageType>(input);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<unsigned int Dim, class PixelType>
void clitk::ImageFillRegionGenericFilter::Update_WithDimAndPixelType_SphericRegion() {

  // Read input
  typedef itk::Image<PixelType,Dim> ImageType;
  //typename ImageType::Pointer input = clitk::readImage<ImageType>(mInputFilenames[0], mIOVerbose);
  typename ImageType::Pointer input = GetInput<ImageType>(0);

  // Get pixel value in correct type
  PixelType value = PixelTypeDownCast<double, PixelType>(mPixelValue); 

  // Centered?
  if(m_IsCentered)
    {
      typename ImageType::SizeType size= input->GetLargestPossibleRegion().GetSize();
      typename ImageType::SpacingType spacing= input->GetSpacing();
      typename ImageType::PointType origin= input->GetOrigin();
      mCenter.resize(Dim);
      for (unsigned int i=0; i<Dim; i++)
	mCenter[i]=origin[i]+(double)size[i]/2*spacing[i];
    }

  // Build iterator
  typedef itk::ImageRegionIteratorWithIndex<ImageType> IteratorType;
  IteratorType it(input, input->GetLargestPossibleRegion());
  it.GoToBegin();

  typename ImageType::PointType point; 
  //typename itk::Vector<double, Dim> distance; 
  typename ImageType::IndexType index;
  //bool inside;
  double distance;
  
  while (!it.IsAtEnd())
    {    
      //      inside=true;
      index=it.GetIndex();
      input->TransformIndexToPhysicalPoint(index, point);
      distance=0.0;      
      for(unsigned int i=0; i<Dim; i++)
	distance+=powf( ( (mCenter[i]-point[i])/mRadius[i] ), 2);
	
      //  inside= ( (fabs(distance[i])<fabs(mRadius[i])) && inside );
      // 	  distance[i]=mCenter[i]-point[i];
      // 	  inside= ( (fabs(distance[i])<fabs(mRadius[i])) && inside );
      //	}
      
      if (distance<1)
	it.Set(value);
      ++it;
    }

  // Write results
  SetNextOutput<ImageType>(input);
}

//--------------------------------------------------------------------



//--------------------------------------------------------------------
void clitk::ImageFillRegionGenericFilter::SetSphericRegion(std::vector<double> &  radius, 
  							   std::vector<double> & center) 
{
  mRadius.clear(); 
  mRadius.resize(radius.size());
  std::copy(radius.begin(), radius.end(), mRadius.begin());
  mCenter.clear();
  mCenter.resize(center.size());
  std::copy(center.begin(), center.end(), mCenter.begin());
  mSphericRegion = true;
  m_IsCentered=false;
}

void clitk::ImageFillRegionGenericFilter::SetSphericRegion(std::vector<double> & radius) {
  mRadius.clear(); 
  mRadius.resize(radius.size());
  std::copy(radius.begin(), radius.end(), mRadius.begin());
  m_IsCentered=true;
  mSphericRegion = true;
}
//--------------------------------------------------------------------


#endif //define CLITKIMAGEFILLREGIONGENERICFILTER_CXX
