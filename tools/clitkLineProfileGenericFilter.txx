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
#ifndef clitkLineProfileGenericFilter_cxx
#define clitkLineProfileGenericFilter_cxx

/* =================================================
 * @file   clitkLineProfileGenericFilter.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/

#include "itkBresenhamLine.h"

namespace clitk
{

//-----------------------------------------------------------
  // Update
  //-----------------------------------------------------------
  template<class InputImageType> 
  void LineProfileGenericFilter::UpdateWithInputImageType()
  {
    typedef InputImageType ImageType;

    if (m_Verbose)
      std::cout << "LineProfileGenericFilter::UpdateWithInputImageType" << std::endl;
    
    // type checks
    if (m_ArgsInfo.p0_given != ImageType::ImageDimension ||
        m_ArgsInfo.p1_given != ImageType::ImageDimension) 
        throw std::logic_error("Dimension of input points and image do not match.");

    typename ImageType::Pointer image = this->template GetInput<InputImageType>(0);
    typename ImageType::RegionType region = image->GetLargestPossibleRegion();

    typedef typename ImageType::PointType PointType;
    PointType p0, p1;
    for (unsigned int i = 0; i < ImageType::ImageDimension; i++) {
      p0[i] = m_ArgsInfo.p0_arg[i];
      p1[i] = m_ArgsInfo.p1_arg[i];
    }

    // compute params of line between p0 and p1
    // length (magnitude) must be an integer value, so it's
    // the max distance along one the axes plus one to account
    // for the last point.
    typedef itk::BresenhamLine<ImageType::ImageDimension> LineType;
    typename LineType::LType direction = p1 - p0;
    typename LineType::LType::RealValueType mag = 0;
    for (unsigned int i = 0; i < ImageType::ImageDimension; i++) {
      if (direction[i] > mag)
        mag = direction[i];
    }
    mag++;
    
    if (m_Verbose)
      std::cout << "Building line with direction = " << direction << " and length = " << mag << "..." << std::endl;
    
    // build the line itself
    LineType line;
    typename LineType::OffsetArray offsets;
    offsets = line.BuildLine(direction, mag);
     
    if (m_Verbose)
      std::cout << "Line has " << offsets.size() << " points" << std::endl;
    
    // fill the output vectors
    typedef typename ImageType::PixelType PixelType;
    typedef typename ImageType::OffsetType OffsetType;
    typedef typename ImageType::IndexType IndexType;
    typedef std::vector<IndexType> IndexArrayType;
    typedef std::vector<PixelType> ValueArrayType;

    IndexType index0, index1;
    for (unsigned int i = 0; i < ImageType::ImageDimension; i++) {
      index0[i] = m_ArgsInfo.p0_arg[i];
      index1[i] = m_ArgsInfo.p1_arg[i];
    }
    
    if (m_Verbose)
      std::cout << "Getting profile along line..." << std::endl;
    
    IndexType index;
    IndexArrayType line_indices;
    ValueArrayType line_values;
    for (size_t i = 0; i < offsets.size(); i++)
    {
      index = index0 + offsets[i];
      if (m_Verbose) {
        std::cout << "index " << i << " = " << index << std::endl;
      }
      
      if (region.IsInside(index)) {
        if (m_Verbose)
          std::cout << "value " << i << " = " << image->GetPixel(index) << std::endl;
        
        line_indices.push_back(index);
        line_values.push_back(image->GetPixel(index));
      }
      else if (m_Verbose)
        std::cout << "index outside image" << std::endl;
    }
    
    if (m_Verbose) {
      std::cout << "I bring to you The Computed Points!" << std::endl;
    }
    
    for (size_t i = 0; i < line_indices.size(); i++) {
      std::cout << line_indices[i] << " " << line_values[i] << std::endl;
    }
  }
  

} //end clitk

#endif  //#define clitkLineProfileGenericFilter_cxx
