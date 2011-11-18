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
#ifndef clitkCombineImageFilter_txx
#define clitkCombineImageFilter_txx

/* =================================================
 * @file   clitkCombineImageFilter.txx
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
  CombineImageFilter<InputImageType>::CombineImageFilter()
  {
   m_Verbose=false;
  }


  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template<class InputImageType> 
  void 
  CombineImageFilter<InputImageType>::ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread, itk::ThreadIdType threadId)
  {
    typename InputImageType::ConstPointer input1=this->GetInput(0);
    typename InputImageType::ConstPointer input2=this->GetInput(1);
    typename InputImageType::Pointer output=this->GetOutput();

    typedef itk::ImageRegionIterator<InputImageType> IteratorType;
    typedef itk::ImageRegionConstIterator<InputImageType> ConstIteratorType;
    typedef itk::ImageRegionConstIterator<MaskImageType> MaskIteratorType;
    
    ConstIteratorType it1(input1, outputRegionForThread);
    ConstIteratorType it2(input2, outputRegionForThread);
    MaskIteratorType itM(m_Mask, outputRegionForThread);
    IteratorType itO(output, outputRegionForThread);

    it1.GoToBegin();
    it2.GoToBegin();
    itM.GoToBegin();
    itO.GoToBegin();

    while(! itO.IsAtEnd())
      {
	if(itM.Get())
	  itO.Set(it1.Get());
	else
	  itO.Set(it2.Get());
	
	++it1;
	++it2;
	++itM;
	++itO;
      }

 }


}//end clitk
 
#endif //#define clitkCombineImageFilter_txx
