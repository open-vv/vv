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

#ifndef CLITKSEGMENTATIONFUNCTIONS_H
#define CLITKSEGMENTATIONFUNCTIONS_H

#include "clitkAutoCropFilter.h"

//--------------------------------------------------------------------
namespace clitk {

  //--------------------------------------------------------------------
  template<class TInternalImageType, class TMaskInternalImageType>
  typename TInternalImageType::Pointer
  SetBackground(typename TInternalImageType::ConstPointer input,
                typename TMaskInternalImageType::ConstPointer mask, 
                typename TMaskInternalImageType::PixelType maskBG, 
                typename TInternalImageType::PixelType outValue);
  //--------------------------------------------------------------------

    
  //--------------------------------------------------------------------
  template<class TInternalImageType, class TMaskInternalImageType>
  typename TInternalImageType::Pointer
  SetBackground(typename TInternalImageType::Pointer input, 
                typename TMaskInternalImageType::Pointer mask, 
                typename TMaskInternalImageType::PixelType maskBG, 
                typename TInternalImageType::PixelType outValue) {
    return SetBackground<TInternalImageType, TMaskInternalImageType>
      (static_cast<typename TInternalImageType::ConstPointer>(input),  
       static_cast<typename TMaskInternalImageType::ConstPointer>(mask), 
       maskBG, outValue);
  }
  //--------------------------------------------------------------------


  //-------------------------------------------------------------------- 
  template<class TImageType>
  typename TImageType::Pointer
  Labelize(typename TImageType::Pointer input, 
           typename TImageType::PixelType BG, 
           bool isFullyConnected, 
           int minimalComponentSize);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class TImageType>
  typename TImageType::Pointer
  RemoveLabels(typename TImageType::Pointer input, 
               typename TImageType::PixelType BG, 
               std::vector<typename TImageType::PixelType> & labelsToRemove);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
    typename ImageType::Pointer
    AutoCrop(typename ImageType::Pointer input, 
	     typename ImageType::PixelType BG) {
      typedef clitk::AutoCropFilter<ImageType> AutoCropFilterType;
      typename AutoCropFilterType::Pointer autoCropFilter = AutoCropFilterType::New();
      autoCropFilter->SetInput(input);
      autoCropFilter->Update();   
      return autoCropFilter->GetOutput();
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  template<class TImageType>
  typename TImageType::Pointer
  KeepLabels(typename TImageType::Pointer input,
             typename TImageType::PixelType BG, 
             typename TImageType::PixelType FG,  
             typename TImageType::PixelType firstKeep, 
             typename TImageType::PixelType lastKeep, 
             bool useLastKeep);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class TImageType>
  typename TImageType::Pointer
  LabelizeAndSelectLabels(typename TImageType::Pointer input,
                          typename TImageType::PixelType BG, 
                          typename TImageType::PixelType FG, 
                          bool isFullyConnected,
                          int minimalComponentSize,
                          LabelizeParameters<typename TImageType::PixelType> * param);
}
//--------------------------------------------------------------------

#include "clitkSegmentationFunctions.txx"
//--------------------------------------------------------------------
                      
#endif
