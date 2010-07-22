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
#ifndef clitkMultiResolutionPyramidRegionFilter_h
#define clitkMultiResolutionPyramidRegionFilter_h

/* =================================================
 * @file   clitkMultiResolutionPyramidRegionFilter.h
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk include
#include "clitkIO.h"
#include "clitkCommon.h"

//itk include
#include "itkLightObject.h"

namespace clitk 
{

  template <class InputImageType>
  class ITK_EXPORT MultiResolutionPyramidRegionFilter :
    public itk::LightObject
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef MultiResolutionPyramidRegionFilter                                                 Self;
    typedef itk::LightObject                                          Superclass;
    typedef itk::SmartPointer<Self>                                   Pointer;
    typedef itk::SmartPointer<const Self>                             ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( MultiResolutionPyramidRegionFilter, LightObject );

    /** Dimension of the domain space. */
    itkStaticConstMacro(InputImageDimension, unsigned int, InputImageType::ImageDimension);


    //----------------------------------------
    // Typedefs
    //----------------------------------------
    typedef itk::ImageRegion<InputImageDimension> RegionType;
    typedef itk::Array2D<unsigned int>  ScheduleType;
    typedef typename InputImageType::SizeType SizeType;
    typedef typename InputImageType::IndexType IndexType;

    //----------------------------------------
    // Set & Get
    //----------------------------------------    
    void SetRegion(RegionType r){m_Region=r;}
    void SetSchedule(ScheduleType r){m_Schedule=r;}

    //----------------------------------------  
    // Update
    //----------------------------------------  
    void Update(void);
    RegionType GetOutput(unsigned int i)
    {
      this->Update();
      return m_RegionPyramid[i];
    }
      

  protected:

    //----------------------------------------  
    // Constructor & Destructor
    //----------------------------------------  
    MultiResolutionPyramidRegionFilter();
    ~MultiResolutionPyramidRegionFilter() {};

    //----------------------------------------  
    // Data members
    //----------------------------------------
    RegionType m_Region;
    std::vector<RegionType>  m_RegionPyramid;
    ScheduleType m_Schedule;

  };


} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkMultiResolutionPyramidRegionFilter.txx"
#endif

#endif // #define clitkMultiResolutionPyramidRegionFilter_h


