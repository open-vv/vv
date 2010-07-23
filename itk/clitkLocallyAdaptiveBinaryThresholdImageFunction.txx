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
#ifndef __clitkLocallyAdaptiveBinaryThresholdImageFunction_txx
#define __clitkLocallyAdaptiveBinaryThresholdImageFunction_txx
#include "clitkLocallyAdaptiveBinaryThresholdImageFunction.h"
#include "itkNumericTraits.h"
#include "itkConstNeighborhoodIterator.h"

namespace clitk
{

  /**
   * Constructor
   */
  template <class TInputImage, class TCoordRep>
  LocallyAdaptiveBinaryThresholdImageFunction<TInputImage,TCoordRep>
  ::LocallyAdaptiveBinaryThresholdImageFunction()
  {
    m_Radius.Fill(1);
    m_LowerBorderIsGiven=true;
    m_UpperBorderIsGiven=true;
    m_MaximumSDIsGiven=true;
    m_Multiplier=1.0;
    m_MaximumSD=100.0;
  }


  /**
   *
   */
  template <class TInputImage, class TCoordRep>
  void
  LocallyAdaptiveBinaryThresholdImageFunction<TInputImage,TCoordRep>
  ::PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    this->Superclass::PrintSelf(os,indent);

    os << indent << "Radius: " << m_Radius << std::endl;
  }


  /**
   *
   */
  template <class TInputImage, class TCoordRep>
  bool
  LocallyAdaptiveBinaryThresholdImageFunction<TInputImage,TCoordRep>
  ::EvaluateAtIndex(const IndexType& index) const
  {
  
    if( !this->GetInputImage() )
      {
	return ( false );
      }
  
    if ( !this->IsInsideBuffer( index ) )
      {
	return ( false );
      }

    // Create an N-d neighborhood kernel, using a zeroflux boundary condition
    itk::ConstNeighborhoodIterator<InputImageType>
      it(m_Radius, this->GetInputImage(), this->GetInputImage()->GetBufferedRegion());

    // Set the iterator at the desired location
    it.SetLocation(index);
    PixelType centerValue = it.GetPixel(0);
    PixelType currentvalue;
    bool isInside=true;

    // Walk the neighborhood for the mean and SD
    const unsigned int size = it.Size();
    typename   itk::NumericTraits<PixelType>::RealType mean=0;
    typename   itk::NumericTraits<PixelType>::RealType sd=0;
    for (unsigned int i = 1; i < size; ++i)
      {
	currentvalue=it.GetPixel(i);
	mean+=currentvalue;
	sd+=currentvalue*currentvalue;  
      }
    mean/=( typename itk::NumericTraits<PixelType>::RealType) size-1.;
    sd= sqrt( (sd /(typename itk::NumericTraits<PixelType>::RealType)size-1.) - (mean*mean) );

    // Verify fixed borders
    if (this->GetLower() > centerValue || centerValue > this->GetUpper())
      isInside = false;

    // Verify lower adaptive borders  
    if( (m_LowerBorderIsGiven)  &&  (centerValue <  (    mean - (typename  itk::NumericTraits<PixelType>::RealType) ( m_Multiplier*sd) )  )  ) 
      isInside = false;
  
    // Verify upper adaptive border
    if ( (m_UpperBorderIsGiven)  &&  (centerValue >  (    mean + (typename  itk::NumericTraits<PixelType>::RealType) ( m_Multiplier*sd) )  )  )  
      isInside = false;
    
    // Verify SD
    if ( (m_MaximumSDIsGiven)  &&  ( sd> m_MaximumSD) )
      isInside=false;
      
//     DD(centerValue);
//     DD(mean);
//     DD(sd);
//     DD(isInside);
    
    return ( isInside );
  }


} // end namespace itk

#endif
