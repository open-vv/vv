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
#ifndef __clitkDeformationFieldStatisticsFilter_txx
#define __clitkDeformationFieldStatisticsFilter_txx
#include "clitkDeformationListStatisticsFilter.h"


namespace clitk
{

  //------------------------------------------------------
  //Magnitude
  //------------------------------------------------------
  template <class ListItemType> 
  void 
  DeformationListStatisticsFilter<ListItemType>::GetStatistics(const ListType& list, ValueType & mean,  ValueType & sd,  ValueType & max)
  {
    // Initialize
    ValueType norm;
    mean=0;
    sd=0;
    max=0;

    // loop over the list of displacements
    for (unsigned int i=0;i<list.size();i++)
      {
	norm=list[i].GetNorm();
	mean+=norm;
	sd+=norm*norm;
	if (max<norm)max=norm;
      }
	
    // Normalize
    mean/=list.size();
    sd/=list.size();
    sd-=mean*mean;
    sd=sqrt(sd);
    
  }



  //------------------------------------------------------
  // For multiple lists	
  //------------------------------------------------------
  template <class ListItemType> 
  void 
  DeformationListStatisticsFilter<ListItemType>::GetStatistics(const ListsType& list, ValueListType & mean,  ValueListType & sd,  ValueListType & max)
  {

    mean.resize(list.size());
    sd.resize(list.size());
    max.resize(list.size());

    for (unsigned int i=0;i<list.size();i++)
      {
	// call function per list
	GetStatistics(list[i], mean[i], sd[i], max[i]);
      }
  }


  //------------------------------------------------------
  // Magnitude AND General statistics for magnitude	
  //------------------------------------------------------
  template <class ListItemType> 
  void 
  DeformationListStatisticsFilter<ListItemType>::GetStatistics(const ListsType& list, ValueType&  mean, ValueType &sd, ValueType & max,  ValueListType & meanList, ValueListType & sdList,  ValueListType & maxList)
  {
    // Calculate statistics per list
    GetStatistics(list, meanList, sdList, maxList);

    // Initialize
    mean=0;
    sd=0;
    max=0;
    
    // Loop
    for (unsigned int i=0;i<list.size();i++)
      {
	mean+=meanList[i];
	sd+=sdList[i]*sdList[i];
	if(max<maxList[i])max=maxList[i];
      }

    // Normalize
    mean/=list.size();
    sd/=list.size();
    sd=sqrt(sd);
  }


  //------------------------------------------------------
  // Per component	
  //------------------------------------------------------
  template <class ListItemType> 
  void 
  DeformationListStatisticsFilter<ListItemType>::GetStatistics(const ListType& list, ListItemType & mean,  ListItemType & sd,  ListItemType & max)
  {
    // Initialize
    ListItemType displacement;
    mean.Fill(itk::NumericTraits<ValueType>::Zero);
    sd.Fill(itk::NumericTraits<ValueType>::Zero);
    max.Fill(itk::NumericTraits<ValueType>::Zero);

    // Loop over the list of displacements
    for (unsigned int i=0;i<list.size();i++)
      {
	displacement=list[i];
	mean+=displacement;
	for (unsigned int dim=0; dim<list[0].Size(); dim++)
	  {
	    sd[dim]+=displacement[dim]*displacement[dim];
	    if (fabs(max[dim])<fabs(displacement[dim])) max[dim]=displacement[dim];
	  }
      }
	
    // Normalize
    for (unsigned int dim=0; dim<list[0].Size(); dim++)
      {
	mean[dim]/=list.size();
	sd[dim]/=list.size();
	sd[dim]-=mean[dim]*mean[dim];
	sd[dim]=sqrt(sd[dim]);
      }
  }

  //------------------------------------------------------
  // For multiple lists	
  //------------------------------------------------------
  template <class ListItemType> 
  void 
  DeformationListStatisticsFilter<ListItemType>::GetStatistics(const ListsType& list, ListType & mean,  ListType & sd,  ListType & max)
  {
    mean.resize(list.size());
    sd.resize(list.size());
    max.resize(list.size());

    for (unsigned int i=0;i<list.size();i++)
      {
	// call function per list
	GetStatistics(list[i], mean[i], sd[i], max[i]);
      }
  }



  //------------------------------------------------------
  // Magnitude AND General statistics per component	
  //------------------------------------------------------
  template <class ListItemType> 
  void 
  DeformationListStatisticsFilter<ListItemType>::GetStatistics(const ListsType& list, ListItemType&  mean, ListItemType &sd, ListItemType & max,  ListType & meanList, ListType & sdList,  ListType & maxList)
  {
    // Calculate statistics par list
    GetStatistics(list, meanList, sdList, maxList);

    // Initialize
    mean.Fill(itk::NumericTraits<ValueType>::Zero);
    sd.Fill(itk::NumericTraits<ValueType>::Zero);
    max.Fill(itk::NumericTraits<ValueType>::Zero);
    ListItemType displacement;

    // Loop
    for (unsigned int i=0;i<list.size();i++)
      {

	mean +=meanList[i];
	
	for (unsigned int dim=0; dim<mean.Size(); dim++)
	  {
	     sd[dim]+=sdList[i][dim]*sdList[i][dim];
	     if (fabs(max[dim])<fabs(maxList[i][dim])) max[dim]=maxList[i][dim];
	  }
      }

    // Normalize
    for (unsigned int dim=0; dim<mean.Size(); dim++)
      {
	mean[dim]/=list.size();
	sd[dim]/=list.size();
	//sd[dim]-=mean[dim]*mean[dim];
	sd[dim]=sqrt(sd[dim]);
      }
  }


}

#endif
