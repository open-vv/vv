/*=========================================================================

  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
=========================================================================*/

/**
 -------------------------------------------------------------------
 * @file   clitkGuerreroVentilationGenericFilter.cxx
 * @author Joël Schaerer
 * @date   20 April 2009

 * @brief  
 -------------------------------------------------------------------*/

#include "clitkGuerreroVentilationGenericFilter.h"
#include <itkBinaryGuerreroFilter.h>
#include <itkImageDuplicator.h>
//--------------------------------------------------------------------
clitk::GuerreroVentilationGenericFilter::GuerreroVentilationGenericFilter() 
{
    blood_mass_factor=1.;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::GuerreroVentilationGenericFilter::Update () {
  
  // Determine dim, pixel type, number of components
  this->GetInputImageDimensionAndPixelType(mDim,mPixelTypeName,mNbOfComponents);
  
  // Switch by dimension
  if (mDim == 3) { Update_WithDim<3>(); return; }
  if (mDim == 2) { Update_WithDim<2>(); return; }
  std::cerr << "Error, dimension of input image is " << mDim << ", but I only work with 2 or 3." << std::endl;
  exit(0);
}
//--------------------------------------------------------------------

//This is where you put the actual implementation

#include <sstream>
#include <itkExtractImageFilter.h>


//--------------------------------------------------------------------
template<unsigned int Dim>
void clitk::GuerreroVentilationGenericFilter::Update_WithDim() { 
#define TRY_TYPE(TYPE)							\
  if (IsSameType<TYPE>(mPixelTypeName)) { Update_WithDimAndPixelType<Dim, TYPE>(); return; } 
  // TRY_TYPE(signed char);
  // TRY_TYPE(uchar);
  TRY_TYPE(short);
  //TRY_TYPE(ushort);
  // TRY_TYPE(int);
//   TRY_TYPE(unsigned int); 
  //TRY_TYPE(float);
  // TRY_TYPE(double);
#undef TRY_TYPE

  std::string list = CreateListOfTypes<uchar, short, ushort, int, uint, float, double>();
  std::cerr << "Error, I don't know the type '" << mPixelTypeName << "' for the input image '"
	    << mInputFilenames[0] << "'." << std::endl << "Known types are " << list << std::endl;
  exit(0);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<unsigned int Dim, class PixelType>
void clitk::GuerreroVentilationGenericFilter::Update_WithDimAndPixelType() {

    // Read input
    assert(mInputFilenames.size() == 2);
    typedef itk::Image<PixelType,Dim> ImageType;
    typedef itk::Image<float,Dim> OutputImageType;
    typename ImageType::Pointer input = clitk::readImage<ImageType>(mInputFilenames[0], mIOVerbose);
    typename ImageType::Pointer ref = clitk::readImage<ImageType>(mInputFilenames[1], mIOVerbose);


    typedef itk::BinaryGuerreroFilter<ImageType,ImageType,OutputImageType> GFilterType;
    typename GFilterType::Pointer filter = GFilterType::New();
    filter->SetInput1(ref);
    filter->SetInput2(input);
    filter->SetBloodCorrectionFactor(blood_mass_factor);
    filter->SetUseCorrectFormula(use_correct_formula);
    filter->Update();
    this->SetNextOutput<OutputImageType>(filter->GetOutput());
    //clitk::writeImage<OutputImageType>(filter->GetOutput(), mOutputFilename, mIOVerbose);
    //std::cout << "Warning: removed " << filter->GetFunctor().aberant_voxels << " aberant voxels from the ventilation image"
        //<< std::endl;
}
