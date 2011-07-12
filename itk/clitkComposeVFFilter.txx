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
#ifndef __clitkComposeVFFilter_txx
#define __clitkComposeVFFilter_txx
#include "clitkComposeVFFilter.h"


namespace clitk
{

  //=========================================================================================================================
  //constructor
  template <class InputImageType, class OutputImageType> 
  ComposeVFFilter<InputImageType, OutputImageType>::ComposeVFFilter()
  {
    for (unsigned int i=0; i<ImageDimension; i++) m_EdgePaddingValue[i]=0.0; //no other reasonable value?
    m_Verbose=false;
  }


  //=========================================================================================================================
  //update the output for the outputRegionForThread
  template<class InputImageType, class OutputImageType> 
  void ComposeVFFilter<InputImageType, OutputImageType>::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId )
  {
 
    //Get pointer to the output
    typename OutputImageType::Pointer outputPtr = this->GetOutput();

    //Iterator over output
    typedef itk::ImageRegionConstIteratorWithIndex<InputImageType> Input1ImageIteratorType;
    typedef itk::ImageRegionIterator<OutputImageType> OutputImageIteratorType;
 
    //define the output and input1 iterator over the outputRegionForThread
    OutputImageIteratorType outputIt(outputPtr, outputRegionForThread);
    Input1ImageIteratorType input1It(m_Input1, outputRegionForThread);
    
    //Initialize
    typename InputImageType::IndexType index;
    itk::ContinuousIndex<double,ImageDimension> contIndex;
    typename InputImageType::PointType point1;
    typename InputImageType::PointType point2;
    typedef typename OutputImageType::PixelType DisplacementType;
    DisplacementType displacement;
    DisplacementType totalDisplacement;
    input1It.GoToBegin();

    //define some temp variables outside the loop
    signed long baseIndex[ImageDimension];
    //    long tIndex;
    double distance[ImageDimension];
    unsigned int dim;
    double totalOverlap;
    double overlap;
    unsigned int upper;
    unsigned int counter;
    typename OutputImageType::IndexType neighIndex;


    //Find the number of neighbors
    unsigned int neighbors =  1 << ImageDimension;
     
    //==================================================================================================
    //1. loop over the region of the deformationfield and compose the displacements
    while( !input1It.IsAtEnd() )
      {
	// get the input image index
	index = input1It.GetIndex();
	m_Input1->TransformIndexToPhysicalPoint( index, point1 );

	// get the displacement of input 1
	displacement = input1It.Get();
	
	// compute the required image point in input2
	for(unsigned int j = 0; j < ImageDimension; j++ ) point2[j] = point1[j] + (double)displacement[j];

	//JV TODO add something for the borders
	//true if inside image
	if(m_Input2->TransformPhysicalPointToContinuousIndex(point2, contIndex ) )
	  {
	    
	    for(dim = 0; dim < ImageDimension; dim++)
	      {
		// The following  block is equivalent to the following line without
		// having to call floor. (Only for positive inputs, we already now that is in the image)
		// baseIndex[dim] = (long) vcl_floor(contIndex[dim] );
	
		baseIndex[dim] = (long) contIndex[dim];
		distance[dim] = contIndex[dim] - double( baseIndex[dim] );
		
		//Reset the accumulator
		totalDisplacement[dim]=0.0;	    
	      }

	    //Add contribution for each neighbor
	    totalOverlap = itk::NumericTraits<double>::Zero;
	    for( counter = 0; counter < neighbors ; counter++ )
	      {		
		overlap = 1.0;     // fraction overlap
		upper = counter;  // each bit indicates upper/lower neighbour
		
		
		// get neighbor index and overlap fraction
		for( dim = 0; dim < ImageDimension; dim++ )
		  {
		    if ( upper & 1 )
		      {
			neighIndex[dim] = baseIndex[dim] + 1;
			overlap *= distance[dim];
		      }
		    else
		      {
			neighIndex[dim] = baseIndex[dim];
			overlap *= 1.0 - distance[dim];
		      }
		    upper >>= 1;
		  }

		//JV shouldn't we verify that the index is not over the upper border instead of zero?
		// Set neighbor value only if overlap is not zero and index is still in image
		if( overlap>0.0 && neighIndex[0]>=0 && neighIndex[1]>=0 && neighIndex[2]>=0 )
		  {
		    //what to store? the weighted displacement vector of Input2? 
		    totalDisplacement+=m_Input2->GetPixel(neighIndex)*overlap;
		    //JV don't we loose more by adding and verifyning each time, it will never be 1.0 before the end no?
		    totalOverlap += overlap;
		  }
		
		if( totalOverlap == 1.0 )
		  {
		    // finished
		    break;
		  }
	      }
	    //add the displacement of input1 and the interpolated displacement of input2 to the output
	    outputIt.Set(static_cast<PixelType>(input1It.Get())+totalDisplacement);
	  }
	//displacement from input1 goes outside input2
	else outputIt.Set(m_EdgePaddingValue);
 	
	++input1It;
	++outputIt;
      }
  }
  
}

#endif
