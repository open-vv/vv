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
#ifndef __clitkDeformationFieldTransform_txx
#define __clitkDeformationFieldTransform_txx
#include "clitkDeformationFieldTransform.h"


namespace clitk
{

  // Constructor
  template<class TScalarType, unsigned int InputDimension, unsigned int OutputDimension, unsigned int SpaceDimension>
  DeformationFieldTransform<TScalarType, InputDimension, OutputDimension, SpaceDimension>
  ::DeformationFieldTransform():Superclass(OutputDimension,1)
  {
     m_DeformationField=NULL;
     m_Interpolator=DefaultInterpolatorType::New();
  }
    
   // Transform a point
  template<class TScalarType, unsigned int InputDimension,unsigned int OutputDimension, unsigned int SpaceDimension>
  typename DeformationFieldTransform<TScalarType,  InputDimension, OutputDimension, SpaceDimension>::OutputPointType
  DeformationFieldTransform<TScalarType, InputDimension, OutputDimension, SpaceDimension>::
  TransformPoint(const InputPointType &inputPoint) const 
  {
    OutputPointType outputPoint;
    outputPoint.Fill(0.);
    for (unsigned int i=0;i<min(OutputDimension,InputDimension); i++) outputPoint[i]=inputPoint[i];
    DisplacementType displacement;
    if (m_Interpolator->IsInsideBuffer(inputPoint) )
      {
	displacement=m_Interpolator->Evaluate(inputPoint);
	for (unsigned int i=0;i<min(OutputDimension,SpaceDimension); i++) 
	  outputPoint[i]+=displacement[i];
	return outputPoint;
      }

    // No displacement
    else return outputPoint;
	
  }

  
} // namespace clitk

#endif
