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

#ifndef CLITKIMAGE2DICOMRTSTRUCTFILTER_H
#define CLITKIMAGE2DICOMRTSTRUCTFILTER_H

// clitk
#include "clitkDicomRT_ROI.h"
#include "clitkImageCommon.h"
#include "clitkFilterBase.h"
#include "clitkDicomRT_StructureSet.h"

namespace clitk {

  //--------------------------------------------------------------------
  template<class PixelType>
  class Image2DicomRTStructFilter: public clitk::FilterBase {
    
  public:
    Image2DicomRTStructFilter();
    ~Image2DicomRTStructFilter();

    typedef itk::Image<PixelType, 3> ImageType;
    typedef typename ImageType::Pointer ImagePointer;
    typedef typename clitk::DicomRT_StructureSet::Pointer DicomRTStructPointer;

    // Set inputs
    itkSetMacro(Input, ImagePointer);
    itkGetConstMacro(Input, ImagePointer);
    
    // Run filter
    void Update();    
    
    // Get output
    itkGetConstMacro(DicomRTStruct, DicomRTStructPointer);

  protected:
    ImagePointer m_Input;
    DicomRTStructPointer m_DicomRTStruct;
  };
  //--------------------------------------------------------------------

} // end namespace clitk

#include "clitkImage2DicomRTStructFilter.txx"

#endif // CLITKIMAGE2DICOMRTSTRUCTFILTER_H

