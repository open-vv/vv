/*=========================================================================
  Program:         vv http://www.creatis.insa-lyon.fr/rio/vv
  Main authors :   XX XX XX

  Authors belongs to: 
  - University of LYON           http://www.universite-lyon.fr/
  - Léon Bérard cancer center    http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory      http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence
  - BSD       http://www.opensource.org/licenses/bsd-license.php
  - CeCILL-B  http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html

  =========================================================================*/

#ifndef CLITKDICOMRT_STRUCTURESET_H
#define CLITKDICOMRT_STRUCTURESET_H

#include "clitkCommon.h" 
#include "clitkDicomRT_ROI.h"
#include "vvImage.h"

namespace clitk {

//--------------------------------------------------------------------
class DicomRT_StructureSet : public itk::LightObject{
  
public:
  typedef DicomRT_StructureSet Self;
  typedef itk::SmartPointer<Self> Pointer;
  itkNewMacro(Self);

  void Print(std::ostream & os = std::cout) const;
  void Read(const std::string & filename);

  const std::vector<DicomRT_ROI::Pointer> & GetListOfROI() const;
  clitk::DicomRT_ROI * GetROI(int n);
  const std::string & GetStudyID() const;
  const std::string & GetStudyTime() const;
  const std::string & GetStudyDate() const;
  const std::string & GetLabel() const;
  const std::string & GetName() const;
  const std::string & GetDate() const;
  const std::string & GetTime() const;

  int AddBinaryImageAsNewROI(vvImage * i, std::string name);
  
protected:
  std::string mStudyID;
  std::string mStudyTime;
  std::string mStudyDate;
  std::string mLabel;
  std::string mName;
  std::string mDate;
  std::string mTime;
  std::map<int, std::string> mMapOfROIName;
  std::map<int, int> mMapOfROIIndex;
  std::vector<clitk::DicomRT_ROI::Pointer> mListOfROI;

private:
  DicomRT_StructureSet();
  ~DicomRT_StructureSet();
};
//--------------------------------------------------------------------

} // end namespace clitk
#endif // CLITKDICOMRT_STRUCTURESET_H

