/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/
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
#include <itkExtractImageFilter.h>

//--------------------------------------------------------------------
clitk::GuerreroVentilationGenericFilter::GuerreroVentilationGenericFilter()
  :ImageToImageGenericFilter<Self>("GuerreroVentilationGenericFilter")
{
  blood_mass_factor=1.;
  InitializeImageType<2>();
  InitializeImageType<3>();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<unsigned int Dim>
void clitk::GuerreroVentilationGenericFilter::InitializeImageType()
{
  ADD_IMAGE_TYPE(Dim, short);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void clitk::GuerreroVentilationGenericFilter::UpdateWithInputImageType()
{

  // Input should be 2
  assert(mInputFilenames.size() == 2);

  // Reading input
  typedef ImageType InputImageType;
  typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);
  typename InputImageType::Pointer ref = this->template GetInput<InputImageType>(1);

  typedef itk::Image<float,InputImageType::ImageDimension> OutputImageType;
  //    typename ImageType::Pointer input = clitk::readImage<ImageType>(mInputFilenames[0], mIOVerbose);
  //typename ImageType::Pointer ref = clitk::readImage<ImageType>(mInputFilenames[1], mIOVerbose);

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
