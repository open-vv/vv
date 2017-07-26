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
#ifndef CLITKVectorArithmGENERICFILTER_TXX
#define CLITKVectorArithmGENERICFILTER_TXX

#include "clitkImageCommon.h"

#include "itkMinimumMaximumImageCalculator.h"

namespace clitk
{

//--------------------------------------------------------------------
template<class args_info_type>
VectorArithmGenericFilter<args_info_type>::VectorArithmGenericFilter()
  :ImageToImageGenericFilter<Self>("VectorArithmGenericFilter"),mTypeOfOperation(0)
{
  InitializeImageType<3>();
  mIsOperationUseASecondImage = false;
  mIsOutputScalar = false;
  mOverwriteInputImage = true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dim>
void VectorArithmGenericFilter<args_info_type>::InitializeImageType()
{
  ADD_VEC_IMAGE_TYPE(Dim,3u,float);
  ADD_VEC_IMAGE_TYPE(Dim,3u,double);
  ADD_VEC_IMAGE_TYPE(Dim,2u,float);
  ADD_VEC_IMAGE_TYPE(Dim,2u,double);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
void VectorArithmGenericFilter<args_info_type>::EnableOverwriteInputImage(bool b)
{
  mOverwriteInputImage = b;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
void VectorArithmGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a)
{
  mArgsInfo=a;

  // Set value
  this->SetIOVerbose(mArgsInfo.verbose_flag);
  mTypeOfOperation = mArgsInfo.operation_arg;
  mDefaultPixelValue = mArgsInfo.pixelValue_arg;
  mScalar = mArgsInfo.scalar_arg;
  mOutputIsFloat = mArgsInfo.setFloatOutput_flag;

  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();

  if (mArgsInfo.input1_given) this->AddInputFilename(mArgsInfo.input1_arg);
  if (mArgsInfo.input2_given) {
    mIsOperationUseASecondImage = true;
    this->AddInputFilename(mArgsInfo.input2_arg);
    if (mArgsInfo.operation_arg == 2)
      mIsOutputScalar = true;
  } 
  else if (mArgsInfo.operation_arg == 5 || mArgsInfo.operation_arg == 6)
    mIsOutputScalar = true;

  if (mArgsInfo.output_given) {
    this->SetOutputFilename(mArgsInfo.output_arg);
    mOverwriteInputImage = false;
  }

  // Check type of operation (with scalar or with other image)
  if ((mArgsInfo.input2_given) && (mArgsInfo.scalar_given)) {
    std::cerr << "ERROR : you cannot provide both --scalar and --input2 option" << std::endl;
    exit(-1);
  }
  if ((!mArgsInfo.input2_given) && (!mArgsInfo.scalar_given)) {
    if (mArgsInfo.operation_arg < 5) {
      std::cerr << "Such operation need the --scalar option." << std::endl;
      exit(-1);
    }
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<class ImageType>
void VectorArithmGenericFilter<args_info_type>::UpdateWithInputImageType()
{
  // Read input1
  typename ImageType::Pointer input1 = this->template GetInput<ImageType>(0);

  // Set input image iterator
  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  IteratorType it(input1, input1->GetLargestPossibleRegion());

  // typedef input2
  typename ImageType::Pointer input2 = NULL;
  IteratorType it2;

  // Special case for normalisation
  /*
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
      input2 = this->template GetInput<ImageType>(1);
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

  // Check if overwrite and outputisfloat and pixeltype is not float -> do not overwrite
  if (mOverwriteInputImage && mOutputIsFloat && (typeid(typename ImageType::PixelType) != typeid(float))) {
    // std::cerr << "Warning. Could not use both mOverwriteInputImage and mOutputIsFloat, because input is "
    //                     << typeid(PixelType).name()
    //                     << std::endl;
    mOverwriteInputImage = false;
  }

  // ---------------- Overwrite input Image ---------------------
  if (mOverwriteInputImage && !mIsOutputScalar) {
    // Set output iterator (to input1)
    IteratorType ito = IteratorType(input1, input1->GetLargestPossibleRegion());
    if (mIsOperationUseASecondImage) ComputeImage(it, it2, ito);
    else ComputeImage(it, ito);
    this->template SetNextOutput<ImageType>(input1);
  }

  // ---------------- Create new output Image ---------------------
  else {
    // Create output image
    if (!mIsOutputScalar) {
      typedef ImageType OutputImageType;
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
    }
    else {
      // Create scalar output image
      typedef itk::Image<typename ImageType::PixelType::ValueType, ImageType::ImageDimension> OutputImageType;
      typename OutputImageType::Pointer output = OutputImageType::New();
      output->SetRegions(input1->GetLargestPossibleRegion());
      output->SetOrigin(input1->GetOrigin());
      output->SetSpacing(input1->GetSpacing());
      output->Allocate();
      // Set output iterator
      typedef itk::ImageRegionIterator<OutputImageType> IteratorOutputType;
      IteratorOutputType ito = IteratorOutputType(output, output->GetLargestPossibleRegion());
      if (mIsOperationUseASecondImage) ComputeScalarImage(it, it2, ito);
      else ComputeScalarImage(it, ito);
      this->template SetNextOutput<OutputImageType>(output);
    }
  }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class args_info_type>
template<class Iter1, class Iter2, class Iter3>
void  VectorArithmGenericFilter<args_info_type>::ComputeImage(Iter1 it1, Iter2 it2, Iter3 ito)
{
  it1.GoToBegin();
  it2.GoToBegin();
  ito.GoToBegin();
  typedef typename Iter3::PixelType PixelType;

  switch (mTypeOfOperation) {
  case 0: // Addition
    while (!ito.IsAtEnd()) {
      ito.Set(it1.Get() + it2.Get());
      ++it1;
      ++it2;
      ++ito;
    }
    break;

  case 1: // term to term Multiply
    while (!ito.IsAtEnd()) {
      typename Iter1::PixelType outputPixel(ito.Get());
      outputPixel.SetVnlVector(element_product(it1.Get().GetVnlVector(),it2.Get().GetVnlVector()));
      ito.Set(outputPixel);
      ++it1;
      ++it2;
      ++ito;
    }
    break;

    /*
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
    /*
  case 5: // Absolute difference
    while (!ito.IsAtEnd()) {
      ito.Set(it2.Get()-it1.Get());
      ++it1;
      ++it2;
      ++ito;
    }
    break;
    */
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
    /*
  case 9: // CrossProduct
    while (!ito.IsAtEnd()) {
      ito.Set(it1.Get()^it2.Get());
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
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<class Iter1, class Iter2>
void clitk::VectorArithmGenericFilter<args_info_type>::ComputeImage(Iter1 it, Iter2 ito)
{
  ito.GoToBegin();
  it.GoToBegin();
  
  typedef typename Iter1::PixelType PixelType;

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
    */
    /*
  case 5: // Absolute value
    while (!it.IsAtEnd()) {
      ito.Set(PixelTypeDownCast<double, PixelType>(it.GetNorm()));
      ++it;
      ++ito;
    }
    break;
  case 6: // Squared value
    while (!it.IsAtEnd()) {
      ito.Set(PixelTypeDownCast<double, PixelType>(it.GetSquaredNorm());
      ++it;
      ++ito;
    }
    break;
    */
    /*
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
  case 12: // normalize
    while (!it.IsAtEnd()) {
      PixelType n = it.Get();
      if (n.GetNorm() != 0)
        n.Normalize();
      
      ito.Set(n);
      ++it;
      ++ito;
    }
    break;
  default: // error ?
    std::cerr << "ERROR : the operation number (" << mTypeOfOperation << ") is not known." << std::endl;
    exit(-1);
  }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class args_info_type>
template<class Iter1, class Iter2, class Iter3>
void  VectorArithmGenericFilter<args_info_type>::ComputeScalarImage(Iter1 it1, Iter2 it2, Iter3 ito)
{
  it1.GoToBegin();
  it2.GoToBegin();
  ito.GoToBegin();
  typedef typename Iter3::PixelType PixelType;

  switch (mTypeOfOperation) {
  case 2: // Multiply
    while (!ito.IsAtEnd()) {
      ito.Set(it1.Get() * it2.Get());
      ++it1;
      ++it2;
      ++ito;
    }
    break;
  default: // error ?
    std::cerr << "ERROR : the operation number (" << mTypeOfOperation << ") is not known." << std::endl;
    exit(-1);
  }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class args_info_type>
template<class Iter1, class Iter2>
void clitk::VectorArithmGenericFilter<args_info_type>::ComputeScalarImage(Iter1 it, Iter2 ito)
{
  ito.GoToBegin();
  it.GoToBegin();
  
  typedef typename Iter2::PixelType PixelType;

 
  // Perform operation
  switch (mTypeOfOperation) {
  case 5: // Absolute value
    while (!it.IsAtEnd()) {
      ito.Set(PixelTypeDownCast<double, PixelType>(it.Get().GetNorm()));
      ++it;
      ++ito;
    }
    break;
  case 6: // Squared value
    while (!it.IsAtEnd()) {
      ito.Set(PixelTypeDownCast<double, PixelType>(it.Get().GetSquaredNorm()));
      ++it;
      ++ito;
    }
    break;
  default: // error ?
    std::cerr << "ERROR : the operation number (" << mTypeOfOperation << ") is not known." << std::endl;
    exit(-1);
  }
}
//--------------------------------------------------------------------


} // end namespace

#endif  //#define CLITKVectorArithmGENERICFILTER_TXX
