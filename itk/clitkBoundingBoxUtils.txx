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
  ======================================================================-====*/

namespace clitk {

  //--------------------------------------------------------------------
  template<class ImageType>
  void ComputeBBFromImageRegion(const ImageType * image, 
                                typename ImageType::RegionType region,
                                typename itk::BoundingBox<unsigned long, 
                                ImageType::ImageDimension>::Pointer bb) {
    typedef typename ImageType::IndexType IndexType;
    IndexType firstIndex;
    IndexType lastIndex;
    for(unsigned int i=0; i<image->GetImageDimension(); i++) {
      firstIndex[i] = region.GetIndex()[i];
      lastIndex[i] = firstIndex[i]+region.GetSize()[i];
    }

    typedef itk::BoundingBox<unsigned long, 
                             ImageType::ImageDimension> BBType;
    typedef typename BBType::PointType PointType;
    PointType lastPoint;
    PointType firstPoint;
    image->TransformIndexToPhysicalPoint(firstIndex, firstPoint);
    image->TransformIndexToPhysicalPoint(lastIndex, lastPoint);

    bb->SetMaximum(lastPoint);
    bb->SetMinimum(firstPoint);
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<int Dimension>
  void ComputeBBIntersection(typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbo, 
                             typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbi1, 
                             typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbi2) {

    typedef itk::BoundingBox<unsigned long, Dimension> BBType;
    typedef typename BBType::PointType PointType;
    PointType lastPoint;
    PointType firstPoint;

    for(unsigned int i=0; i<Dimension; i++) {
      firstPoint[i] = std::max(bbi1->GetMinimum()[i], 
                               bbi2->GetMinimum()[i]);
      lastPoint[i] = std::min(bbi1->GetMaximum()[i], 
                              bbi2->GetMaximum()[i]);
    }

    bbo->SetMaximum(lastPoint);
    bbo->SetMinimum(firstPoint);
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void ComputeRegionFromBB(const ImageType * image, 
                           const typename itk::BoundingBox<unsigned long, 
                                                           ImageType::ImageDimension>::Pointer bb, 
                           typename ImageType::RegionType & region) {
    // Types
    typedef typename ImageType::IndexType  IndexType;
    typedef typename ImageType::PointType  PointType;
    typedef typename ImageType::RegionType RegionType;
    typedef typename ImageType::SizeType   SizeType;

    // Region starting point
    IndexType regionStart;
    PointType start = bb->GetMinimum();
    image->TransformPhysicalPointToIndex(start, regionStart);
    
    // Region size
    SizeType regionSize;
    PointType maxs = bb->GetMaximum();
    PointType mins = bb->GetMinimum();
    for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
      regionSize[i] = lrint((maxs[i] - mins[i])/image->GetSpacing()[i]);
    }
   
    // Create region
    region.SetIndex(regionStart);
    region.SetSize(regionSize);
  }
  //--------------------------------------------------------------------


} // end of namespace

