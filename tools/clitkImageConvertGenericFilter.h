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
#ifndef CLITKIMAGECONVERTGENERICFILTER_H
#define CLITKIMAGECONVERTGENERICFILTER_H
/**
 ===================================================================
 * @file   clitkImageConvertGenericFilter.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   05 May 2008 10:40:24

 * @brief  

 ===================================================================*/

// clitk include
#include "clitkImageToImageGenericFilter.h"

// itk include
#include "itkCastImageFilter.h"
#include "itkVectorCastImageFilter.h"


namespace clitk {
  
  template <class TPixel>
  class ImageConvertTraits
  {
  public:
    enum { IS_VECTOR = false };
    ImageConvertTraits() {
      TPixel p = "SCALAR";
    }
  };

  template < class TPixel, unsigned int Comp > 
  class ImageConvertTraits< itk::Vector<TPixel, Comp> > 
  { 
  public: 
    enum { IS_VECTOR = true }; 
  };

  class ImageConvertGenericFilter: 
    public clitk::ImageToImageGenericFilter<ImageConvertGenericFilter> {
    
  public: 
    // constructor - destructor
    ImageConvertGenericFilter();

    // Types
    typedef ImageConvertGenericFilter     Self;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    // New
    itkNewMacro(Self);
    
    // Members functions
    std::string GetInputPixelTypeName() { return m_PixelTypeName; }
    std::string GetOutputPixelTypeName() { return mOutputPixelTypeName; }
    void SetOutputPixelType(std::string p) { mOutputPixelTypeName = p; }
    bool IsWarningOccur() { return mWarningOccur; }
    std::string & GetWarning() { return mWarning; }
    void EnableDisplayWarning(bool b) { mDisplayWarning = b; }

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>  
    void UpdateWithInputImageType();

    template<class PixelType, class OutputPixelType>
    void CheckTypes(std::string inType, std::string outType);
    
  protected:

    template<unsigned int Dim> void InitializeImageType();
    std::string mOutputPixelTypeName;
    std::string mWarning;
    bool mWarningOccur;
    bool mDisplayWarning;

  private:
    template <class InputImageType, bool isVector>
    class UpdateWithSelectiveOutputType
    {
    public:
      static bool Run(ImageConvertGenericFilter& filter, std::string outputPixelType)
      {
          if (IsSameType<char>(outputPixelType))
            UpdateWithOutputType<char>(filter);
          else if (IsSameType<uchar>(outputPixelType))
            UpdateWithOutputType<uchar>(filter);
          else if (IsSameType<short>(outputPixelType))
            UpdateWithOutputType<short>(filter);
          else if (IsSameType<ushort>(outputPixelType))
            UpdateWithOutputType<ushort>(filter);
          else if (IsSameType<int>(outputPixelType))
            UpdateWithOutputType<int>(filter);
          else if (IsSameType<float>(outputPixelType))
            UpdateWithOutputType<float>(filter);
          else if (IsSameType<double>(outputPixelType))
            UpdateWithOutputType<double>(filter);
          else
          {
            std::string list = CreateListOfTypes<float, double>();
            std::cerr << "Error, I don't know the vector output type '" << outputPixelType 
                      << "'. " << std::endl << "Known types are " << list << "." << std::endl;
            return false;
          }
          
          return true;
      }

    private:
      
      template <class OutputPixelType> 
      static void UpdateWithOutputType(ImageConvertGenericFilter& filter)
      {
        // Read
        typename InputImageType::Pointer input =filter.template GetInput<InputImageType>(0);

        // Typedef
        typedef typename InputImageType::PixelType PixelType;

        // Warning
        filter.CheckTypes<PixelType, OutputPixelType>(filter.GetInputPixelTypeName(), filter.GetOutputPixelTypeName());

        // Cast
        typedef itk::Image<OutputPixelType,InputImageType::ImageDimension> OutputImageType;
        typedef itk::CastImageFilter<InputImageType, OutputImageType> FilterType;
        typename FilterType::Pointer cast_filter = FilterType::New();
        cast_filter->SetInput(input);
        cast_filter->Update();

        // Write
        filter.SetNextOutput<OutputImageType>(cast_filter->GetOutput());
      }
    };
    
    template <class InputImageType>
    class UpdateWithSelectiveOutputType<InputImageType, true>
    {
    public:
      static bool Run(ImageConvertGenericFilter& filter, std::string outputPixelType)
      {
        /*
        // RP: future conversions? 
        if (IsSameType<char>(outputPixelType))
          UpdateWithOutputVectorType<char>();
        else if (IsSameType<uchar>(outputPixelType))
          UpdateWithOutputVectorType<uchar>();
        else if (IsSameType<short>(outputPixelType))
          UpdateWithOutputVectorType<short>();
        else if (IsSameType<ushort>(outputPixelType))
          UpdateWithOutputVectorType<ushort>();
        else if (IsSameType<int>(outputPixelType))
          UpdateWithOutputVectorType<int>();
        else 
          */
        if (IsSameType<float>(outputPixelType))
          UpdateWithOutputVectorType<float>(filter);
        else if (IsSameType<double>(outputPixelType))
          UpdateWithOutputVectorType<double>(filter);
        else
        {
          std::string list = CreateListOfTypes<float, double>();
          std::cerr << "Error, I don't know the vector output type '" << outputPixelType  
                    << "'. " << std::endl << "Known types are " << list << "." << std::endl;
          return false;
        }
        
        return true;
      }
      
    private:
      
      template <class OutputPixelType> 
      static void UpdateWithOutputVectorType(ImageConvertGenericFilter& filter)
      {
        // Read
        typename InputImageType::Pointer input =filter.template GetInput<InputImageType>(0);

        // Typedef
        typedef typename InputImageType::PixelType::ValueType PixelType;

        // Warning
        filter.CheckTypes<PixelType, OutputPixelType>(filter.GetInputPixelTypeName(), filter.GetOutputPixelTypeName());
        
        // Cast
        typedef itk::Image<itk::Vector<OutputPixelType, InputImageType::PixelType::Dimension>, InputImageType::ImageDimension> OutputImageType;
        typedef itk::VectorCastImageFilter<InputImageType, OutputImageType> FilterType;
        typename FilterType::Pointer cast_filter = FilterType::New();
        cast_filter->SetInput(input);
        cast_filter->Update();

        // Write
        filter.SetNextOutput<OutputImageType>(cast_filter->GetOutput());
      }
    };
  }; // end class ImageConvertGenericFilter

//#include "clitkImageConvertGenericFilter.txx"

} // end namespace

#endif /* end #define CLITKIMAGECONVERTGENERICFILTER_H */

