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

#ifndef CLITKFILTERWITHANATOMICALFEATUREDATABASEMANAGEMENT_H
#define CLITKFILTERWITHANATOMICALFEATUREDATABASEMANAGEMENT_H

// clitk
#include "clitkAnatomicalFeatureDatabase.h"
#include "clitkFilterBase.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    - Convenient class to add AFDB capabilities to a filter 
    - "virtual" inheritance is needed to avoir ambiguous inherited
    functions
  */
  //--------------------------------------------------------------------
  class FilterWithAnatomicalFeatureDatabaseManagement: public virtual FilterBase
  {
  public:
    // Standard class typedefs
    typedef FilterWithAnatomicalFeatureDatabaseManagement Self;
    
    // Run-time type information (and related methods)
    itkTypeMacro(FilterWithAnatomicalFeatureDatabaseManagement, Object);

    itkSetMacro(AFDBFilename, std::string);
    itkGetConstMacro(AFDBFilename, std::string);

    itkSetMacro(AFDBPath, std::string);
    itkGetConstMacro(AFDBPath, std::string);

    itkGetConstMacro(DisplayUsedStructuresOnlyFlag, bool);
    itkSetMacro(DisplayUsedStructuresOnlyFlag, bool);
    itkBooleanMacro(DisplayUsedStructuresOnlyFlag);

    void WriteAFDB();
    void LoadAFDB();

    AnatomicalFeatureDatabase::Pointer GetAFDB();
    void SetAFDB(AnatomicalFeatureDatabase * a) { m_AFDB = a; }

  protected:
    FilterWithAnatomicalFeatureDatabaseManagement();
    virtual ~FilterWithAnatomicalFeatureDatabaseManagement() {}    
    
    std::string m_AFDBFilename;
    std::string m_AFDBPath;
    clitk::AnatomicalFeatureDatabase::Pointer m_AFDB;

    // For debug: display used structures but do not perform
    // segmentation
    bool m_DisplayUsedStructuresOnlyFlag;
    void AddUsedStructures(std::string station, std::string structure);    

  private:
    FilterWithAnatomicalFeatureDatabaseManagement(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#endif // CLITKFILTERWITHANATOMICALFEATUREDATABASEMANAGEMENT_H
