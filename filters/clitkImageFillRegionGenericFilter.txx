/*-------------------------------------------------------------------------
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
                                                                             
  -------------------------------------------------------------------------*/

#ifndef CLITKIMAGEFILLREGIONGENERICFILTER_TXX
#define CLITKIMAGEFILLREGIONGENERICFILTER_TXX

/*-------------------------------------------------
 * @file   clitkImageFillRegionGenericFilter.txx
 * @author Cristina Gimenez <cristina.gs@gmail.com>
 * @date   9 August 2006
 * 
 -------------------------------------------------*/

//--------------------------------------------------------------------
template<unsigned int Dim>
void ImageFillRegionGenericFilter::Update_WithDim() { 
#define TRY_TYPE(TYPE)							\
  if (IsSameType<TYPE>(mPixelTypeName)) { Update_WithDimAndPixelType<Dim, TYPE>(); return; } 
  // TRY_TYPE(signed char);
  // TRY_TYPE(uchar);
  TRY_TYPE(short);
  //TRY_TYPE(ushort);
  // TRY_TYPE(int);
  //   TRY_TYPE(unsigned int); 
  TRY_TYPE(float);
  // TRY_TYPE(double);
#undef TRY_TYPE

  std::string list = CreateListOfTypes<uchar, short, ushort, int, uint, float, double>();
  std::cerr << "Error, I don't know the type '" << mPixelTypeName << "' for the input image '"
	    << mInputFilenames[0] << "'." << std::endl << "Known types are " << list << std::endl;
  exit(0);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<unsigned int Dim, class PixelType>
void ImageFillRegionGenericFilter::Update_WithDimAndPixelType() {

  // Spheric region
  if (mSphericRegion) return Update_WithDimAndPixelType_SphericRegion<Dim,PixelType>();

  // Read input
  typedef itk::Image<PixelType,Dim> ImageType;
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
void ImageFillRegionGenericFilter::Update_WithDimAndPixelType_SphericRegion() {

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

#endif  //#define CLITKIMAGEFILLREGIONGENERICFILTER_TXX
