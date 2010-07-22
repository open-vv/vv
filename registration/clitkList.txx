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
#ifndef CLITKLIST_TXX
#define CLITKLIST_TXX
#include "clitkList.h"

using namespace std; 

namespace clitk 
{
  

  //---------------------------------------------------------------------
  // Read
  //---------------------------------------------------------------------
  template<class ListItemType>
  void List< ListItemType>::Read(const string& filename, bool verbose)
  {
    std::ifstream is;
    openFileForReading(is, filename);

    this->resize(0);
    ListItemType item;
    ValueType x;
   
    if(verbose)std::cout<<"Reading "<<filename<<"..."<<std::endl;
    for (unsigned int dim=0; dim<Dimension; dim++)
      {
	skipComment(is);
	is >> x;
	item[dim]=x;
      }
   
    while (!is.eof()) 
      {
	this->push_back(item);
	for (unsigned int dim=0; dim<Dimension; dim++)
	  {
	    skipComment(is);
	    is >> x;
	    item[dim]=x;
	  }
      }
  }


  //---------------------------------------------------------------------
  // ReadPointPairs (specific for DARS- IX point list format)
  //---------------------------------------------------------------------
  template<class ListItemType>
  void 
  List<ListItemType>::ReadAndConvertPointPairs(const std::string& fileName, List& correspList, const typename ImageType::Pointer referenceImage, bool verbose)
  {
    // Init
    this->resize(0);
    correspList.resize(0);

    // Tags
    IndexType item1, item2;
    itk::Point<ValueType, Dimension> point1,point2;
    unsigned int pointNumber=0;
    bool veryUnsure=false;

    // Read first point
    bool pointWasFound=ReadPointPair(fileName, pointNumber, item1, item2, veryUnsure);

    // Loop over all points 
    while (pointWasFound)
      {
	// Store the values
	if(veryUnsure)
	  {
	    if (verbose) std::cout<<"Omitting item "<<pointNumber<<" (very unsure): "<<item1<<" ; "<<item2<<std::endl;
	  }
	else
	  {
	    if (verbose) std::cout<<"Adding item "<<pointNumber<<": "<<item1<<" ; "<<item2<<std::endl;
	    referenceImage->TransformContinuousIndexToPhysicalPoint(item1, point1);
	    referenceImage->TransformContinuousIndexToPhysicalPoint(item2, point2);
	    if (verbose) std::cout<<"Converted items "<<pointNumber<<" to points: "<<point1<<" ; "<<point2<<std::endl;
	    this->push_back(point1);
	    correspList.push_back(point2);
	  }

	// Next point
	pointNumber++;
	pointWasFound=ReadPointPair(fileName, pointNumber, item1, item2, veryUnsure);
      }
  }
  
  //---------------------------------------------------------------------
  // Read one point
  //---------------------------------------------------------------------
  template<class ListItemType>
  bool
  List< ListItemType>::ReadPointPair(const std::string& fileName, const unsigned int& pointNumber, IndexType& item1, IndexType& item2, bool& veryUnsure)
  {
    // bool
    bool pointWasFound=false;
    veryUnsure=false;
    item1.Fill(0.0);
    item2.Fill(0.0);
    
    // Open the file
    ifstream listStream(fileName.c_str());
    string line;
    if(!listStream.is_open()){
      std::cerr << "ERROR: Couldn't open file " << fileName << " in List::Read" << std::endl;
      return false;
    }
    
    // Skip the configuration lines
    while ( !listStream.eof())
      {
	// Get the line
	skipComment(listStream);
	getline(listStream, line);
	
	// Get the line type
	unsigned int endIndexItemType= line.find_first_of ("_",0)-1;
	string typeString= line.substr(0 , endIndexItemType+1);
	
	// Check
	if  (typeString=="Point")
	  break;
      }

    // Get the point number
    unsigned int beginIndexItemNumber= line.find_first_of ("_",0)+1; 
    unsigned int endIndexItemNumber= line.find_first_of ("->",0)-1;
    stringstream numberString( line.substr(beginIndexItemNumber , endIndexItemNumber-beginIndexItemNumber+1) );
    unsigned int itemNumber;
    numberString>> itemNumber;

    // FF to the pointNumber
    while ( (itemNumber != pointNumber) && (!listStream.eof()) )
      {
	// Skipcomment
	skipComment(listStream);
	
	// Get Line
	getline(listStream, line);
	
	// Get the point number
	beginIndexItemNumber= line.find_first_of ("_",0)+1; 
	endIndexItemNumber= line.find_first_of ("->",0)-1;
	stringstream numberString( line.substr(beginIndexItemNumber , endIndexItemNumber-beginIndexItemNumber+1) );
	numberString>> itemNumber;
      }
    
    // Go over all fields of the pointNumber
    while (itemNumber == pointNumber && (!listStream.eof()) )
      {
	// point found
	pointWasFound=true;

	// Get the tag
	unsigned int beginIndexItemTag= line.find_first_of ("->",0)+2; 
	unsigned int endIndexItemTag= line.find_first_of ("=",0)-1;
	string itemTag= line.substr(beginIndexItemTag , endIndexItemTag-beginIndexItemTag+1);
	
	// Get the point Value
	unsigned int beginIndexItemValue= line.find_first_of ("=",0)+1; 
	stringstream valueString( line.substr(beginIndexItemValue) );
	double itemValue;
	valueString>> itemValue;
	
	// Fill the items
	if (itemTag=="X")
	  item1[0]=itemValue;
	if (itemTag=="Y")
	  item1[1]=itemValue;
	if (itemTag=="Z")
	  item1[2]=itemValue;
	if (itemTag=="X_Corresp")
	  item2[0]=itemValue;
	if (itemTag=="Y_Corresp")
	  item2[1]=itemValue;
	if (itemTag=="Z_Corresp")
	  item2[2]=itemValue;
	if (itemTag=="VeryUnsure")
	  veryUnsure=itemValue;
	
	// Get the next line
	skipComment(listStream);
	getline(listStream, line);
	
	// Get the point number
	unsigned int beginIndexItemNumber= line.find_first_of ("_",0)+1; 
	unsigned int endIndexItemNumber= line.find_first_of ("->",0)-1;
	stringstream numberString( line.substr(beginIndexItemNumber , endIndexItemNumber-beginIndexItemNumber+1) );
	numberString >> itemNumber;
      }

    return pointWasFound;
  }


 //---------------------------------------------------------------------
  // Print
  //---------------------------------------------------------------------
  template<class ListItemType>
  void List< ListItemType>::Print()
  {
    for (unsigned int i=0; i< this->size(); i++)
      std::cout<<this->at(i)<<std::endl;
  }


  //---------------------------------------------------------------------
  // Write
  //---------------------------------------------------------------------
  template<class ListItemType>
  void List<ListItemType>::Write(const string fileName, const bool verbose)
  {
    ofstream listStream(fileName.c_str());
    if(!listStream.is_open()){
      cerr << "ERROR: Couldn't open file " << fileName << " in List::Write" << endl;
      return;
    }

    typename ListType::iterator it=this->begin();
    while(it!=this->end()) {
      listStream << (*it)[0];
      for (unsigned int i=1; i< this->at(0).Size(); i++)
	listStream <<" "<< (*it)[i];
      listStream<< endl;      
      it++;
    }
    listStream.close();
  }

  //---------------------------------------------------------------------
  // Norm
  //---------------------------------------------------------------------
  template<class ListItemType>
  List<itk::FixedArray<double, 1> > List< ListItemType>::Norm()
  {
    List<itk::FixedArray<double, 1> > norm;
    itk::FixedArray<double, 1> n;
    unsigned int d;
    for (unsigned int i=0; i< this->size(); i++)
      {
	n[0]=0;
	for (d=0; d< Dimension; d++)
	  n[0]+=this->at(i)[d] * this->at(i)[d];
	n[0]=sqrt(n[0]);
	norm.push_back(n);
      }
    return norm;
  }
 

  

}

#endif //#define CLITKLIST


    //    // Open the file
    //     ifstream listStream(fileName.c_str());
    //     ListItemType item;
    //     string line;
    //     if(!listStream.is_open()){
    //       std::cerr << "ERROR: Couldn't open file " << fileName << " in List::Read" << std::endl;
    //       return;
    //     }
    //     skipComment(listStream);
    
    //     // Skip the configuration lines
    //     while (true)
    //       {
    // 	// Get the line
    // 	skipComment(listStream);
    // 	getline(listStream, line);
    
    // 	// Get the line type
    // 	unsigned int endIndexItemType= line.find_first_of ("_",0)-1;
    // 	string typeString= line.substr(0 , endIndexItemType+1);
    
    // 	// Check
    // 	if  (typeString=="Point")
    // 	  break;
    //       }
    
    //     // Get the point number
    //     unsigned int beginIndexItemNumber= line.find_first_of ("_",0)+1; 
    //     unsigned int endIndexItemNumber= line.find_first_of ("->",0)-1;
    //     stringstream numberString( line.substr(beginIndexItemNumber , endIndexItemNumber-beginIndexItemNumber+1) );
    //     unsigned int itemNumber;
    //     numberString>> itemNumber;
    
    //     // Go over all the lines
    //     while(!listStream.eof()) 
    //       {
    // 	// Stored data
    // 	itk::Point<ValueType, Dimension> point1, point2;
    // 	point1.Fill(0.);
    // 	point2.Fill(0.);
    // 	bool veryUnsure=false;
    
    // 	// Retrieved data
    // 	itk::ContinuousIndex<ValueType, Dimension> item1, item2;
    // 	item1.Fill(0.);
    // 	item2.Fill(0.);
    
    // 	// Go over all lines corresponding to this point
    // 	unsigned int currentNumber=itemNumber;
    // 	while (itemNumber == currentNumber && (!listStream.eof()) )
    // 	  {
    // 	    // Get the tag
    // 	    unsigned int beginIndexItemTag= line.find_first_of ("->",0)+2; 
    // 	    unsigned int endIndexItemTag= line.find_first_of ("=",0)-1;
    // 	    string itemTag= line.substr(beginIndexItemTag , endIndexItemTag-beginIndexItemTag+1);
    
    // 	    // Get the point Value
    // 	    unsigned int beginIndexItemValue= line.find_first_of ("=",0)+1; 
    // 	    stringstream valueString( line.substr(beginIndexItemValue) );
    // 	    double itemValue;
    // 	    valueString>> itemValue;
    
    // 	    // Fill the items
    // 	    if (itemTag=="X")
    // 	      item1[0]=itemValue;
    // 	    if (itemTag=="Y")
    // 	      item1[1]=itemValue;
    // 	    if (itemTag=="Z")
    // 	      item1[2]=itemValue;
    // 	    if (itemTag=="X_Corresp")
    // 	      item2[0]=itemValue;
    // 	    if (itemTag=="Y_Corresp")
    // 	      item2[1]=itemValue;
    // 	    if (itemTag=="Z_Corresp")
    // 	       item2[2]=itemValue;
    // 	    if (itemTag=="VeryUnsure")
    // 	      veryUnsure=itemValue;
    
    // 	    // Get the next line
    // 	    skipComment(listStream);
    // 	    getline(listStream, line);
    
    // 	    // Get the point number
    // 	    unsigned int beginIndexItemNumber= line.find_first_of ("_",0)+1; 
    // 	    unsigned int endIndexItemNumber= line.find_first_of ("->",0)-1;
    // 	    stringstream numberString( line.substr(beginIndexItemNumber , endIndexItemNumber-beginIndexItemNumber+1) );
    // 	    numberString>> itemNumber;
    // 	  }
