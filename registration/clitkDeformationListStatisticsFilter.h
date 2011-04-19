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
#ifndef __clitkDeformationListStatistics_h
#define __clitkDeformationListStatistics_h
#include "clitkImageCommon.h"
#include "clitkList.h"
#include "clitkLists.h"

//itk include
#include "itkLightObject.h"
#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkNumericTraits.h"
#include "itkSimpleFastMutexLock.h"
#include "itkImageMaskSpatialObject.h"

namespace clitk
{
  
  template < class ListItemType >  
  class ITK_EXPORT DeformationListStatisticsFilter : public itk::LightObject
  
  {
  public:
    typedef DeformationListStatisticsFilter     Self;
    typedef itk::LightObject    Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;

   
    /** Method for creation through the object factory. */
    itkNewMacro(Self);  
    
     //====================================
    //Typedefs
    typedef typename ListItemType::ValueType ValueType;
    itkStaticConstMacro(Dimension, unsigned int, ListItemType::Dimension);
    //typedef itk::Vector<ValueType, Dimension> ListItemType;
    typedef std::vector<ValueType> ValueListType;
    typedef clitk::List<ListItemType> ListType;
    typedef clitk::Lists<ListItemType> ListsType;  

    //====================================
    //Get
    void GetStatistics( const ListType &,  ValueType & mean, ValueType & sd, ValueType & max);
    void GetStatistics( const ListsType &,  ValueListType & mean, ValueListType & sd, ValueListType & max);
    void GetStatistics( const ListsType &,  ValueType & mean , ValueType & sd, ValueType & max, ValueListType & meanList, ValueListType & sdList, ValueListType & maxList);
 
    void GetStatistics( const ListType &, ListItemType & meanXYZ, ListItemType & sdXYZ, ListItemType & maxXYZ );
    void GetStatistics( const ListsType &, ListType & meanXYZ, ListType & sdXYZ, ListType & maxXYZ );
    void GetStatistics( const ListsType &, ListItemType & mean , ListItemType & sd, ListItemType & max, ListType & meanXYZList, ListType & sdXYZList, ListType & maxXYZList );
    
  protected:
    DeformationListStatisticsFilter(){};
    ~DeformationListStatisticsFilter() {};
    
  private:

  };





} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkDeformationListStatisticsFilter.txx"
#endif

#endif // #define __clitkDeformationListStatistics_h
