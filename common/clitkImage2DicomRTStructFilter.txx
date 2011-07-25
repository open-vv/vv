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

// std
#include <iterator>
#include <algorithm>

// clitk
#include "clitkImage2DicomRTStructFilter.h"
#include "clitkImageCommon.h"

// vtk
#include <vtkPolyDataToImageStencil.h>
#include <vtkSmartPointer.h>
#include <vtkImageStencil.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkMetaImageWriter.h>
#include <vtkImageData.h>
#include <itkImage.h>
#include <itkVTKImageToImageFilter.h>

//--------------------------------------------------------------------
template<class PixelType>
clitk::Image2DicomRTStructFilter<PixelType>::Image2DicomRTStructFilter()
{
  DD("Image2DicomRTStructFilter Const");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class PixelType>
clitk::Image2DicomRTStructFilter<PixelType>::~Image2DicomRTStructFilter()
{
  DD("Image2DicomRTStructFilter Destructor");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class PixelType>
void clitk::Image2DicomRTStructFilter<PixelType>::Update() 
{
  DD("Image2DicomRTStructFilter::Update");


}
//--------------------------------------------------------------------




