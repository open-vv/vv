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

// clitk
#include "clitkFilterWithAnatomicalFeatureDatabaseManagement.h"

//--------------------------------------------------------------------
clitk::FilterWithAnatomicalFeatureDatabaseManagement::
FilterWithAnatomicalFeatureDatabaseManagement() 
{
  m_AFDB = NULL; 
  SetAFDBFilename("default.afdb");
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
  try {
    GetAFDB()->Load();
  } catch (clitk::ExceptionObject e) {
    std::cout << "Could not read '" << GetAFDBFilename() << "', create one AFDB." << std::endl;
    GetAFDB();
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
clitk::AnatomicalFeatureDatabase * clitk::FilterWithAnatomicalFeatureDatabaseManagement::GetAFDB() 
{
  if (m_AFDB == NULL) {
    m_AFDB = new clitk::AnatomicalFeatureDatabase;
  }
  return m_AFDB;
}
//--------------------------------------------------------------------
