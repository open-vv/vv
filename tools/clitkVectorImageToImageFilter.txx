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
#ifndef clitkVectorImageToImageFilter_txx
#define clitkVectorImageToImageFilter_txx

/* =================================================
 * @file   clitkVectorImageToImageFilter.txx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


namespace clitk
{

  //-------------------------------------------------------------------
  // Constructor
  //-------------------------------------------------------------------
  template<class InputImageType, class OutputImageType>
  VectorImageToImageFilter<InputImageType, OutputImageType>::VectorImageToImageFilter()
  {
   m_Verbose=false;
   m_ComponentIndex=0;
  }


  //-------------------------------------------------------------------
  // Generate Data
  //-------------------------------------------------------------------
  template<class InputImageType, class  OutputImageType> 
  void VectorImageToImageFilter<InputImageType, OutputImageType>::ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread, itk::ThreadIdType threadId)
  {
    // Iterators
    typename OutputImageType::Pointer output=this->GetOutput();
    typename InputImageType::ConstPointer input=this->GetInput();

    typedef itk::ImageRegionConstIterator<InputImageType> InputIteratorType;
    InputIteratorType inputIt (input, outputRegionForThread);

    typedef itk::ImageRegionIterator<OutputImageType> OutputIteratorType;
    OutputIteratorType outputIt (output, outputRegionForThread);
    
    while(! inputIt.IsAtEnd() )
      {
	outputIt.Set(inputIt.Get()[m_ComponentIndex]);
	++outputIt;
	++inputIt;
      }
  }


}//end clitk
 
#endif //#define clitkVectorImageToImageFilter_txx
