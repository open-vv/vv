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

#ifndef CLITKRELATIVEPOSITIONDATABASE_H
#define CLITKRELATIVEPOSITIONDATABASE_H

// clitk
#include "clitkCommon.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    FIXME
   */
  //--------------------------------------------------------------------
  

  //--------------------------------------------------------------------
   class RelativePositionDirectionType {
  public:
    double angle1;
    double angle2;
    bool notFlag;

    void Print(std::ostream & os = std::cout) const {
      os << clitk::rad2deg(angle1) << " " << clitk::rad2deg(angle2) << " ";
      if (notFlag) os << "true"; 
      else os << "false";
      os << " ";
    }
    
    void PrintOptions(std::ostream & os = std::cout) const {
      os << "angle1 = " << clitk::rad2deg(angle1) << std::endl
         << "angle2 = " << clitk::rad2deg(angle2) << std::endl;
      if (notFlag) os << "inverse" << std::endl;
    }
    
    void Println(std::ostream & os = std::cout) const {
      Print(os);
      os << std::endl;
    }

    bool operator< (const RelativePositionDirectionType &compare) const
    {
      if (angle1 < compare.angle1) return true;
      if (angle1 > compare.angle1) return false;
        
      if (angle2 < compare.angle2) return true;
      if (angle2 > compare.angle2) return false;
        
      if (notFlag == true) {
        if (compare.notFlag == false) return true;
        else return false;
      }
      return false;
    }
  };
  //--------------------------------------------------------------------
 

  //--------------------------------------------------------------------
  class RelativePositionDataBaseIndexType {
  public:
    std::string patient;
    std::string station;
    std::string object;
    RelativePositionDirectionType direction;
    void Print(std::ostream & os = std::cout) const {
      os << patient << " " << station << " " << object << " ";
      direction.Print(os);
    }
    void Println(std::ostream & os = std::cout) const {
      Print(os);
      os << std::endl;
    }
  };
  //--------------------------------------------------------------------
  

  //--------------------------------------------------------------------
  class RelativePositionInformationType {
  public:
    double threshold;
    int sizeBeforeThreshold;
    int sizeAfterThreshold;
    int sizeReference;
    void Print(std::ostream & os = std::cout) const {
      os << threshold << " " << sizeBeforeThreshold << " " 
         << sizeAfterThreshold << " " << sizeReference;
    }
    void Println(std::ostream & os = std::cout) const {
      Print(os);
      os << std::endl;
    }
  };
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  class RelativePositionDataBase {
    
  public:    
    RelativePositionDataBase() {}
    ~RelativePositionDataBase() {}

    typedef RelativePositionDataBaseIndexType IndexType;

    void Read(const std::string & filename);
    double GetAreaGain(const IndexType & index) const;
    double GetThreshold(const IndexType & index) const;
    int GetNumberOfPatient(const IndexType & index) const;
    std::vector<std::string> & GetListOfPatients(const IndexType & index) const;
    void GetListOfObjects(const std::string & station, std::vector<std::string> & objects) const;
    void GetListOfDirections(const std::string & station, 
                               const std::string & object, 
                               std::vector<RelativePositionDirectionType> & directions) const;
    bool CheckIndex(const IndexType & index) const;

  protected:
    typedef std::map<std::string, RelativePositionInformationType> MapByPatientType;
    typedef std::map<RelativePositionDirectionType, MapByPatientType> MapByDirectionType;
    typedef std::map<std::string, MapByDirectionType> MapByObjectType;
    typedef std::map<std::string, MapByObjectType> MapByStationType;
    MapByStationType m_DB;
    
    void ReadIndex(std::istream & is, IndexType & index);
    void ReadInformation(std::istream & is, RelativePositionInformationType & v);

    const MapByDirectionType & GetMapByDirection(const IndexType & index) const;
    const MapByPatientType & GetMapByPatient(const IndexType & index) const;
    const RelativePositionInformationType & GetInformation(const IndexType & index) const;
    const MapByObjectType & GetMapByObject(const std::string & station) const;

  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#endif
