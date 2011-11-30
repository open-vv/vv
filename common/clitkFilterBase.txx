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

#include "clitkImageCommon.h"
#include "clitkMemoryUsage.h"

//--------------------------------------------------------------------
template<class OptionType>
void clitk::FilterBase::VerboseOption(std::string name, OptionType value) 
{
  if (!this->GetVerboseOptionFlag()) return;
  std::cout << "Set option '" << name << "' = " << value << std::endl;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class OptionType>
void clitk::FilterBase::VerboseOption(std::string name, int nb, OptionType value) 
{
  if (!this->GetVerboseOptionFlag()) return;
  if (nb==0) std::cout << "Set option '" << name << "' not given" << std::endl;
  else {
    std::cout << "Set option '" << name << "' = " << value << std::endl;
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class OptionType>
void clitk::FilterBase::VerboseOptionV(std::string name, int nb, OptionType * value) 
{
  if (!this->GetVerboseOptionFlag()) return;
  if (nb==0) std::cout << "Set option '" << name << "' not given" << std::endl;
  else {
    std::cout << "Set option '" << name << "'[" << nb << "] ";
    for(int i=0; i<nb; i++) std::cout << value[i] << " ";
    std::cout << std::endl;
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class TInternalImageType>
void clitk::FilterBase::StopCurrentStep(typename TInternalImageType::Pointer p, std::string txt) 
{
  StopCurrentStep();
  if (m_WriteStepFlag) {
    std::ostringstream name;
    name << "step-" << GetCurrentStepId() << ".mhd";
    clitk::writeImage<TInternalImageType>(p, name.str());
  }
  clitk::PrintMemory(GetVerboseMemoryFlag(), "End of step"); 
  if (GetVerboseImageSizeFlag()) {
    std::ostream & os = std::cout;
    unsigned int dim = p->GetImageDimension();
    int nb = 1;
    os << txt << " size = ";
    for(unsigned int i=0; i<dim-1; i++) {
      os << p->GetLargestPossibleRegion().GetSize()[i] << "x";
      nb *= p->GetLargestPossibleRegion().GetSize()[i];
    }
    os << p->GetLargestPossibleRegion().GetSize()[dim-1] << "  ";
    nb *= p->GetLargestPossibleRegion().GetSize()[dim-1];
    os << " pixels = " << nb << std::endl;    
  }
}
//--------------------------------------------------------------------


