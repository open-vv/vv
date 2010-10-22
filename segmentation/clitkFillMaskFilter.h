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

#ifndef CLITKFILLMASKFILTER_H
#define CLITKFILLMASKFILTER_H

// clitk 
#include "clitkCommon.h"

//itk 
#include "itkLightObject.h"
#include "itkJoinSeriesImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkPermuteAxesImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkCastImageFilter.h"

namespace clitk 
{
  
  //--------------------------------------------------------------------
  template <class TImageType>
  class ITK_EXPORT FillMaskFilter: 
    public itk::ImageToImageFilter<TImageType, TImageType > 
  {
  public:
    /** Standard class typedefs. */
    typedef itk::ImageToImageFilter<TImageType, TImageType> Superclass;
    typedef FillMaskFilter                 Self;
    typedef itk::SmartPointer<Self>        Pointer;
    typedef itk::SmartPointer<const Self>  ConstPointer;
    
    /** Method for creation through the object factory */
    itkNewMacro(Self);  

    /**  Run-time type information (and related methods)*/
    itkTypeMacro(FillMaskFilter, ImageToImageFilter);

    /** Some convenient typedefs */
    typedef TImageType                       ImageType;
    typedef typename ImageType::ConstPointer InputImageConstPointer;
    typedef typename ImageType::Pointer      InputImagePointer;
    typedef typename ImageType::RegionType   InputImageRegionType; 
    typedef typename ImageType::PixelType    InputImagePixelType; 
    typedef typename ImageType::SizeType     InputImageSizeType; 
    typedef typename ImageType::IndexType    InputImageIndexType; 
    typedef typename ImageType::PointType    InputImagePointType; 
    itkStaticConstMacro(ImageDimension, unsigned int, ImageType::ImageDimension);

    /** Options */
    void AddDirection(int dir);
    void ResetDirections();
    const std::vector<int> & GetDirections() const { return m_Directions; }

    // Set all options at a time
    template<class ArgsInfoType> void SetOptionsFromArgsInfo(ArgsInfoType & arg);
    template<class ArgsInfoType> void SetOptionsToArgsInfo(ArgsInfoType & arg);
 
  protected:
    FillMaskFilter();
    virtual ~FillMaskFilter() {};

    // Main members
    InputImageConstPointer input;
    std::vector<int> m_Directions;

   // Main function
    virtual void GenerateData();
    
   private:
    FillMaskFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkFillMaskFilter.txx"
#endif

#endif
