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

  switch ( m_ArgsInfo.interp_arg ) {
  case 0:

    m_Interpolator= itk::NearestNeighborInterpolateImageFunction< ImageType,TCoordRep >::New();
    if (m_Verbose) std::cout<<"Using nearestneighbor interpolation..."<<std::endl;
    break;

  case 1:

    m_Interpolator = itk::LinearInterpolateImageFunction< ImageType,TCoordRep >::New();
    if (m_Verbose) std::cout<<"Using linear interpolation..."<<std::endl;
    break;

  case 2: {
    typename itk::BSplineInterpolateImageFunction< ImageType,TCoordRep >::Pointer m =itk::BSplineInterpolateImageFunction< ImageType,TCoordRep >::New();
    m->SetSplineOrder(m_ArgsInfo.interpOrder_arg);
    m_Interpolator=m;
    if (m_Verbose) std::cout<<"Using Bspline interpolation..."<<std::endl;
    break;
  }

  case 3: {
    typename itk::BSplineInterpolateImageFunctionWithLUT< ImageType,TCoordRep >::Pointer m =itk::BSplineInterpolateImageFunctionWithLUT< ImageType,TCoordRep >::New();
    m->SetSplineOrder(m_ArgsInfo.interpOrder_arg);
    m->SetLUTSamplingFactor(m_ArgsInfo.interpSF_arg);
    m_Interpolator=m;
    if (m_Verbose) std::cout<<"Using BLUT interpolation..."<<std::endl;
    break;
  }

  case 4: {
    typename itk::QuantitativeInterpolateImageFunction< ImageType,TCoordRep >::Pointer m =itk::QuantitativeInterpolateImageFunction< ImageType,TCoordRep >::New();
    m_Interpolator=m;
    if (m_Verbose) std::cout<<"Using linear interpolation..."<<std::endl;
    break;
  }


  }//end of switch


  //============================================================================
  //return the pointer
  return m_Interpolator;
}

template <class args_info_type,  class ImageType, class TCoordRep>
void
GenericInterpolator<args_info_type, ImageType, TCoordRep>::SetOutputSpacing(SpacingType spacing)
{
  //m_Interpolator->SetOutputSpacing(spacing);
}

}

#endif
