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

#include "clitkDicomRT_Contour.h"
#include <vtkCellArray.h>

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
#if GDCM_MAJOR_VERSION == 2
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
    p[2] = points[i*3+2];
    mData->SetPoint(i, p);
    if (mZ == -1) mZ = p[2];
    if (p[2] != mZ) {
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
    ComputeMesh();
  }
  return mMesh;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_Contour::ComputeMesh()
{
//  DD("ComputeMesh Contour");
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
    // DD(ids[0]);
//     DD(ids[1]);
    mMesh->GetLines()->InsertNextCell(2,ids);
  }
  // DD(mMesh->GetNumberOfCells());
  mMeshIsUpToDate = true;
}
//--------------------------------------------------------------------
