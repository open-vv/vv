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
#ifndef clitkMultiResolutionPyramidRegionFilter_txx
#define clitkMultiResolutionPyramidRegionFilter_txx

/* =================================================
 * @file   clitkMultiResolutionPyramidRegionFilter.txx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


namespace clitk
{

  //-------------------------------------------------------------------
  // Update with the number of dimensions
  //-------------------------------------------------------------------
  template<class InputImageType>
  MultiResolutionPyramidRegionFilter<InputImageType>::MultiResolutionPyramidRegionFilter()
  {
    ;
  }


  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template <class InputImageType> 
  void 
  MultiResolutionPyramidRegionFilter<InputImageType>::Update()
  {

    // Compute the FixedImageRegion corresponding to each level of the 
    // pyramid. This uses the same algorithm of the ShrinkImageFilter 
    // since the regions should be compatible. 
    unsigned int numberOfLevels=m_Schedule.rows();
    m_RegionPyramid.reserve( numberOfLevels );
    m_RegionPyramid.resize( numberOfLevels );
    SizeType inputSize =m_Region.GetSize();
    IndexType inputStart =m_Region.GetIndex();

    for ( unsigned int level=0; level < numberOfLevels; level++ )
      {
	
	SizeType  size;
	IndexType start;
	for ( unsigned int dim = 0; dim < InputImageDimension; dim++)
	  {
	    const float scaleFactor = static_cast<float>( m_Schedule[ level ][ dim ] );
	    
	    size[ dim ] = static_cast<typename SizeType::SizeValueType>(
		vcl_floor(static_cast<float>( inputSize[ dim ] ) / scaleFactor ) );
	    if( size[ dim ] < 1 )
	      {
		size[ dim ] = 1;
	      }
	    
	    start[ dim ] = static_cast<typename IndexType::IndexValueType>(
		  vcl_ceil(static_cast<float>( inputStart[ dim ] ) / scaleFactor ) ); 
	  }
	m_RegionPyramid[ level ].SetSize( size );
	m_RegionPyramid[ level ].SetIndex( start );
    }
  }


}//end clitk
 
#endif //#define clitkMultiResolutionPyramidRegionFilter_txx
