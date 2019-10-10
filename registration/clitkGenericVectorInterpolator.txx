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
#ifndef __clitkGenericVectorInterpolator_txx	
#define __clitkGenericVectorInterpolator_txx
#include "clitkGenericVectorInterpolator.h"


namespace clitk
{

  //=========================================================================================================================
  //constructor
  template <class args_info_type, class ImageType, class TCoordRep> 
  GenericVectorInterpolator<args_info_type, ImageType, TCoordRep>::GenericVectorInterpolator()
  {
    m_Interpolator=NULL;
    m_Verbose=false;
  }
  
  
  //=========================================================================================================================
  //Get the pointer
  template <class args_info_type, class ImageType, class TCoordRep> 
  typename GenericVectorInterpolator<args_info_type, ImageType, TCoordRep>::InterpolatorPointer 
  GenericVectorInterpolator<args_info_type, ImageType, TCoordRep>::GetInterpolatorPointer()
  {
    //============================================================================
    // We retrieve the type of interpolation from the command line
    //============================================================================
    typename InterpolatorType::Pointer interpolator;  
  
    switch ( m_ArgsInfo.interpVF_arg )
      {
      case 0: 

#if ( ITK_VERSION_MAJOR < 5 )
	interpolator= itk::VectorNearestNeighborInterpolateImageFunction< ImageType,TCoordRep >::New();
#else
	interpolator= itk::NearestNeighborInterpolateImageFunction< ImageType,TCoordRep >::New();
#endif
	if (m_Verbose) std::cout<<"Using nearestneighbor interpolation..."<<std::endl;
	break;
 
      case 1:

#if ( ITK_VERSION_MAJOR < 5 )
	interpolator = itk::VectorLinearInterpolateImageFunction< ImageType,TCoordRep >::New();
#else
	interpolator = itk::LinearInterpolateImageFunction< ImageType,TCoordRep >::New();
#endif
	if (m_Verbose) std::cout<<"Using linear interpolation..."<<std::endl;
	break;  
      
/*      case 2:
	{
	  typename clitk::VectorBSplineInterpolateImageFunction< ImageType,TCoordRep >::Pointer m =clitk::VectorBSplineInterpolateImageFunction< ImageType,TCoordRep >::New();
	  m->SetSplineOrder(m_ArgsInfo.interpVFOrder_arg);
	  interpolator=m;
	  if (m_Verbose) std::cout<<"Using Bspline interpolation..."<<std::endl;
	  break; 
	}
	
      case 3:
	{
	  typename clitk::VectorBSplineInterpolateImageFunctionWithLUT< ImageType,TCoordRep >::Pointer m =clitk::VectorBSplineInterpolateImageFunctionWithLUT< ImageType,TCoordRep >::New();
	  m->SetSplineOrder(m_ArgsInfo.interpVFOrder_arg);
	  m->SetLUTSamplingFactor(m_ArgsInfo.interpVFSF_arg);
	  interpolator=m;
	  if (m_Verbose) std::cout<<"Using BLUT interpolation..."<<std::endl;
	  break;
	} 
*/
      }//end of switch
    
    
    //============================================================================
    //return the pointer
    return interpolator;
  }
  
}

#endif
