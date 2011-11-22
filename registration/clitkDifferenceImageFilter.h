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
#ifndef __clitkDifferenceImageFilter_h
#define __clitkDifferenceImageFilter_h
#include "clitkCommon.h"
#include "clitkImageCommon.h"

//itk include
#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkImageRegionConstIterator.h"
#include "itkNumericTraits.h"

namespace clitk
{
  
  template <  class InputImageType , class OutputImageType>  
  class DifferenceImageFilter : public itk::ImageToImageFilter<InputImageType, OutputImageType>
  
  {
  public:
    typedef DifferenceImageFilter     Self;
    typedef itk::ImageToImageFilter<InputImageType,OutputImageType>     Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;

   
    /** Method for creation through the object factory. */
    itkNewMacro(Self);  
  
    /** Determine the image dimension. */
    itkStaticConstMacro(ImageDimension, unsigned int,
			InputImageType::ImageDimension );
  
    /** Inherit some types from the superclass. */
    typedef typename OutputImageType::RegionType OutputImageRegionType;
        
    //Set
    void SetValidInput(const typename InputImageType::Pointer input);
    void SetTestInput( const typename InputImageType::Pointer input);
  
  protected:
    DifferenceImageFilter();
    ~DifferenceImageFilter() {};
    void ThreadedGenerateData(const OutputImageRegionType& threadRegion, itk::ThreadIdType threadId);
    
       
  };





} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkDifferenceImageFilter.txx"
#endif

#endif // #define __clitkDifferenceImageFilter_h
