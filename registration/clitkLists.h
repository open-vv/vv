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
#ifndef clitkLists_h
#define clitkLists_h
/**
   =================================================
   * @file   clitkLists.h
   * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
   * @date   30 April 2008
   * 
   * @brief 
   * 
   =================================================*/

#include "clitkCommon.h"
#include "clitkIO.h"
#include "clitkList.h"

// itk include
#include "itkLightObject.h"

using namespace std;

namespace clitk{

  template <class ListItemType > 
  class Lists: public std::vector<clitk::List<ListItemType> >
  {
  public:
    //=====================================================================================
    //Typedefs
    typedef typename ListItemType::ValueType ValueType;
    itkStaticConstMacro(Dimension, unsigned int, ListItemType::Dimension);
    typedef List<ListItemType> ListType;
    typedef itk::Image<signed short, Dimension> ImageType;

    //=====================================================================================  
    // IO
    void Read(const std::vector<std::string>& fileNames, bool verbose=false);
    void ReadAndConvertPointPairs(const std::vector<std::string>& fileNames, ListType& refList, const typename ImageType::Pointer referenceImage , bool verbose=false);
    void Write(const std::vector<std::string> fileNames, const bool verbose);
    void Print();
  
    //=====================================================================================
    // Members
    Lists< itk::FixedArray<double,1> > Norm(void);
          
    //=====================================================================================
    //Constructors - Destructors
    Lists(){};
    Lists(unsigned int size){this->resize(size);};
    Lists(const std::vector<std::string> fileNames, bool verbose=false ){Read(fileNames, verbose);}
    ~Lists(){};
  

  protected:


  };


} // namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkLists.txx"
#endif

#endif
