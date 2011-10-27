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
  for(int i=0; i<m_ListOfObjects.size(); i++) {
    // DD(i);
    // DD(m_ListOfObjects[i]);
    // Set current index
    index.object = m_ListOfObjects[i];
    // Get the list of orientation
    std::vector<clitk::RelativePositionOrientationType> m_ListOfOrientations;
    db.GetListOfOrientations(GetStationName(), index.object, m_ListOfOrientations);
    
    // Loop over orientation
    for(int j=0; j<m_ListOfOrientations.size(); j++) {
      // DD(j);
      // m_ListOfOrientations[j].Println();
      // Set current index
      index.orientation = m_ListOfOrientations[j];
      // Compute the best RelPos parameters 
      double threshold;
      bool ok = ComputeOptimalThreshold(index, threshold);
      m_ListOfThresholds.push_back(threshold);
      
      // Print debug FIXME
      if (ok) {
        std::cout << m_ListOfObjects[i] << " ";
        m_ListOfOrientations[j].Print();
        std::cout << " " << threshold << " " << ok << std::endl;
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
  // index.orientation.Println();

  // For a given station, object, orientation
  bool stop=false;
  int i=0;
  if (index.orientation.notFlag) threshold = 0.0;
  else threshold = 1.0;
  while (!stop && (i<ListOfPatients.size())) {
    // DD(i);
    index.patient = ListOfPatients[i];
    // std::cout << i << " " << index.patient << " ";
    // Check index
    if (!db.CheckIndex(index)) {
      std::cout << "Warning index does not exist in the DB. index = "; 
      index.Println(std::cout);
    }
    else {
      // index = patient station object, orientation = angle1 angle2 notFlag
      if (db.GetAreaGain(index) == 1.0) stop = true;
      else {
        if (index.orientation.notFlag) threshold = std::max(db.GetThreshold(index), threshold);
        else threshold = std::min(db.GetThreshold(index), threshold);
        // std::cout << db.GetThreshold(index) << " opt=" << threshold;
      } 
    }
    ++i;
  }
  // std::cout << std::endl;
  // DD(threshold);
  // DD(stop);
  return !stop; // if stop before the end, this orientation is not useful.
}
//--------------------------------------------------------------------
