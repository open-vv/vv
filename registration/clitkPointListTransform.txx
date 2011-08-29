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
#ifndef __clitkPointListTransform_txx
#define __clitkPointListTransform_txx
#include "clitkPointListTransform.h"


namespace clitk
{

  // Constructor
  template<class TScalarType, unsigned int NDimensions, unsigned int NOutputDimensions>
  PointListTransform<TScalarType, NDimensions, NOutputDimensions>
#if ITK_VERSION_MAJOR >= 4
  ::PointListTransform():Superclass(1)
#else
  ::PointListTransform():Superclass(NOutputDimensions,1)
#endif
  {
    m_PointLists.resize(0);
    m_PointList.resize(1);
  }
    
  // Find the point list in the lists
  template<class TScalarType, unsigned int NDimensions,unsigned int NOutputDimensions>
  typename PointListTransform<TScalarType,  NDimensions, NOutputDimensions>::PointListType
  PointListTransform<TScalarType, NDimensions,NOutputDimensions>::
  GetCorrespondingPointList(const InputPointType &inputPoint) const 
  { 
    SpacePointType point;
    for(unsigned int j=0; j< SpaceDimension;j++)
      point[j]=inputPoint[j];
    
    if(m_PointList[0]==point) return m_PointList;
    else
      {
	for (unsigned int i=0; i< m_PointLists.size();i++)
	  if(m_PointLists[i][0]==point) return m_PointLists[i];
      }
    
    itkExceptionMacro(<<"Point List not found");
  }


  // Transform a point
  template<class TScalarType, unsigned int NDimensions,unsigned int NOutputDimensions>
  typename PointListTransform<TScalarType,  NDimensions,NOutputDimensions>::OutputPointType
  PointListTransform<TScalarType, NDimensions,NOutputDimensions>::
  TransformPoint(const InputPointType &inputPoint) const 
  {

    // -------------------------------
    // Get the corresponding point list
    m_PointList = this->GetCorrespondingPointList(inputPoint);

    // -------------------------------
    // Create 1D vector image
    typename PointListImageType::Pointer pointListImage=PointListImageType::New();
    typename PointListImageType::RegionType region;
    region.SetSize(0,m_PointList.size()+6);
    pointListImage->SetRegions(region);
    pointListImage->Allocate();
    typename PointListImageType::SpacingType spacing;
    spacing[0]=1;
    pointListImage->SetSpacing(spacing);
    typename PointListImageType::PointType origin;
    origin[0]=-2.;
    pointListImage->SetOrigin(origin);


    // -------------------------------
    // Copy Point list to image
    typedef itk::ImageRegionIterator<PointListImageType> IteratorType;
    IteratorType it(pointListImage, pointListImage->GetLargestPossibleRegion());

    // First points are the last
    PointListImagePixelType pixel;
    for (unsigned int j=0; j<2;j++)
      {
	for (unsigned int i=0; i <SpaceDimension; i++)
	  pixel[i]=m_PointList[m_PointList.size()-2+j][i];
	it.Set(pixel); 
	++it;
      }

    // Copy the rest
    unsigned int position=0;
    while(position< m_PointList.size())
      {
	for (unsigned int i=0; i <SpaceDimension; i++)
	  pixel[i]=m_PointList[position][i];
	it.Set(pixel); 
	++it;
	++position;
      }
    
    // last points are the first
    for (unsigned int j=0; j<4;j++)
      {
	for (unsigned int i=0; i <SpaceDimension; i++)
	  pixel[i]=m_PointList[j][i];
	it.Set(pixel); 
	++it;
      }	

    // -------------------------------
    // Set 1D image to vectorInterpolator
    m_Interpolator->SetInputImage(pointListImage);
   

    // -------------------------------
    // Evaluate at phase value
    typename PointListImageType::PointType t;
    t[0]=inputPoint[ImageDimension-1];

    // Inside valid region?
    if ( (t[0] >= 0) &&
	 (t[0] < m_PointList.size()) )
      {
	pixel= m_Interpolator->Evaluate(t);
	OutputPointType outputPoint;
	for (unsigned int i=0; i < SpaceDimension; i++)
	  outputPoint[i]=pixel[i];
	outputPoint[ImageDimension-1]=t[0];
	return outputPoint;
      }
    // No displacement
    else return inputPoint;
	 
  }

  
} // namespace clitk

#endif
