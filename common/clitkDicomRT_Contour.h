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

#ifndef CLITKDICOMRT_CONTOUR_H
#define CLITKDICOMRT_CONTOUR_H

#include "clitkCommon.h" 
#include <gdcmFile.h>
#if GDCM_MAJOR_VERSION >= 2
#else
#include <gdcm.h>
#include <gdcmSQItem.h>
#endif
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>

namespace clitk {

//--------------------------------------------------------------------
class DicomRT_Contour : public itk::LightObject{
  
public:
  typedef DicomRT_Contour Self;
  typedef itk::SmartPointer<Self> Pointer;
  itkNewMacro(Self);

  void Print(std::ostream & os = std::cout) const;

#if GDCM_MAJOR_VERSION >= 2
  bool Read(gdcm::Item * item);
  void UpdateDicomItem();
#else
  bool Read(gdcm::SQItem * item);
#endif

  vtkPolyData * GetMesh();
  void SetMesh(vtkPolyData * mesh);
  vtkPoints * GetPoints() {return mData;}
  double GetZ() const {return mZ;}
  void SetTransformMatrix(vtkMatrix4x4* matrix);
  double GetTolerance();
  void SetTolerance(double tol);
  
  
protected:
  void ComputeMeshFromDataPoints();
  void ComputeDataPointsFromMesh();
  unsigned int mNbOfPoints;
  std::string mType;
  vtkSmartPointer<vtkPoints> mData;
  vtkSmartPointer<vtkPolyData> mMesh;
  vtkSmartPointer<vtkPoints> mPoints;
  vtkSmartPointer<vtkMatrix4x4> mTransformMatrix;
  bool mMeshIsUpToDate;
  ///Z location of the contour
  double mZ;
  double mTolerance;
  
#if GDCM_MAJOR_VERSION >= 2
  gdcm::Item * mItem;
#else
  gdcm::SQItem * mItem;
#endif

private:
  DicomRT_Contour();
  ~DicomRT_Contour();
};
//--------------------------------------------------------------------



} // end namespace clitk
#endif // CLITKDICOMRT_CONTOUR_H

