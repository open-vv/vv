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
#ifndef __clitkLinearCombinationImageFilter_txx
#define __clitkLinearCombinationImageFilter_txx
#include "clitkLinearCombinationImageFilter.h"


namespace clitk
{

  //=========================================================================================================================
  // LinearCombinationImageFilter
  //=========================================================================================================================

  //=========================================================================================================================
  //constructor
  template <class InputImageType, class OutputImageType> 
  LinearCombinationImageFilter<InputImageType, OutputImageType>
  ::LinearCombinationImageFilter()
  {
    this->SetNumberOfRequiredInputs(2);
    m_A=1.;
    m_B=1.;
    m_C=0.;
    
  }

  //=========================================================================================================================
  //set input
  template <class InputImageType, class OutputImageType> 
  void LinearCombinationImageFilter<InputImageType, OutputImageType>::SetFirstInput(const typename InputImageType::Pointer input)
  {
    this->SetInput(0,input);
  }
  template <class InputImageType, class OutputImageType> 
  void LinearCombinationImageFilter<InputImageType, OutputImageType>::SetSecondInput(const typename InputImageType::Pointer input)
  {
    this->SetInput(1,input);
  }


  template <class InputImageType, class OutputImageType> 
  void LinearCombinationImageFilter<InputImageType, OutputImageType>::SetFirstInput(const typename InputImageType::ConstPointer input)
  {
    this->SetInput(0,input);
  }
  template <class InputImageType, class OutputImageType> 
  void LinearCombinationImageFilter<InputImageType, OutputImageType>::SetSecondInput(const typename InputImageType::ConstPointer input)
  {
    this->SetInput(1,input);
  }


  //=========================================================================================================================
  //Update
  template <class InputImageType, class OutputImageType> 
  void 
  LinearCombinationImageFilter<InputImageType, OutputImageType>
  ::ThreadedGenerateData(const OutputImageRegionType &threadRegion, itk::ThreadIdType threadId)
  {
    //Pointers to input and output
    typename InputImageType::ConstPointer input1=this->GetInput(0);
    typename InputImageType::ConstPointer input2=this->GetInput(1);
    typename OutputImageType::Pointer output=this->GetOutput();

    //Iterator over input1, input0 and output
    typedef itk::ImageRegionConstIterator<InputImageType> InputIteratortype;
    typedef itk::ImageRegionIterator<OutputImageType> OutputIteratortype;
    InputIteratortype input1Iterator (input1, threadRegion);
    InputIteratortype input2Iterator (input2, threadRegion);
    OutputIteratortype outputIterator (output, threadRegion);
    input1Iterator.GoToBegin();
    input2Iterator.GoToBegin();
    outputIterator.GoToBegin();

    //Find which coefficient should be applied

    if (m_C==0.)
      {
	if (m_B==1.)
	  {
	    if (m_A==1.)
	      {
		while (!outputIterator.IsAtEnd())
		  {
		    // outputIterator.Set(static_cast<OutputPixelType>(input1Iterator.Get()+input2Iterator.Get()));
		    outputIterator.Set(input1Iterator.Get()+input2Iterator.Get());
		    ++input1Iterator;
		    ++input2Iterator;
		    ++outputIterator;	 
		  }
	      }
	    else 
	      {
		while (!outputIterator.IsAtEnd())
		  {
		    // outputIterator.Set(static_cast<OutputPixelType>(m_A* input1Iterator.Get()+input2Iterator.Get()));
		    outputIterator.Set(m_A* input1Iterator.Get()+input2Iterator.Get());
		    ++input1Iterator;
		    ++input2Iterator;
		    ++outputIterator;	 
		  }
	      }
	  }
	// case B not 1
	else
	  {
	    if (m_A==1.)
	      {
		
		while (!outputIterator.IsAtEnd())
		  {
		    //outputIterator.Set(static_cast<OutputPixelType>(input1Iterator.Get()+m_B *input2Iterator.Get()));
		    outputIterator.Set(input1Iterator.Get()+m_B *input2Iterator.Get());
		    ++input1Iterator;
		    ++input2Iterator;
		    ++outputIterator;	 
		  }
	      }
	    else 
	      {
		while (!outputIterator.IsAtEnd())
		  {
		    //outputIterator.Set(static_cast<OutputPixelType>(m_A* input1Iterator.Get()+ m_B*input2Iterator.Get()));
		    outputIterator.Set(m_A* input1Iterator.Get()+ m_B*input2Iterator.Get());
		    ++input1Iterator;
		    ++input2Iterator;
		    ++outputIterator;	 
		  }
	      }
	  }

      }

    //Case C not 0
    else
      {
	if (m_B==1.)
	  {
	    if (m_A==1.)
	      {
		
		while (!outputIterator.IsAtEnd())
		  {
		    outputIterator.Set(static_cast<OutputPixelType>(input1Iterator.Get()+input2Iterator.Get())+ m_C);
		    ++input1Iterator;
		    ++input2Iterator;
		    ++outputIterator;	 
		  }
	      }
	    else 
	      {
		while (!outputIterator.IsAtEnd())
		  {
		    outputIterator.Set(static_cast<OutputPixelType>(m_A* input1Iterator.Get()+input2Iterator.Get()+ m_C) );
		    ++input1Iterator;
		    ++input2Iterator;
		    ++outputIterator;	 
		  }
	      }
	  }
	// case B not 1
	else
	  {
	    if (m_A==1.)
	      {
	      
		while (!outputIterator.IsAtEnd())
		  {
		    outputIterator.Set(static_cast<OutputPixelType>(input1Iterator.Get()+m_B *input2Iterator.Get()+ m_C) );
		    ++input1Iterator;
		    ++input2Iterator;
		    ++outputIterator;	 
		  }
	      }
	    else 
	      {
		while (!outputIterator.IsAtEnd())
		  {
		    outputIterator.Set(static_cast<OutputPixelType>(m_A* input1Iterator.Get()+ m_B*input2Iterator.Get()+ m_C) );
		    ++input1Iterator;
		    ++input2Iterator;
		    ++outputIterator;	 
		  }
	      }
	  }
      }

  }

}// end of namespace clitk

  

#endif
