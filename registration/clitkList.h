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
#ifndef CLITKLIST_H
#define CLITKLIST_H
/**
   =================================================
   * @file   clitkList.h
   * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
   * @date   30 April 2008
   * 
   * @brief 
   * 
   =================================================*/

#include "clitkCommon.h"
#include "clitkIO.h"

// itk include
#include "itkLightObject.h"

using namespace std;

namespace clitk{

  template <class ListItemType > 
  class List: public std::vector<ListItemType >
  {

  public:
    //=====================================================================================
    //Typedefs
    typedef typename ListItemType::ValueType ValueType;
    itkStaticConstMacro(Dimension, unsigned int, ListItemType::Dimension);
    typedef List<ListItemType> ListType;
    typedef itk::ContinuousIndex<ValueType, Dimension> IndexType;
    typedef itk::Image<signed short, Dimension> ImageType;

    //=====================================================================================  
    // IO
    void Read(const string& fileName, bool verbose=false);
    void ReadAndConvertPointPairs(const string& fileName, List& correspList, const typename ImageType::Pointer referenceImage , bool verbose=false);
    bool ReadPointPair(const string& fileName, const unsigned int& pointNumber, IndexType& item1, IndexType& item2, bool& veryUnsure);
    void Write(const string fileName, const bool verbose);
    void Print();
  
    //=====================================================================================
    // Members
    List<itk::FixedArray<double, 1> > Norm(void);

    //=====================================================================================
    //Constructors - Destructors
    List(){};
    List(const string fileName, bool verbose=false ){Read(fileName, verbose);}
    List(unsigned int size) { this->resize(size); };
    ~List(){};

  protected:


  };


} // namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkList.txx"
#endif

#endif

