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
#if GDCM_MAJOR_VERSION == 2
  void Read(std::map<int, std::string> & rois, gdcm::Item const & item);
#else
  void Read(std::map<int, std::string> & rois, gdcm::SQItem * item);
#endif
  void SetFromBinaryImage(vvImage * image, int n, 
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
  
  void SetImage(vvImage * im);
  DicomRT_Contour* GetContour(int n);

  double GetContourSpacing() const {return mZDelta;}
  
protected:
  void ComputeMesh();
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
  ///Spacing between two contours
  double mZDelta;

private:
  DicomRT_ROI();
  ~DicomRT_ROI();
};
//--------------------------------------------------------------------

} // end namespace clitk
#endif // CLITKDICOMRT_ROI_H

