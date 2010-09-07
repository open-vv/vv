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
clitk::DicomRT_ROI::DicomRT_ROI()
{
  mName = "NoName";
  mNumber = -1;
  mColor.resize(3);
  mColor[0] = mColor[1] = mColor[2] = 0;
  mMeshIsUpToDate = false;
  mBackgroundValue = 0;
  mForegroundValue = 1;
  mZDelta = 0;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
clitk::DicomRT_ROI::~DicomRT_ROI()
{

}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI::SetDisplayColor(double r, double v, double b)
{
  mColor.resize(3);
  mColor[0] = r;
  mColor[1] = v;
  mColor[2] = b;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
int clitk::DicomRT_ROI::GetROINumber() const
{
  return mNumber;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::string & clitk::DicomRT_ROI::GetName() const
{
  return mName;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::string & clitk::DicomRT_ROI::GetFilename() const
{
  return mFilename;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::vector<double> & clitk::DicomRT_ROI::GetDisplayColor() const
{
  return mColor;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI::Print(std::ostream & os) const
{
  os << "ROI " << mNumber << "\t" << mName
     << "\t(" << mColor[0] << " " << mColor[1] << " " << mColor[2] << ")"
     << "\t Contours = " << mListOfContours.size() << std::endl;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI::SetBackgroundValueLabelImage(double bg)
{
  mBackgroundValue = bg;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
double clitk::DicomRT_ROI::GetBackgroundValueLabelImage() const
{
  return mBackgroundValue;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI::SetForegroundValueLabelImage(double bg)
{
  mForegroundValue = bg;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
double clitk::DicomRT_ROI::GetForegroundValueLabelImage() const
{
  return mForegroundValue;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI::Read(std::map<int, std::string> & rois, gdcm::SQItem * item)
{

  // Change number if needed

  // TODO

  // ROI number [Referenced ROI Number]
  mNumber = atoi(item->GetEntryValue(0x3006,0x0084).c_str());

  // Retrieve ROI Name
  mName = rois[mNumber];

  // ROI Color [ROI Display Color]
  mColor = clitk::parse_string<double>(item->GetEntryValue(0x3006,0x002a),'\\');

  // Read contours [Contour Sequence]
  gdcm::SeqEntry * contours=item->GetSeqEntry(0x3006,0x0040);
  bool contour_processed=false;
  bool delta_computed=false;
  double last_z=0;
  for(gdcm::SQItem* j=contours->GetFirstSQItem(); j!=0; j=contours->GetNextSQItem()) {
    DicomRT_Contour * c = new DicomRT_Contour;
    bool b = c->Read(j);
    if (b) {
      mListOfContours.push_back(c);
      if (contour_processed) {
        double delta=c->GetZ() - last_z;
        if (delta_computed)
          assert(mZDelta == delta);
        else
          mZDelta = delta;
      } else
        contour_processed=true;
      last_z=c->GetZ();
    }
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI::SetImage(vvImage * image)
{
  mImage = image;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
vtkPolyData * clitk::DicomRT_ROI::GetMesh()
{
  if (!mMeshIsUpToDate) {
    ComputeMesh();
  }
  return mMesh;
}
//--------------------------------------------------------------------
clitk::DicomRT_Contour * clitk::DicomRT_ROI::GetContour(int n)
{
  return mListOfContours[n];
}

//--------------------------------------------------------------------
void clitk::DicomRT_ROI::ComputeMesh()
{
  vtkAppendPolyData * append = vtkAppendPolyData::New();
  for(unsigned int i=0; i<mListOfContours.size(); i++) {
    append->AddInput(mListOfContours[i]->GetMesh());
  }
  append->Update();
  mMesh = append->GetOutput();
  mMeshIsUpToDate = true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI::SetFromBinaryImage(vvImage::Pointer image, int n,
					    std::string name,
					    std::vector<double> color, 
					    std::string filename)
{

  // ROI number [Referenced ROI Number]
  mNumber = n;

  // ROI Name
  mName = name;
  mFilename = filename;

  // ROI Color [ROI Display Color]
  mColor = color;

  // No contours [Contour Sequence]
  mListOfContours.clear();

  // Set image
  mImage = image;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const vvImage::Pointer clitk::DicomRT_ROI::GetImage() const
{
  return mImage;
}
//--------------------------------------------------------------------
