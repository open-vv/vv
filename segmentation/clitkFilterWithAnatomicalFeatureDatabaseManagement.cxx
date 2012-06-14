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

// clitk
#include "clitkFilterWithAnatomicalFeatureDatabaseManagement.h"

//--------------------------------------------------------------------
clitk::FilterWithAnatomicalFeatureDatabaseManagement::
FilterWithAnatomicalFeatureDatabaseManagement() 
{
  m_AFDB = NULL; 
  SetAFDBFilename("default.afdb");
  SetAFDBPath("./");
  DisplayUsedStructuresOnlyFlagOff();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::FilterWithAnatomicalFeatureDatabaseManagement::WriteAFDB() 
{
  GetAFDB()->SetFilename(GetAFDBFilename());
  GetAFDB()->Write();
}
//--------------------------------------------------------------------

    
//--------------------------------------------------------------------
void clitk::FilterWithAnatomicalFeatureDatabaseManagement::LoadAFDB() 
{
  GetAFDB()->SetFilename(GetAFDBFilename());
  GetAFDB()->SetPath(GetAFDBPath());
  try {
    GetAFDB()->Load();
  } catch (clitk::ExceptionObject e) {
    std::cout << "******* Could not read '" << GetAFDBFilename() << "', create one AFDB. ********" << std::endl;
    GetAFDB();
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
clitk::AnatomicalFeatureDatabase::Pointer clitk::FilterWithAnatomicalFeatureDatabaseManagement::GetAFDB() 
{
  if (!m_AFDB) {
    m_AFDB = clitk::AnatomicalFeatureDatabase::New();
  }
  return m_AFDB;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::FilterWithAnatomicalFeatureDatabaseManagement::
AddUsedStructures(std::string station, std::string structure)
{
  if (!GetDisplayUsedStructuresOnlyFlag()) return;
  std::cout << station << "\t" << structure;
  bool founded = true;
  bool tag = GetAFDB()->TagExist(structure);
  if (tag) {
    typedef typename itk::Image<uchar, 3> ImageType;
    founded = GetAFDB()->CheckImage<ImageType>(structure);
    if (!founded) { 
      std::cout << " \t Image not exist in DB ";
      std::cout << "<" << GetAFDB()->GetTagValue(structure) << "> ";
    }
  }
  if (!tag) std::cout << " \t Tag not found in DB";
  std::cout << std::endl;
}
//--------------------------------------------------------------------
