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

#ifndef CLITKRELATIVEPOSITIONDATABASE_CXX
#define CLITKRELATIVEPOSITIONDATABASE_CXX

// clitk
#include "clitkRelativePositionDataBase.h"

namespace clitk {

  //--------------------------------------------------------------------
  void RelativePositionDataBase::ReadIndex(std::istream & is, IndexType & index)
  {
    is >> index.patient; 
    is >> index.station;
    is >> index.object;
    is >> index.orientation.angle1; 
    index.orientation.angle1 = clitk::deg2rad(index.orientation.angle1);
    is >> index.orientation.angle2;
    index.orientation.angle2 = clitk::deg2rad(index.orientation.angle2);
    std::string s;
    is >> s;
    if (s=="true") index.orientation.notFlag = true;
    else index.orientation.notFlag = false;
  }
  //--------------------------------------------------------------------
 

  //--------------------------------------------------------------------
  void RelativePositionDataBase::ReadInformation(std::istream & is, RelativePositionInformationType & v)
  {
    is >> v.threshold;
    is >> v.sizeBeforeThreshold;
    is >> v.sizeAfterThreshold;
    is >> v.sizeReference;
  }
  //--------------------------------------------------------------------
  
  //--------------------------------------------------------------------
  void RelativePositionDataBase::Read(const std::string & filename)
  {
    DD(filename);
    std::ifstream is;
    openFileForReading(is, filename);

    std::string s;
    IndexType index;
    RelativePositionInformationType v;
    while (is) {
      skipComment(is); /* FIXME Read Index etc */ 
      ReadIndex(is, index);
      ReadInformation(is, v);      

      if (is) {// FIXME INSERT
        // Set in station
        if (m_DB.find(index.station) == m_DB.end()) {
          MapByObjectType s;
          m_DB[index.station] = s;
        }
        MapByObjectType & s = m_DB[index.station];
        
        // Get Orientation map from Object
        if (s.find(index.object) == s.end()) {
          MapByOrientationType r;
          s[index.object] = r;
        }
        MapByOrientationType & r = s[index.object];
        
        // Get Patient map from Orientation
        if (r.find(index.orientation) == r.end()) {
          MapByPatientType q;
          r[index.orientation] = q;
        }
        MapByPatientType & q = r[index.orientation];

        // Set value by patient
        q[index.patient] = v;
        
        // Debug
        // index.Println(); 
        // GetMapByPatient(index).find(index.patient)->second.Println();
        
      } // End insertion
    } // end loop reading
    DD("end read");
  }
  //--------------------------------------------------------------------

  
  //--------------------------------------------------------------------
  const RelativePositionDataBase::MapByOrientationType & 
  RelativePositionDataBase::GetMapByOrientation(const IndexType & index) const
  {
    const MapByObjectType & a = GetMapByObject(index.station);
    if (a.find(index.object) == a.end()) {
      clitkExceptionMacro("Could not find index in DB (object= '" << index.object << "' not found).");
    }
    return a.find(index.object)->second;    
  }
  //--------------------------------------------------------------------
  

  //--------------------------------------------------------------------
  const RelativePositionDataBase::MapByObjectType & 
  RelativePositionDataBase::GetMapByObject(const std::string & station) const
  {
    if (m_DB.find(station) == m_DB.end()) {
      clitkExceptionMacro("Could not find index in DB (station= '" << station << "' not found).");
    }
    return m_DB.find(station)->second;    
  }
  //--------------------------------------------------------------------
  

  //--------------------------------------------------------------------
  const RelativePositionDataBase::MapByPatientType & 
  RelativePositionDataBase::GetMapByPatient(const IndexType & index) const
  {
    const MapByOrientationType & a = GetMapByOrientation(index);
    if (a.find(index.orientation) == a.end()) {
      std::ostringstream s;
      index.orientation.Print(s);
      clitkExceptionMacro("Could not find index in DB (orientation= '" << s.str() << "' not found).");
    }
    return a.find(index.orientation)->second;
  }
  //--------------------------------------------------------------------
  

  //--------------------------------------------------------------------
  const RelativePositionInformationType & 
  RelativePositionDataBase::GetInformation(const IndexType & index) const
  {
    const RelativePositionDataBase::MapByPatientType & a = GetMapByPatient(index);
    if (a.find(index.patient) == a.end()) {
      clitkExceptionMacro("Could not find index in DB (patient= '" << index.patient << "' not found).");
    }
    return a.find(index.patient)->second;
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  int RelativePositionDataBase::GetNumberOfPatient(const IndexType & index) const
  {
    const MapByPatientType & o = GetMapByPatient(index);
    return o.size();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  std::vector<std::string> & RelativePositionDataBase::GetListOfPatients(const IndexType & index) const
  {
    const MapByPatientType & o = GetMapByPatient(index);
    MapByPatientType::const_iterator iter = o.begin();
    std::vector<std::string> * v = new std::vector<std::string>; 
    MapToVecFirst(o, *v);
    return *v;
  }
  //--------------------------------------------------------------------


 //--------------------------------------------------------------------
  double RelativePositionDataBase::GetAreaGain(const IndexType & index) const
  {
    // FIXME change name
    const RelativePositionInformationType & v = GetInformation(index);
    return v.sizeAfterThreshold/v.sizeBeforeThreshold;
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  double RelativePositionDataBase::GetThreshold(const IndexType & index) const
  {
    const RelativePositionInformationType & v = GetInformation(index);
    return v.threshold;
  }
  //--------------------------------------------------------------------
    

  //--------------------------------------------------------------------
  void
  RelativePositionDataBase::GetListOfObjects(const std::string & station, std::vector<std::string> & objects) const
  {
    const MapByObjectType & a = GetMapByObject(station);
    MapToVecFirst(a, objects);
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  void
  RelativePositionDataBase::GetListOfOrientations(const std::string & station, 
                                                  const std::string & object, 
                                                  std::vector<RelativePositionOrientationType> & orientations) const
  {
    IndexType i;
    i.station = station;
    i.object = object;
    const MapByOrientationType & n = GetMapByOrientation(i);
    MapToVecFirst(n, orientations);    
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  bool RelativePositionDataBase::CheckIndex(const IndexType & index) const
  {
    try {
      const RelativePositionInformationType & m =  GetInformation(index);
    } catch (clitk::ExceptionObject e) {
      // std::cout << e.what() << std::endl;      
      return false;
    }
    return true;
  }
  //--------------------------------------------------------------------
  

} // end namespace clitk
//--------------------------------------------------------------------

#endif
