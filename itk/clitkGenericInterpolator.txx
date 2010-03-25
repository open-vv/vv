#ifndef __clitkGenericInterpolator_txx	
#define __clitkGenericInterpolator_txx
#include "clitkGenericInterpolator.h"


namespace clitk
{

  //=========================================================================================================================
  //constructor
  template <class args_info_type,  class ImageType, class TCoordRep> 
  GenericInterpolator<args_info_type, ImageType, TCoordRep>::GenericInterpolator()
  {
    m_Interpolator=NULL;
    m_Verbose=false;
  }
  
  
  //=========================================================================================================================
  //Get the pointer
  template <class args_info_type,  class ImageType, class TCoordRep> 
  typename GenericInterpolator<args_info_type, ImageType, TCoordRep>::InterpolatorPointer 
  GenericInterpolator<args_info_type, ImageType, TCoordRep>::GetInterpolatorPointer()
  {
    //============================================================================
    // We retrieve the type of interpolation from the command line
    //============================================================================
    typename InterpolatorType::Pointer interpolator;  
  
    switch ( m_ArgsInfo.interp_arg )
      {
      case 0: 

	interpolator= itk::NearestNeighborInterpolateImageFunction< ImageType,TCoordRep >::New();
	if (m_Verbose) std::cout<<"Using nearestneighbor interpolation..."<<std::endl;
	break;
 
      case 1:

	interpolator = itk::LinearInterpolateImageFunction< ImageType,TCoordRep >::New();
	if (m_Verbose) std::cout<<"Using linear interpolation..."<<std::endl;
	break;  
      
      case 2:
	{	
	  typename itk::BSplineInterpolateImageFunction< ImageType,TCoordRep >::Pointer m =itk::BSplineInterpolateImageFunction< ImageType,TCoordRep >::New();
	  m->SetSplineOrder(m_ArgsInfo.interpOrder_arg);
	  interpolator=m;
	  if (m_Verbose) std::cout<<"Using Bspline interpolation..."<<std::endl;
	  break; 
	}

      case 3:
	{
	  typename itk::BSplineInterpolateImageFunctionWithLUT< ImageType,TCoordRep >::Pointer m =itk::BSplineInterpolateImageFunctionWithLUT< ImageType,TCoordRep >::New();
	  m->SetSplineOrder(m_ArgsInfo.interpOrder_arg);
	  m->SetLUTSamplingFactor(m_ArgsInfo.interpSF_arg);
	  interpolator=m;
	  if (m_Verbose) std::cout<<"Using BLUT interpolation..."<<std::endl;
	  break;
	} 


      }//end of switch
    
    
    //============================================================================
    //return the pointer
    return interpolator;
  }
  
}

#endif
