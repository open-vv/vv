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

#include "vvStructureSetActor.h"
#include "vvROIActor.h"

//------------------------------------------------------------------------------
vvStructureSetActor::vvStructureSetActor()
{

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvStructureSetActor::~vvStructureSetActor()
{
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvStructureSetActor::SetStructureSet(clitk::DicomRT_StructureSet * s)
{
  mStructureSet = s;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvStructureSetActor::SetSlicerManager(vvSlicerManager * s)
{
  mSlicerManager = s;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvROIActor * vvStructureSetActor::GetROIActor(int n)
{
  if (mMapROIIndex.find(n) == mMapROIIndex.end()) {
    std::cerr << "No ROI number " << n << std::endl;
    return NULL;
  }
  return mROIActors[mMapROIIndex[n]];
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvStructureSetActor::CreateNewROIActor(int n)
{
  // Check
  clitk::DicomRT_ROI * roi = mStructureSet->GetROI(n);
  if (roi == NULL) {
    std::cerr << "Error. No ROI number " << n << std::endl;
    exit(0);
  }

  // Add ROI Actors
  vvROIActor * actor = new vvROIActor;
  mROIActors.push_back(actor);
  actor->SetROI(roi);
  actor->SetSlicerManager(mSlicerManager);
  actor->Initialize();
  mMapROIIndex[n] = mROIActors.size()-1;

  //  actor->Update();
}
//------------------------------------------------------------------------------


