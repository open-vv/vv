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

#ifndef CLITKDICOMRT_ROI_H
#define CLITKDICOMRT_ROI_H

#include "clitkDicomRT_Contour.h"
#include "vvImage.h"

namespace clitk {

//--------------------------------------------------------------------
class DicomRT_ROI : public itk::LightObject
{
  
public:
  typedef DicomRT_ROI Self;
  typedef itk::SmartPointer<Self> Pointer;
  itkNewMacro(Self);

  void Print(std::ostream & os = std::cout) const;
  void SetFromBinaryImage(vvImage::Pointer image, int n, 
        std::string name, 
        std::vector<double> color, 
        std::string filename);

  int GetROINumber() const;
  const std::string & GetName() const;
  const std::string & GetFilename() const;
  const std::vector<double> & GetDisplayColor() const;
  vtkPolyData * GetMesh();
  vvImage * GetImage() const;

  void SetDisplayColor(double r, double v, double b);
  std::vector<double> & GetDisplayColor() { return mColor; }
  
  double GetBackgroundValueLabelImage() const;
  void SetBackgroundValueLabelImage(double bg);
  
  double GetForegroundValueLabelImage() const;
  void SetForegroundValueLabelImage(double bg);
  
  void SetImage(vvImage::Pointer im);
  DicomRT_Contour* GetContour(int n);

  // Compute a vtk mesh from the dicom contours
  void ComputeMeshFromContour();
  void ComputeContoursFromImage();
  
  // Indicate if the mesh is uptodate according to the dicom
  void SetDicomUptodateFlag(bool b) { m_DicomUptodateFlag = b; }
  bool GetDicomUptoDateFlag() const { return m_DicomUptodateFlag; }
  void SetName(std::string n) { mName = n; }

  // Read from DICOM RT STRUCT
#if GDCM_MAJOR_VERSION == 2
  bool Read(gdcm::Item * itemInfo, gdcm::Item * itemContour);
  void UpdateDicomItem();
#else
  void Read(std::map<int, std::string> & rois, gdcm::SQItem * item);
#endif

protected:
  std::string mName;
  std::string mFilename;
  int mNumber;
  std::vector<double> mColor;
  std::vector<DicomRT_Contour::Pointer> mListOfContours;
  vtkSmartPointer<vtkPolyData> mMesh;
  bool mMeshIsUpToDate;
  vvImage::Pointer mImage;
  double mBackgroundValue;
  double mForegroundValue;
  bool m_DicomUptodateFlag;

#if GDCM_MAJOR_VERSION == 2
  gdcm::Item * mItemInfo;
  gdcm::Item * mItemContour;
  gdcm::SmartPointer<gdcm::SequenceOfItems> mContoursSequenceOfItems;
#endif

private:
  DicomRT_ROI();
  ~DicomRT_ROI();
};
//--------------------------------------------------------------------

} // end namespace clitk
#endif // CLITKDICOMRT_ROI_H

