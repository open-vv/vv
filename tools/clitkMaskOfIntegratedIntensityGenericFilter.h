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

#ifndef CLITKMaskOfIntegratedIntensityGENERICFILTER_H
#define CLITKMaskOfIntegratedIntensityGENERICFILTER_H

#include "clitkIO.h"
#include "clitkImageToImageGenericFilter.h"

//--------------------------------------------------------------------
namespace clitk
{

  template<class args_info_type>
    class ITK_EXPORT MaskOfIntegratedIntensityGenericFilter:
    public ImageToImageGenericFilter<MaskOfIntegratedIntensityGenericFilter<args_info_type> >
    {

    public:

      //--------------------------------------------------------------------
      MaskOfIntegratedIntensityGenericFilter();

      //--------------------------------------------------------------------
      typedef MaskOfIntegratedIntensityGenericFilter         Self;
      typedef itk::SmartPointer<Self>                      Pointer;
      typedef itk::SmartPointer<const Self>                ConstPointer;

      //--------------------------------------------------------------------
      // Method for creation through the object factory
      // and Run-time type information (and related methods)
      itkNewMacro(Self);
      itkTypeMacro(MaskOfIntegratedIntensityGenericFilter, LightObject);

      //--------------------------------------------------------------------
      void SetArgsInfo(const args_info_type & a);

      void SetPercentage(double p) { mPercentage = p; }
      double GetPercentage() const { return mPercentage; }

      //--------------------------------------------------------------------
      // Main function called each time the filter is updated
      template<class InputImageType>
        void UpdateWithInputImageType();

    protected:
      template<unsigned int Dim> void InitializeImageType();
      args_info_type mArgsInfo;
      double mPercentage;

    }; // end class
  //--------------------------------------------------------------------

//Implementation of the pair comparative function
template <typename T> bool comparator ( const std::pair<T, size_t>& l, const std::pair<T, size_t>& r);

} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkMaskOfIntegratedIntensityGenericFilter.txx"
#endif

#endif // #define clitkMaskOfIntegratedIntensityGenericFilter_h
