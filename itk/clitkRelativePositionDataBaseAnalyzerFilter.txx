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

//--------------------------------------------------------------------
clitk::RelativePositionDataBaseAnalyzerFilter::
RelativePositionDataBaseAnalyzerFilter():
  clitk::FilterBase(),
  clitk::FilterWithAnatomicalFeatureDatabaseManagement()
{
  VerboseFlagOff();
  SetDatabaseFilename("default.dbrp");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void 
clitk::RelativePositionDataBaseAnalyzerFilter::
PrintOptions() 
{
  DD("TODO");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void 
clitk::RelativePositionDataBaseAnalyzerFilter::
Update() 
{
  // Load DB of relative position
  db.Read(GetDatabaseFilename());

  // Get list of objects, list of orientation
  std::vector<std::string> m_ListOfObjects;
  db.GetListOfObjects(GetStationName(), m_ListOfObjects);
  
  // Set initial index in the DB
  clitk::RelativePositionDataBase::IndexType index;
  index.station = GetStationName();

  // Loop over objects
  std::vector<double> m_ListOfThresholds;
  for(unsigned int i=0; i<m_ListOfObjects.size(); i++) {
    // DD(i);
    // DD(m_ListOfObjects[i]);
    // Set current index
    index.object = m_ListOfObjects[i];
    // Get the list of direction
    std::vector<clitk::RelativePositionDirectionType> m_ListOfDirections;
    db.GetListOfDirections(GetStationName(), index.object, m_ListOfDirections);
    
    // Loop over direction
    for(unsigned int j=0; j<m_ListOfDirections.size(); j++) {
      // DD(j);
      // m_ListOfDirections[j].Println();
      // Set current index
      index.direction = m_ListOfDirections[j];
      // Compute the best RelPos parameters 
      double threshold;
      bool ok = ComputeOptimalThreshold(index, threshold);
      m_ListOfThresholds.push_back(threshold);
      
      // Print debug FIXME
      if (ok) {
        /*std::cout << m_ListOfObjects[i] << " ";
        m_ListOfDirections[j].Print();
        std::cout << " " << threshold << " " << ok << std::endl;
        */
        std::cout << "# -----------------------" << std::endl
                  << "object = " << m_ListOfObjects[i] << std::endl;
        m_ListOfDirections[j].PrintOptions();
        std::cout << "threshold = " << threshold << std::endl
                  << "sliceBySlice" << std::endl << std::endl; // FIXME spacing ?
      }
    }
  }
}    
//--------------------------------------------------------------------
    

//--------------------------------------------------------------------
bool
clitk::RelativePositionDataBaseAnalyzerFilter::
ComputeOptimalThreshold(RelativePositionDataBaseIndexType & index, double & threshold) 
{
  // Get list of patient
  std::vector<std::string> & ListOfPatients = db.GetListOfPatients(index);
  //  DD(ListOfPatients.size());
  // index.direction.Println();

  // For a given station, object, direction
  bool stop=false;
  unsigned int i=0;
  if (index.direction.notFlag) threshold = 0.0;
  else threshold = 1.0;
  while (!stop && (i<ListOfPatients.size())) {
    index.patient = ListOfPatients[i];
    // Check index
    if (!db.CheckIndex(index)) {
      std::cout << "Warning index does not exist in the DB. index = "; 
      index.Println(std::cout);
    }
    else {
      if (index.direction.notFlag) threshold = std::max(db.GetThreshold(index), threshold);
      else threshold = std::min(db.GetThreshold(index), threshold);
    }
    ++i;
  } // end while

  if (index.direction.notFlag)  {
    if (threshold >=1) return false; // not useful
  }
  else {
    if (threshold <=0) return false; // not useful
  }
  return true;
}
//--------------------------------------------------------------------
