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

#ifndef CLITKRELATIVEPOSITIONANALYZERFILTER_H
#define CLITKRELATIVEPOSITIONANALYZERFILTER_H

// clitk
#include "clitkFilterWithAnatomicalFeatureDatabaseManagement.h"
#include "clitkFilterBase.h"
#include "clitkRelativePositionDataBase.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    Load a database of relative positions. Analyze it and provide
    common relative position for each patient. 
  */
  //--------------------------------------------------------------------
  
  class RelativePositionDataBaseAnalyzerFilter:
    public virtual clitk::FilterBase,
    public clitk::FilterWithAnatomicalFeatureDatabaseManagement
  {

  public:
    RelativePositionDataBaseAnalyzerFilter();
    virtual ~RelativePositionDataBaseAnalyzerFilter() {}
    
    // Input
    itkGetConstMacro(DatabaseFilename, std::string);
    itkSetMacro(DatabaseFilename, std::string);
    itkGetConstMacro(StationName, std::string);
    itkSetMacro(StationName, std::string);
    itkGetConstMacro(ObjectName, std::string);
    itkSetMacro(ObjectName, std::string);
    itkGetConstMacro(OutputFilename, std::string);
    itkSetMacro(OutputFilename, std::string);
    
    // For debug
    void PrintOptions();
    
    // Go !
    virtual void Update();

  protected:
    std::string m_DatabaseFilename;
    std::string m_StationName;
    std::string m_ObjectName;
    std::string m_OutputFilename;
    clitk::RelativePositionDataBase db;

    bool ComputeOptimalThreshold(RelativePositionDataBaseIndexType & index, double & threshold);

  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#include "clitkRelativePositionDataBaseAnalyzerFilter.txx"

#endif
