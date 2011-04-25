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
#ifndef clitkSelectPointsGenericFilter_cxx
#define clitkSelectPointsGenericFilter_cxx

/* =================================================
 * @file   clitkSelectPointsGenericFilter.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/

#include "clitkSelectPointsGenericFilter.h"


namespace clitk
{


  //-----------------------------------------------------------
  // Constructor
  //-----------------------------------------------------------
  SelectPointsGenericFilter::SelectPointsGenericFilter()
  {
    m_Verbose=false;
  }


  //-----------------------------------------------------------
  // Update
  //-----------------------------------------------------------
  void SelectPointsGenericFilter::Update()
  {
    //-----------------------------
    // Typedefs
    //-----------------------------
    typedef double ValueType;
    typedef std::vector<ValueType> MeasureListType;

    typedef itk::Point<double, 3> PointType;
    typedef clitk::List<PointType> PointListType;
    typedef clitk::Lists<PointType> PointListsType;

   
    //-----------------------------
    // Input point lists
    //-----------------------------   
    PointListsType pointLists;
    unsigned int numberOfPoints=0;
    unsigned int numberOfLists=m_ArgsInfo.input_given;
    for (unsigned int i=0; i<numberOfLists; i++)
      {
	// Read the lists
	pointLists.push_back(PointListType(m_ArgsInfo.input_arg[i], m_Verbose) );

	// Verify the number of points
	if (i==0) numberOfPoints=pointLists[i].size();
	else 
	  {
	    if  (numberOfPoints!=pointLists[i].size())
	      {
		std::cerr<<"Size of first list ("<<numberOfPoints
			 <<") is different from size of list "<<i
			 <<" ("<<pointLists[i].size()<<")..."<<std::endl;
		return;
	      }
	  }
      }    
    
    //-----------------------------
    // Reference point list
    //-----------------------------
    PointListType referencePointList;
    if (m_Verbose) std::cout<<"Reference point list:"<<std::endl;
    referencePointList=PointListType(m_ArgsInfo.ref_arg, m_Verbose);
    if  (numberOfPoints!=referencePointList.size())
      {
	std::cerr<<"Size of the first list ("<<numberOfPoints
		 <<") is different from size of the reference list ("
		 << referencePointList.size() <<")..."<<std::endl;
	return;
      }

    
    //-----------------------------
    // Select
    //-----------------------------
    PointListType sReferencePointList;
    PointListsType sPointLists(numberOfLists);
    unsigned int counter=0;
    for (unsigned int number=0; number< referencePointList.size(); number++)
      {
	for (unsigned int component=0; component< m_ArgsInfo.component_given; component++)
	  {
	    if (  (referencePointList[number][m_ArgsInfo.component_arg[component]] >= m_ArgsInfo.lower_arg[component])
		  &&  (referencePointList[number][m_ArgsInfo.component_arg[component]] <= m_ArgsInfo.upper_arg[component]) )
	      {
		if(m_Verbose) std::cout<<"Selecting point "<<number<<": "<<referencePointList[number]<<std::endl;
		sReferencePointList.push_back(referencePointList[number]);
		for (unsigned int i=0;i<pointLists.size();i++)
		  sPointLists[i].push_back(pointLists[i][number]);
		counter++;
	      }
	  }
      }
    if (m_Verbose) std::cout<<"Selected "<<counter<<" points..."<<std::endl;
    

    
    //-----------------------------
    // Output
    //-----------------------------
    sReferencePointList.Write(m_ArgsInfo.sRef_arg,m_Verbose);
    std::vector<std::string> filenames;
    for (unsigned int i=0;i<pointLists.size();i++)
      filenames.push_back(m_ArgsInfo.sInput_arg[i]);
    sPointLists.Write(filenames,m_Verbose);

  }


} //end clitk

#endif  //#define clitkSelectPointsGenericFilter_cxx
