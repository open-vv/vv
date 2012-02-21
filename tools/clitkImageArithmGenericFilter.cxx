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
#ifndef CLITKIMAGEARITHMGENERICFILTER_CXX
#define CLITKIMAGEARITHMGENERICFILTER_CXX

#include "clitkImageArithmGenericFilter.h"

namespace clitk {
  // Specialisation
//   template<>
//   class ImageArithmGenericFilter<args_info_clitkImageArithm>;

  template<>
  template<>
  void ImageArithmGenericFilter<args_info_clitkImageArithm>::UpdateWithInputImageType< itk::Image< itk::Vector<float, 3u>, 3u > >()
  {
    typedef itk::Image< itk::Vector<float, 3u>, 3u > ImageType;
    
    // Read input1
    ImageType::Pointer input1 = this->GetInput<ImageType>(0);

    // Set input image iterator
    typedef itk::ImageRegionIterator<ImageType> IteratorType;
    IteratorType it(input1, input1->GetLargestPossibleRegion());

    // typedef input2
    ImageType::Pointer input2 = NULL;
    IteratorType it2;

    /*
    // Special case for normalisation
    if (mTypeOfOperation == 12) {
      typedef itk::MinimumMaximumImageCalculator<ImageType> MinMaxFilterType;
      typename MinMaxFilterType::Pointer ff = MinMaxFilterType::New();
      ff->SetImage(input1);
      ff->ComputeMaximum();
      mScalar = ff->GetMaximum();
      mTypeOfOperation = 11; // divide
    }
    */

    if (mIsOperationUseASecondImage) {
        // Read input2
        input2 = this->GetInput<ImageType>(1);
        // Set input image iterator
        it2 = IteratorType(input2, input2->GetLargestPossibleRegion());
        // Check dimension
        if (!clitk::HaveSameSize<ImageType, ImageType>(input1, input2)) {
          itkExceptionMacro(<< "The images (input and input2) must have the same size");
        }
        if(!clitk::HaveSameSpacing<ImageType, ImageType>(input1, input2)) {
          itkWarningMacro(<< "The images (input and input2) do not have the same spacing. "
                          << "Using first input's information.");
        }
    }

    /*
    // Check if overwrite and outputisfloat and pixeltype is not float -> do not overwrite
    if (mOverwriteInputImage && mOutputIsFloat && (typeid(typename ImageType::PixelType) != typeid(float))) {
      // std::cerr << "Warning. Could not use both mOverwriteInputImage and mOutputIsFloat, because input is "
      //                     << typeid(PixelType).name()
      //                     << std::endl;
      mOverwriteInputImage = false;
    }
    */
    
    // ---------------- Overwrite input Image ---------------------
    if (mOverwriteInputImage) {
      // Set output iterator (to input1)
      IteratorType ito = IteratorType(input1, input1->GetLargestPossibleRegion());
      if (mIsOperationUseASecondImage) ComputeImage(it, it2, ito);
      else ComputeImage(it, ito);
      this->SetNextOutput<ImageType>(input1);
    }
    // ---------------- Create new output Image ---------------------
    else {
      /*if (mOutputIsFloat) {
        // Create output image
        typedef itk::Image<float,ImageType::ImageDimension> OutputImageType;
        typename OutputImageType::Pointer output = OutputImageType::New();
        output->SetRegions(input1->GetLargestPossibleRegion());
        output->SetOrigin(input1->GetOrigin());
        output->SetSpacing(input1->GetSpacing());
        output->Allocate();
        // Set output iterator
        typedef itk::ImageRegionIterator<OutputImageType> IteratorOutputType;
        IteratorOutputType ito = IteratorOutputType(output, output->GetLargestPossibleRegion());
        if (mIsOperationUseASecondImage) ComputeImage(it, it2, ito);
        else ComputeImage(it, ito);
        this->template SetNextOutput<OutputImageType>(output);
      } else*/ {
        // Create output image
        typedef ImageType OutputImageType;
        OutputImageType::Pointer output = OutputImageType::New();
        output->SetRegions(input1->GetLargestPossibleRegion());
        output->SetOrigin(input1->GetOrigin());
        output->SetSpacing(input1->GetSpacing());
        output->Allocate();
        // Set output iterator
        typedef itk::ImageRegionIterator<OutputImageType> IteratorOutputType;
        IteratorOutputType ito = IteratorOutputType(output, output->GetLargestPossibleRegion());
        if (mIsOperationUseASecondImage) ComputeImage(it, it2, ito);
        else ComputeImage(it, ito);
        this->SetNextOutput<OutputImageType>(output);
      }
    }
  }

  template<>
  template<>
  void ImageArithmGenericFilter<args_info_clitkImageArithm>::ComputeImage< 
    itk::ImageRegionIterator< itk::Image< itk::Vector<float, 3u>, 3u > >, 
    itk::ImageRegionIterator< itk::Image< itk::Vector<float, 3u>, 3u > > >
    (itk::ImageRegionIterator< itk::Image< itk::Vector<float, 3u>, 3u > > it, 
    itk::ImageRegionIterator< itk::Image< itk::Vector<float, 3u>, 3u > > ito)
  {
    typedef itk::ImageRegionIterator< itk::Image< itk::Vector<float, 3u>, 3u > > Iter2;

    ito.GoToBegin();
    it.GoToBegin();
    
    typedef Iter2::PixelType PixelType;

    PixelType scalar_vector;
    scalar_vector.Fill(mScalar);
    
    // Perform operation
    switch (mTypeOfOperation) {
    case 0: // Addition
      while (!it.IsAtEnd()) {
        ito.Set(it.Get() + scalar_vector);
        ++it;
        ++ito;
      }
      break;
    case 1: // Multiply
      while (!it.IsAtEnd()) {
        ito.Set(it.Get() * mScalar);
        ++it;
        ++ito;
      }
      break;
      /*
    case 2: // Inverse
      while (!it.IsAtEnd()) {
        if (it.Get() != 0)
          ito.Set(mScalar / it.Get()));
        else ito.Set(mDefaultPixelValue);
        ++it;
        ++ito;
      }
      break;
    case 3: // Max
      while (!it.IsAtEnd()) {
        if (it.Get() < mScalar) ito.Set(PixelTypeDownCast<double, PixelType>(mScalar));
        else ito.Set(PixelTypeDownCast<double, PixelType>(it.Get()));
        ++it;
        ++ito;
      }
      break;
    case 4: // Min
      while (!it.IsAtEnd()) {
        if (it.Get() > mScalar) ito.Set(PixelTypeDownCast<double, PixelType>(mScalar));
        else ito.Set(PixelTypeDownCast<double, PixelType>(it.Get()));
        ++it;
        ++ito;
      }
      break;
    case 5: // Absolute value
      while (!it.IsAtEnd()) {
        if (it.Get() <= 0) ito.Set(PixelTypeDownCast<double, PixelType>(-it.Get()));
        // <= zero to avoid warning for unsigned types
        else ito.Set(PixelTypeDownCast<double, PixelType>(it.Get()));
        ++it;
        ++ito;
      }
      break;
    case 6: // Squared value
      while (!it.IsAtEnd()) {
        ito.Set(PixelTypeDownCast<double, PixelType>((double)it.Get()*(double)it.Get()));
        ++it;
        ++ito;
      }
      break;
    case 7: // Log
      while (!it.IsAtEnd()) {
        if (it.Get() > 0)
          ito.Set(PixelTypeDownCast<double, PixelType>(log((double)it.Get())));
        else ito.Set(mDefaultPixelValue);
        ++it;
        ++ito;
      }
      break;
    case 8: // exp
      while (!it.IsAtEnd()) {
        ito.Set(PixelTypeDownCast<double, PixelType>(exp((double)it.Get())));
        ++it;
        ++ito;
      }
      break;
    case 9: // sqrt
      while (!it.IsAtEnd()) {
        if (it.Get() > 0)
          ito.Set(PixelTypeDownCast<double, PixelType>(sqrt((double)it.Get())));
        else {
          if (it.Get() ==0) ito.Set(0);
          else ito.Set(mDefaultPixelValue);
        }
        ++it;
        ++ito;
      }
      break;
    case 10: // exp
      while (!it.IsAtEnd()) {
        ito.Set(PixelTypeDownCast<double, PixelType>((0x10000 - (double)it.Get())/mScalar));
        ++it;
        ++ito;
      }
      break;
      */
    case 11: // divide
      while (!it.IsAtEnd()) {
        ito.Set(it.Get() / mScalar);
        ++it;
        ++ito;
      }
      break;
    default: // error ?
      std::cerr << "ERROR : the operation number (" << mTypeOfOperation << ") is not known." << std::endl;
      exit(-1);
    }
    
  }

  template<>
  template<>
  void ImageArithmGenericFilter<args_info_clitkImageArithm>::ComputeImage< 
    itk::ImageRegionIterator< itk::Image< itk::Vector<float, 3u>, 3u > >, 
    itk::ImageRegionIterator< itk::Image< itk::Vector<float, 3u>, 3u > >, 
    itk::ImageRegionIterator< itk::Image< itk::Vector<float, 3u>, 3u > > 
    >
    (itk::ImageRegionIterator< itk::Image< itk::Vector<float, 3u>, 3u > > it1, 
    itk::ImageRegionIterator< itk::Image< itk::Vector<float, 3u>, 3u > > it2, 
    itk::ImageRegionIterator< itk::Image< itk::Vector<float, 3u>, 3u > > ito)
  {
    typedef itk::ImageRegionIterator< itk::Image< itk::Vector<float, 3u>, 3u > > Iter1;
    typedef itk::ImageRegionIterator< itk::Image< itk::Vector<float, 3u>, 3u > > Iter2;
    typedef itk::ImageRegionIterator< itk::Image< itk::Vector<float, 3u>, 3u > > Iter3;
    
    it1.GoToBegin();
    it2.GoToBegin();
    ito.GoToBegin();
    typedef Iter3::PixelType PixelType;

    switch (mTypeOfOperation) {
    case 0: // Addition
      while (!ito.IsAtEnd()) {
        ito.Set(it1.Get() + it2.Get());
        ++it1;
        ++it2;
        ++ito;
      }
      break;
      /*
    case 1: // Multiply
      while (!ito.IsAtEnd()) {
        ito.Set(it1.Get() * it2.Get()) );
        ++it1;
        ++it2;
        ++ito;
      }
      break;
    case 2: // Divide
      while (!ito.IsAtEnd()) {
        if (it1.Get() != 0)
          ito.Set(it1.Get() / it2.Get()));
        else ito.Set(mDefaultPixelValue);
        ++it1;
        ++it2;
        ++ito;
      }
      break;
    case 3: // Max
      while (!ito.IsAtEnd()) {
        if (it1.Get() < it2.Get()) ito.Set(it2.Get());
        ++it1;
        ++it2;
        ++ito;
      }
      break;
    case 4: // Min
      while (!ito.IsAtEnd()) {
        if (it1.Get() > it2.Get()) ito.Set(it2.Get());
        ++it1;
        ++it2;
        ++ito;
      }
      break;
      */
    case 5: // Absolute difference
      while (!ito.IsAtEnd()) {
        ito.Set(it2.Get()-it1.Get());
        ++it1;
        ++it2;
        ++ito;
      }
      break;
      /*
    case 6: // Squared differences
      while (!ito.IsAtEnd()) {
        ito.Set(pow(it1.Get()-it2.Get(),2)));
        ++it1;
        ++it2;
        ++ito;
      }
      break;
      */
    case 7: // Difference
      while (!ito.IsAtEnd()) {
        ito.Set(it1.Get()-it2.Get());
        ++it1;
        ++it2;
        ++ito;
      }
      break;
      /*
    case 8: // Relative Difference
      while (!ito.IsAtEnd()) {
        if (it1.Get() != 0) ito.Set(PixelTypeDownCast<double, PixelType>(((double)it1.Get()-(double)it2.Get()))/(double)it1.Get());
        else ito.Set(0.0);
        ++it1;
        ++it2;
        ++ito;
      }
      break;
      */
    default: // error ?
      std::cerr << "ERROR : the operation number (" << mTypeOfOperation << ") is not known." << std::endl;
      exit(-1);
    }
  }

}

#endif //define CLITKIMAGEARITHMGENERICFILTER_CXX
