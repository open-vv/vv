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

#ifndef CLITKBOUNDINGBOXUTILS_H
#define CLITKBOUNDINGBOXUTILS_H

// clitk
#include "clitkCommon.h"

// itk
#include <itkBoundingBox.h>

namespace clitk {

  //--------------------------------------------------------------------
  template<class ImageType>
  void ComputeBBFromImageRegion(const ImageType * image, 
                                typename ImageType::RegionType region,
                                typename itk::BoundingBox<unsigned long, 
                                                          ImageType::ImageDimension>::Pointer bb);
  
  //--------------------------------------------------------------------
  template<int Dimension>
  void ComputeBBIntersection(typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbo, 
                             typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbi1, 
                             typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbi2);

  //--------------------------------------------------------------------
  template<int Dimension>
  void ComputeBBIntersection(typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbo, 
                             typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbi1, 
                             typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbi2, 
                             int dimension);

  //--------------------------------------------------------------------
  template<int Dimension>
  void ComputeBBUnion(typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbo, 
                      typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbi1, 
                      typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbi2);
  
  //--------------------------------------------------------------------
  template<class ImageType>
  void ComputeRegionFromBB(const ImageType * image, 
                           const typename itk::BoundingBox<unsigned long, 
                                                           ImageType::ImageDimension>::Pointer bb, 
                           typename ImageType::RegionType & region);

} // end clitk namespace

#include "clitkBoundingBoxUtils.txx"

#endif
