/*=========================================================================
  Program:         vv http://www.creatis.insa-lyon.fr/rio/vv
  Main authors :   XX XX XX

  Authors belongs to: 
  - University of LYON           http://www.universite-lyon.fr/
  - Léon Bérard cancer center    http://www.centreleonberard.fr
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

// clitk
#include "clitkCommon.h" 
#include "clitkDicomRT_ROI.h"

// vv
#include "vvImage.h"

// gdcm
#if GDCM_MAJOR_VERSION == 2
#include "gdcmReader.h"
#include "gdcmWriter.h"
#include "gdcmAttribute.h"
#endif

namespace clitk {

//--------------------------------------------------------------------
class DicomRT_StructureSet : public itk::LightObject{
  
public:
  typedef DicomRT_StructureSet Self;
  typedef itk::SmartPointer<Self> Pointer;
  itkNewMacro(Self);

  typedef std::map<int, clitk::DicomRT_ROI::Pointer> ROIMapContainer;
  typedef ROIMapContainer::iterator ROIIteratorType;
  typedef ROIMapContainer::const_iterator ROIConstIteratorType;

  void Print(std::ostream & os = std::cout) const;
  void Read(const std::string & filename);
  bool IsDicomRTStruct(const std::string & filename);
  void Write(const std::string & filename);

  clitk::DicomRT_ROI * GetROIFromROINumber(int n);
  clitk::DicomRT_ROI* GetROIFromROIName(const std::string& name);
  //clitk::DicomRT_ROI* GetROIFromROINameRegEx(const std::string& regEx);
  clitk::DicomRT_ROI* GetROIFromROINameSubstr(const std::string& s);
  ROIMapContainer * GetROIsFromROINameSubstr(const std::string& s);
  ROIMapContainer & GetROIs() { return mROIs; }
  const std::string & GetStudyID() const;
  const std::string & GetStudyTime() const;
  const std::string & GetStudyDate() const;
  const std::string & GetLabel() const;
  const std::string & GetName() const;
  const std::string & GetDate() const;
  const std::string & GetTime() const;

  int AddBinaryImageAsNewROI(vvImage * i, std::string name);
  
#if GDCM_MAJOR_VERSION == 2
  // Static
  static int ReadROINumber(const gdcm::Item & item);
#endif

protected:
  std::string mStudyID;
  std::string mStudyTime;
  std::string mStudyDate;
  std::string mLabel;
  std::string mName;
  std::string mDate;
  std::string mTime;

  std::map<int, clitk::DicomRT_ROI::Pointer> mROIs;
  std::map<int, std::string> mMapOfROIName;
#if GDCM_MAJOR_VERSION == 2
  gdcm::Reader * mReader;
  gdcm::SmartPointer<gdcm::SequenceOfItems> mROIInfoSequenceOfItems;
  gdcm::SmartPointer<gdcm::SequenceOfItems> mROIContoursSequenceOfItems;  
#endif
  gdcm::File * mFile;

private:
  DicomRT_StructureSet();
  ~DicomRT_StructureSet();
};
//--------------------------------------------------------------------

} // end namespace clitk
#endif // CLITKDICOMRT_STRUCTURESET_H

