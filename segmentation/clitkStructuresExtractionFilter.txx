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
#include "clitkStructuresExtractionFilter.h"

//--------------------------------------------------------------------
template <class TImageType>
clitk::StructuresExtractionFilter<TImageType>::
StructuresExtractionFilter():
  clitk::FilterBase(),
  clitk::FilterWithAnatomicalFeatureDatabaseManagement(),
  itk::ImageToImageFilter<TImageType, MaskImageType>()
{
  SetBackgroundValue(0);
  SetForegroundValue(1);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::StructuresExtractionFilter<TImageType>::
AddRelativePositionListFilename(std::string s) {
  mListOfRelativePositionListFilename.push_back(s);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
typename clitk::StructuresExtractionFilter<TImageType>::MaskImagePointer
clitk::StructuresExtractionFilter<TImageType>::
ApplyRelativePositionList(std::string name, MaskImageType * input, bool overlap) 
{ 
  // Create all RelativePositionList
  for(unsigned int i=0; i<mListOfRelativePositionListFilename.size(); i++) {
    RelPosListPointer rpl = RelPosListType::New();
    rpl->SetDisplayUsedStructuresOnlyFlag(this->GetDisplayUsedStructuresOnlyFlag());
    rpl->SetAFDB(GetAFDB());
    rpl->Read(mListOfRelativePositionListFilename[i]);
    std::string s = rpl->GetInputName();
    mMapOfRelativePositionList[s] = rpl;
  }

  RelPosListPointer relpos;
  if (mMapOfRelativePositionList.find(name) == mMapOfRelativePositionList.end()) {
    std::cerr << "Warning: I do not find '" << name << "' in the RelativePositionList." << std::endl;
    //DD("Not find !"); // do nothing
  }
  else {
    relpos = mMapOfRelativePositionList[name];
    relpos->SetVerboseStepFlag(GetVerboseStepFlag());
    relpos->SetCurrentStepBaseId(GetCurrentStepBaseId());
    relpos->SetCurrentStepId(GetCurrentStepId());
    relpos->SetCurrentStepNumber(GetCurrentStepNumber());
    relpos->SetWriteStepFlag(GetWriteStepFlag());
    relpos->SetInput(input);
    if (!this->GetDisplayUsedStructuresOnlyFlag() && overlap) {
      std::string n = name.substr(8,3);
      MaskImagePointer ref = this->GetAFDB()->template GetImage <MaskImageType>(n+"_Ref");
      relpos->SetReferenceImageForOverlapMeasure(ref);
    }
    relpos->Update();
    input = relpos->GetOutput();

    SetCurrentStepNumber(relpos->GetCurrentStepNumber());
  }
  return input;
}
//--------------------------------------------------------------------

