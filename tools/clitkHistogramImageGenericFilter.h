/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
===========================================================================**/

#ifndef clitkHistogramImageGenericFilter_h
#define clitkHistogramImageGenericFilter_h

#include "clitkIO.h"
#include "clitkImageToImageGenericFilter.h"
#include "clitkHistogramImage_ggo.h"

#include <vtkIntArray.h>
#include <vtkFloatArray.h>
#include <itkHistogram.h>

//--------------------------------------------------------------------
namespace clitk
{

  class ITK_EXPORT HistogramImageGenericFilter:
    public ImageToImageGenericFilter<HistogramImageGenericFilter>
  {

  public:
    //--------------------------------------------------------------------
    typedef HistogramImageGenericFilter         Self;
    typedef itk::SmartPointer<Self>           Pointer;
    typedef itk::SmartPointer<const Self>     ConstPointer;
    typedef args_info_clitkHistogramImage       args_info_type;

    //--------------------------------------------------------------------
    // Method for creation through the object factory
    // and Run-time type information (and related methods)
    itkNewMacro(Self);  
    itkTypeMacro(HistogramImageGenericFilter, LightObject);

    //--------------------------------------------------------------------
    void SetArgsInfo(const args_info_type & a);
    void SetSizeBin (const double size);

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>  
    void UpdateWithInputImageType();

    vtkFloatArray* GetArrayX();
    vtkFloatArray* GetArrayY();

  protected:
    HistogramImageGenericFilter();
    template<unsigned int Dim> void InitializeImageType();
    args_info_type mArgsInfo;
    
    double mBinSize;
    
    vtkSmartPointer<vtkFloatArray> mArrayX;
    vtkSmartPointer<vtkFloatArray> mArrayY;

  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#endif // #define clitkHistogramImageGenericFilter_h
