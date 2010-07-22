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

#include "clitkDicomRT_ROI.h" 
#include <vtkSmartPointer.h>
#include <vtkAppendPolyData.h>

//--------------------------------------------------------------------
clitk::DicomRT_ROI::DicomRT_ROI() {
  mName = "NoName";
  mNumber = -1;
  mColor.resize(3);
  mColor[0] = mColor[1] = mColor[2] = 0;
  mMeshIsUpToDate = false;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
clitk::DicomRT_ROI::~DicomRT_ROI() {
  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
int clitk::DicomRT_ROI::GetROINumber() const {
  return mNumber;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::string & clitk::DicomRT_ROI::GetName() const {
  return mName;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::vector<double> & clitk::DicomRT_ROI::GetDisplayColor() const {
  return mColor;
}
//--------------------------------------------------------------------


 
//--------------------------------------------------------------------
void clitk::DicomRT_ROI::Print(std::ostream & os) const {
  os << "ROI " << mNumber << "\t" << mName 
     << "\t(" << mColor[0] << " " << mColor[1] << " " << mColor[2] << ")"
     << "\t Contours = " << mListOfContours.size() << std::endl;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI::Read(std::map<int, std::string> & rois, gdcm::SQItem * item) {
  
  // ROI number [Referenced ROI Number]
  mNumber = atoi(item->GetEntryValue(0x3006,0x0084).c_str());
  
  // Retrieve ROI Name
  mName = rois[mNumber];

  // ROI Color [ROI Display Color]
  mColor = clitk::parse_string<double>(item->GetEntryValue(0x3006,0x002a),'\\');

  // Read contours [Contour Sequence]
  gdcm::SeqEntry * contours=item->GetSeqEntry(0x3006,0x0040);
  for(gdcm::SQItem* j=contours->GetFirstSQItem();j!=0;j=contours->GetNextSQItem()) {
    DicomRT_Contour * c = new DicomRT_Contour;    
    bool b = c->Read(j);
    if (b) mListOfContours.push_back(c);
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
vtkPolyData * clitk::DicomRT_ROI::GetMesh() {
  if (!mMeshIsUpToDate) {
    ComputeMesh();
  }
  return mMesh;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI::ComputeMesh() {
  vtkAppendPolyData * append = vtkAppendPolyData::New();
  for(unsigned int i=0; i<mListOfContours.size(); i++) {
    append->AddInput(mListOfContours[i]->GetMesh());
  }
  append->Update();
  mMesh = append->GetOutput();
  mMeshIsUpToDate = true;
}
//--------------------------------------------------------------------

