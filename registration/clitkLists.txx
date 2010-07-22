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
#ifndef clitkLists_txx
#define clitkLists_txx
#include "clitkLists.h"

using namespace std; 

namespace clitk 
{
  
  //---------------------------------------------------------------------
  // Read
  //---------------------------------------------------------------------
  template<class ListItemType>
  void Lists< ListItemType>::Read(const std::vector<string>& filenames, bool verbose)
  {

    // Make the lists
    this->resize(filenames.size());

    // Read the lists   
    for (unsigned int i=0; i<filenames.size();i++)
      this->at(i).Read(filenames[i],verbose);
    
  }

  //---------------------------------------------------------------------
  // Norm
  //---------------------------------------------------------------------
  template<class ListItemType>
  Lists<itk::FixedArray<double,1> > Lists<ListItemType>::Norm(void)
  {
    Lists<itk::FixedArray<double,1> > norm;

    // Normalize the lists   
    for (unsigned int i=0; i<this->size();i++)
      norm.push_back(this->at(i).Norm());
  
    return norm;
  }

  //---------------------------------------------------------------------
  // ReadPointPairs (specific for DARS- IX point list format)
  //---------------------------------------------------------------------
  template<class ListItemType>
  void 
  Lists<ListItemType>::ReadAndConvertPointPairs(const std::vector<std::string>& fileNames, ListType& refList, const typename ImageType::Pointer referenceImage, bool verbose)
  {

    // Init
    this->resize(fileNames.size());
    refList.resize(0);
    bool pointWasFound=true;
    unsigned int pointNumber=0;
    itk::ContinuousIndex<ValueType, Dimension> item1;
    std::vector<itk::ContinuousIndex<ValueType, Dimension> > item2(fileNames.size());
    itk::Point<ValueType, Dimension> point1, point2;
    unsigned int totalNumberOfPoints=0;
    std::vector<unsigned int> addedNumberOfPoints,ommittedNumberOfPoints;
    std::vector<bool> pointIsUnsure(fileNames.size());
    std::vector<std::vector<bool> > ommittedNumberOfPointsUnsure;
    
    // Loop over the points
    while (pointWasFound)
      {

	if (verbose) std::cout<<std::endl<<"Processing point number: "<<pointNumber<<"..."<<std::endl;
	if (verbose) std::cout<<"Point number: "<<pointNumber<<" was found in list number: ";
	
	// Loop over lists
	bool veryUnsure=false;
	for (unsigned int i =0; i< fileNames.size(); i++)
	  {
	    // Get the point
	    pointWasFound=this->at(i).ReadPointPair(fileNames[i],pointNumber,item1,item2[i],veryUnsure);
	    if (verbose && pointWasFound) std::cout<<" "<<i;
	   	    
	    // Set unsure
	    pointIsUnsure[i]= veryUnsure;

	    // point not found
	    if(!pointWasFound) break;
	  }
	if (verbose) std::cout<<std::endl;

	// The point was found
	if (pointWasFound)
	  {
	    totalNumberOfPoints++;

	    // Point unsure?
	    bool unsure=false;
	    for (unsigned int i =0; i< fileNames.size(); i++)
	      if (pointIsUnsure[i])
		{
		  unsure=true;
		  break;
		}

	    // At least one of the lists had it tagged as veryUnsure
	    if (unsure)
	      {
		ommittedNumberOfPoints.push_back(pointNumber);
		ommittedNumberOfPointsUnsure.push_back(pointIsUnsure);
		if (verbose) std::cout<<"Omitting item "<<pointNumber<<" (very unsure): "<<item1<<" ; "<<item2[0]<<std::endl;
	      }
	    else 
	      {
		addedNumberOfPoints.push_back(pointNumber);
		// Add the point to all the lists 
		for (unsigned int i =0; i< fileNames.size(); i++)
		  {
		    if (verbose) std::cout<<"Adding item "<<pointNumber<<": "<<item1<<" ; "<<item2[i]<<std::endl;
		    if (i==0)
		      {
			referenceImage->TransformContinuousIndexToPhysicalPoint(item1, point1);
			refList.push_back(point1);
		      }
		    referenceImage->TransformContinuousIndexToPhysicalPoint(item2[i], point2);
		    if (verbose) std::cout<<"Converted items "<<pointNumber<<" to points: "<<point1<<" ; "<<point2<<std::endl;
		    this->at(i).push_back(point2);
		  }
	      }
	  }
	
	// Next point 
	pointNumber++;

      }

    if (verbose)
      {
	std::cout<<std::endl<<totalNumberOfPoints<<" points processed, "<< ommittedNumberOfPoints.size() <<" points omitted, "<< addedNumberOfPoints.size()<<" points added."<<std::endl;
	std::cout<<"Added point numbers: "<< std::endl;
	for (unsigned int i=0; i< addedNumberOfPoints.size();i++)
	  std::cout<<addedNumberOfPoints[i] <<std::endl;
	std::cout<<"Omitted point numbers: "<< std::endl;
	    for (unsigned int i=0; i< ommittedNumberOfPoints.size();i++)
	      {
		std::cout<<ommittedNumberOfPoints[i] <<"\t";
		for (unsigned int j=0; j< fileNames.size();j++)
		  std::cout<<ommittedNumberOfPointsUnsure[i][j]<<" ";
		std::cout<<std::endl;
	      }
      }
  }

    


  //---------------------------------------------------------------------
  // Write
  //---------------------------------------------------------------------
  template<class ListItemType>
  void Lists<ListItemType>::Write(const std::vector<std::string> fileNames, const bool verbose)
  {
    for (unsigned int i =0; i< fileNames.size(); i++)
      this->at(i).Write(fileNames[i], verbose);
  }


} // namespace 

#endif
