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

#include "clitkDicomRT_Contour.h"
#include <vtkCellArray.h>

#if GDCM_MAJOR_VERSION >= 2
#include "gdcmAttribute.h"
#include "gdcmItem.h"
#endif

//--------------------------------------------------------------------
clitk::DicomRT_Contour::DicomRT_Contour()
{
  mMeshIsUpToDate = false;
  mNbOfPoints = 0;
  mZ = -1;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
clitk::DicomRT_Contour::~DicomRT_Contour()
{

}
//--------------------------------------------------------------------



//--------------------------------------------------------------------
void clitk::DicomRT_Contour::Print(std::ostream & os) const
{
  DD("TODO : print Contours");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
#if GDCM_MAJOR_VERSION >= 2
void clitk::DicomRT_Contour::UpdateDicomItem()
{
  DD("DicomRT_Contour::UpdateDicomItem");

  gdcm::DataSet & nestedds = mItem->GetNestedDataSet();

  //NON CONSIDER CONTOUR ITEM NOT SEQ ITEM ?

  // Contour type [Contour Geometric Type]
  gdcm::Attribute<0x3006,0x0042> contgeotype;
  contgeotype.SetFromDataSet( nestedds );

  // Number of points [Number of Contour Points]
  gdcm::Attribute<0x3006,0x0046> numcontpoints;
  numcontpoints.SetFromDataSet( nestedds );
  DD(mNbOfPoints);
  mNbOfPoints = numcontpoints.GetValue();
  DD(mNbOfPoints);

  // Contour dicom values from DataPoints
  //ComputeDataPointsFromMesh();
  uint nb = mData->GetNumberOfPoints();
  std::vector<double> points;
  points.resize(mNbOfPoints*3);
  for(unsigned int i=0; i<nb; i++) {
    double * p = mData->GetPoint(i);
    points[i*3] = p[0];
    points[i*3+1] = p[1];
#if VTK_MAJOR_VERSION <= 5
    points[i*3+1] = p[2];
#else
    points[i*3+1] = p[2]-0.5;
#endif
  }

  // Get attribute
  gdcm::Attribute<0x3006,0x0050> at;
  gdcm::Tag tcontourdata(0x3006,0x0050);
  gdcm::DataElement contourdata = nestedds.GetDataElement( tcontourdata );
  at.SetFromDataElement( contourdata );

  // Set attribute
  at.SetValues(&points[0], points.size(), false);
  DD(at.GetValues()[0]);
  
  DD("replace");
  nestedds.Replace(at.GetAsDataElement());
  DD("done");

  // Change Number of points [Number of Contour Points]
  numcontpoints.SetValue(nb);
  nestedds.Replace(numcontpoints.GetAsDataElement());

  // Test
  gdcm::DataElement aa = nestedds.GetDataElement( tcontourdata );
  at.SetFromDataElement( aa );
  const double* bb = at.GetValues();
  DD(bb[0]);

}
#endif
//--------------------------------------------------------------------


//--------------------------------------------------------------------
#if GDCM_MAJOR_VERSION >= 2
bool clitk::DicomRT_Contour::Read(gdcm::Item * item)
{
  mItem = item;
  
  const gdcm::DataSet& nestedds2 = item->GetNestedDataSet();

  // Contour type [Contour Geometric Type]
  gdcm::Attribute<0x3006,0x0042> contgeotype;
  contgeotype.SetFromDataSet( nestedds2 );

  if (contgeotype.GetValue() != "CLOSED_PLANAR " && contgeotype.GetValue() != "POINT ") { ///WARNING to the space after the name ...
    //std::cerr << "Skip this contour : type=" << mType << std::endl;
    return false;
  }
  if (contgeotype.GetValue() == "POINT ") {
    std::cerr << "Warning: POINT type not fully supported. (don't use GetMesh() with this!)"
      << std::endl;
  }

  gdcm::Attribute<0x3006,0x0046> numcontpoints;
  numcontpoints.SetFromDataSet( nestedds2 );
  // Number of points [Number of Contour Points]
  mNbOfPoints = numcontpoints.GetValue();
  // DD(mNbOfPoints);

  gdcm::Attribute<0x3006,0x0050> at;
  gdcm::Tag tcontourdata(0x3006,0x0050);
  const gdcm::DataElement & contourdata = nestedds2.GetDataElement( tcontourdata );
  at.SetFromDataElement( contourdata );
  const double* points = at.GetValues();
  //  unsigned int npts = at.GetNumberOfValues() / 3;
  assert(at.GetNumberOfValues() == static_cast<unsigned int>(mNbOfPoints)*3);

  // Organize values
  mData = vtkSmartPointer<vtkPoints>::New();
  mData->SetDataTypeToDouble();
  mData->SetNumberOfPoints(mNbOfPoints);
  for(unsigned int i=0; i<mNbOfPoints; i++) {
    double p[3];
    p[0] = points[i*3];
    p[1] = points[i*3+1];
#if VTK_MAJOR_VERSION <= 5
    p[2] = points[i*3+2];
#else
    p[2] = points[i*3+2]+0.5;
#endif
    mData->SetPoint(i, p);
    if (mZ == -1) mZ = p[2];
    if (std::fabs(p[2] - mZ) > mTolerance) {
      DD(i);
      DD(p[2]);
      DD(mZ);
      std::cout << "ERROR ! contour not in the same slice" << std::endl;
      assert(p[2] == mZ);
    }
  }

  return true;

}
#else
bool clitk::DicomRT_Contour::Read(gdcm::SQItem * item)
{

  // Contour type [Contour Geometric Type]
  mType = item->GetEntryValue(0x3006,0x0042);
  // DD(mType);
  if (mType != "CLOSED_PLANAR " && mType != "POINT ") { ///WARNING to the space after the name ...
    //std::cerr << "Skip this contour : type=" << mType << std::endl;
    return false;
  }
  if (mType == "POINT ") {
    std::cerr << "Warning: POINT type not fully supported. (don't use GetMesh() with this!)"
      << std::endl;
  }

  // Number of points [Number of Contour Points]
  mNbOfPoints = parse_value<int>(item->GetEntryValue(0x3006,0x0046));
  // DD(mNbOfPoints);

  // Read values [Contour Data]
  std::vector<float> points = parse_string<float>(item->GetEntryValue(0x3006,0x0050),'\\');
  assert(points.size() == static_cast<unsigned int>(mNbOfPoints)*3);

  // Organize values
  mData = vtkSmartPointer<vtkPoints>::New();
  mData->SetDataTypeToDouble();
  mData->SetNumberOfPoints(mNbOfPoints);
  for(unsigned int i=0; i<mNbOfPoints; i++) {
    double p[3];
    p[0] = points[i*3];
    p[1] = points[i*3+1];
#if VTK_MAJOR_VERSION <= 5
    p[2] = points[i*3+2];
#else
    p[2] = points[i*3+2]+0.5;
#endif
    mData->SetPoint(i, p);
    if (mZ == -1) mZ = p[2];
    if (std::fabs(p[2] - mZ) > mTolerance) {
      DD(i);
      DD(p[2]);
      DD(mZ);
      std::cout << "ERROR ! contour not in the same slice" << std::endl;
      assert(p[2] == mZ);
    }
  }

  return true;
}
#endif
//--------------------------------------------------------------------


//--------------------------------------------------------------------
vtkPolyData * clitk::DicomRT_Contour::GetMesh()
{
  if (!mMeshIsUpToDate) {
    ComputeMeshFromDataPoints();
  }
  return mMesh;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_Contour::SetMesh(vtkPolyData * mesh)
{
  mMesh = mesh;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_Contour::SetTransformMatrix(vtkMatrix4x4* matrix)
{
  mTransformMatrix = matrix;
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
double clitk::DicomRT_Contour::GetTolerance()
{
  return mTolerance;
}
//--------------------------------------------------------------------
void clitk::DicomRT_Contour::SetTolerance(double tol)
{
  mTolerance = tol;
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void clitk::DicomRT_Contour::ComputeMeshFromDataPoints()
{
//  DD("ComputeMesh Contour");
  mMesh = vtkSmartPointer<vtkPolyData>::New();
  mMesh->Allocate(); //for cell structures
  mPoints = vtkSmartPointer<vtkPoints>::New();
  mMesh->SetPoints(mPoints);
  vtkIdType ids[2];
  for (unsigned int idx=0 ; idx<mNbOfPoints ; idx++) {
    double pointIn[4];
    for (unsigned int j=0 ; j<3; ++j)
      pointIn[j] = mData->GetPoint(idx)[j];
    pointIn[3] = 1.0;
    /*double pointOut[4];
    mTransformMatrix->MultiplyPoint(pointIn, pointOut);
    std::cout << pointOut[0] << " " << pointOut[1] << " " << pointOut[2] << " " << pointOut[3] << std::endl;
    mMesh->GetPoints()->InsertNextPoint(pointOut[0],
                                        pointOut[1],
                                        pointOut[2]);*/
    mMesh->GetPoints()->InsertNextPoint(mData->GetPoint(idx)[0],
                                        mData->GetPoint(idx)[1],
                                        mData->GetPoint(idx)[2]);
    //std::cout << mData->GetPoint(idx)[0] << " " << mData->GetPoint(idx)[1] << " " << mData->GetPoint(idx)[2] << std::endl;
    ids[0]=idx;
    ids[1]=(ids[0]+1) % mNbOfPoints; //0-1,1-2,...,n-1-0
    mMesh->GetLines()->InsertNextCell(2,ids);
  }
  //std::cout << std::endl;
  mMeshIsUpToDate = true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_Contour::ComputeDataPointsFromMesh()
{
  DD("ComputeDataPointsFromMesh");


  /*todo

  mMesh = vtkSmartPointer<vtkPolyData>::New();
  mMesh->Allocate(); //for cell structures
  mPoints = vtkSmartPointer<vtkPoints>::New();
  mMesh->SetPoints(mPoints);
  vtkIdType ids[2];
  for (unsigned int idx=0 ; idx<mNbOfPoints ; idx++) {
    mMesh->GetPoints()->InsertNextPoint(mData->GetPoint(idx)[0],
                                        mData->GetPoint(idx)[1],
                                        mData->GetPoint(idx)[2]);
    ids[0]=idx;
    ids[1]=(ids[0]+1) % mNbOfPoints; //0-1,1-2,...,n-1-0
    mMesh->GetLines()->InsertNextCell(2,ids);
  }
  mMeshIsUpToDate = true;
*/
}
//--------------------------------------------------------------------
