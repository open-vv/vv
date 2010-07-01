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

#ifndef CLITKDICOMRT_CONTOUR_H
#define CLITKDICOMRT_CONTOUR_H

#include "clitkCommon.h" 
#include "clitkDicomRT_Contour.h"
#include <gdcm.h>
#include <gdcmSQItem.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

namespace clitk {

  //--------------------------------------------------------------------
  class DicomRT_Contour {
    
  public:
    DicomRT_Contour();
    ~DicomRT_Contour();

    void Print(std::ostream & os = std::cout) const;
    bool Read(gdcm::SQItem * item);
    vtkPolyData * GetMesh();
    
  protected:
    void ComputeMesh();
    unsigned int mNbOfPoints;
    std::string mType;
    vtkPoints * mData;
    vtkPolyData * mMesh;
    bool mMeshIsUpToDate;

  };
  //--------------------------------------------------------------------



} // end namespace clitk
#endif // CLITKDICOMRT_CONTOUR_H

