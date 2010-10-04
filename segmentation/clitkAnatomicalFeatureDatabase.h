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

#ifndef CLITKANATOMICALFEATUREDATABASE_H
#define CLITKANATOMICALFEATUREDATABASE_H

// clitk
#include "clitkCommon.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    Class to store and retreive anatomical feature such as 3D
    named landmarks points. 
  */
  class AnatomicalFeatureDatabase: public itk::Object
  {
  public:
    AnatomicalFeatureDatabase();

    // Set/Get filename 
    itkSetMacro(Filename, std::string);
    itkGetConstMacro(Filename, std::string);

    // Read and write DB
    void Write();
    void Load();
    
    // Get landmarks
    typedef itk::Point<double,3> PointType3D;
    void SetPoint3D(std::string tag, PointType3D & p);
    void GetPoint3D(std::string tag, PointType3D & p);
    
  protected:
    std::string m_Filename;
    typedef std::map<std::string, std::string> MapTagType;
    MapTagType m_MapOfTag;

  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#endif // CLITKANATOMICALFEATUREDATABASE_H
