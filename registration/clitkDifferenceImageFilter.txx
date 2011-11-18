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
#ifndef __clitkDifferenceImageFilter_txx
#define __clitkDifferenceImageFilter_txx
#include "clitkDifferenceImageFilter.h"


namespace clitk
{

  //=========================================================================================================================
  // DifferenceImageFilter
  //=========================================================================================================================

  //=========================================================================================================================
  //constructor
  template <class InputImageType, class OutputImageType> 
  DifferenceImageFilter<InputImageType, OutputImageType>::DifferenceImageFilter()
  {
    this->SetNumberOfRequiredInputs(2);
  }

  //=========================================================================================================================
  //set input
  template <class InputImageType, class OutputImageType> 
  void DifferenceImageFilter<InputImageType, OutputImageType>::SetValidInput(const typename InputImageType::Pointer input)
  {
    this->SetNthInput(0,input);
  }

  template <class InputImageType, class OutputImageType> 
  void DifferenceImageFilter<InputImageType, OutputImageType>::SetTestInput(const typename InputImageType::Pointer input)
  {
    this->SetNthInput(1,input);
  }


  //=========================================================================================================================
  //Update
  template <class InputImageType, class OutputImageType> 
  void DifferenceImageFilter<InputImageType, OutputImageType>::ThreadedGenerateData(const OutputImageRegionType &threadRegion, itk::ThreadIdType threadId)
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

    while (!outputIterator.IsAtEnd())
      {
	outputIterator.Set(input1Iterator.Get()-input2Iterator.Get());
	++input1Iterator;
	++input2Iterator;
	++outputIterator;	 
      }
  }
      
}


#endif
