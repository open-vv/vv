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

#ifndef CLITKXML2DICOMRTSTRUCTFILTER_H
#define CLITKXML2DICOMRTSTRUCTFILTER_H

// clitk
#include "clitkDicomRT_ROI.h"
#include "clitkImageCommon.h"
#include "clitkFilterBase.h"
#include "clitkDicomRT_StructureSet.h"

namespace clitk {

  //--------------------------------------------------------------------
  template<class PixelType>
  class Xml2DicomRTStructFilter: public clitk::FilterBase {

  public:
    Xml2DicomRTStructFilter();
    ~Xml2DicomRTStructFilter();

    typedef itk::Image<PixelType, 3> ImageType;
    typedef typename ImageType::Pointer ImagePointer;
    typedef typename clitk::DicomRT_StructureSet::Pointer DicomRTStructPointer;

    itkSetMacro(InputFilename, std::string);
    itkSetMacro(StructureSetFilename, std::string);
    itkSetMacro(DicomFolder, std::string);
    itkSetMacro(OutputFilename, std::string);

    // Run filter
    void Update();

  protected:
    std::string m_StructureSetFilename;
    std::string m_DicomFolder;
    std::string m_OutputFilename;
    std::string m_InputFilename;
  };
  //--------------------------------------------------------------------

} // end namespace clitk

#include "clitkXml2DicomRTStructFilter.txx"

#endif // CLITKXML2DICOMRTSTRUCTFILTER_H

