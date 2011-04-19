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
#ifndef CLITKIO_H
#define CLITKIO_H
/**
   -------------------------------------------------------------------
   * @file   clitkIO.h
   * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
   * @date   17 May 2006 07:57:56

   * @brief  

   -------------------------------------------------------------------*/

//--------------------------------------------------------------------
// CLITK_INIT
#define CLITK_INIT clitk::RegisterClitkFactories();

namespace clitk {
  void RegisterClitkFactories();
}

#endif /* end #define CLITKIO_H */

