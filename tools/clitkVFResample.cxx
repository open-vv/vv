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
#ifndef CLITKVFRESAMPLE_CXX
#define CLITKVFRESAMPLE_CXX

// clitk
#include "clitkVFResample_ggo.h"
#include "clitkIO.h"
#include "clitkVFResampleGenericFilter.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[])
{

  // Init command line
  GGO(clitkVFResample, args_info);
  CLITK_INIT;

  // Read input image header to check image dimension
  itk::ImageIOBase::Pointer header = clitk::readImageHeader(args_info.input_arg);
  unsigned int dim = header->GetNumberOfDimensions();
  std::string pixelTypeName = header->GetComponentTypeAsString(header->GetComponentType());

  // Print image info if verbose
  if (args_info.verbose_flag) {
    std::cout << "Input image <" << args_info.input_arg << "> is ";
    clitk::printImageHeader(header, std::cout);
    std::cout << std::endl;
  }

  // Get input size/spacing
  std::vector<int>    inputSize;
  std::vector<double> inputSpacing;
  inputSize.resize(dim);
  inputSpacing.resize(dim);
  for(unsigned int i=0; i<dim; i++) {
    inputSpacing[i] = header->GetSpacing(i);
    inputSize[i] = header->GetDimensions(i);
  }

  // Get options
  std::vector<int>    outputSize;
  std::vector<double> outputSpacing;
  outputSize.resize(dim);
  outputSpacing.resize(dim);

  // Check options
  if (!args_info.size_given && !args_info.spacing_given) {
    std::cerr << "Please indicate output size or spacing." << std::endl;
    exit(0);
  }

  // Check options
  if (args_info.size_given && args_info.spacing_given) {
    std::cerr << "Please indicate only output size or spacing, not both." << std::endl;
    exit(0);
  }

  // Size is given and not spacing
  if (args_info.size_given && !args_info.spacing_given) {
    if (args_info.size_given != dim) {
      std::cerr << "Input image is " << dim << "D, please give " << dim << " size numbers." << std::endl;
      exit(0);
    }
    for(unsigned int i=0; i<dim; i++) {
      if ((args_info.size_arg[i] == -1) || (args_info.size_arg[i]==0))
        outputSize[i] = inputSize[i];
      else
        outputSize[i] = args_info.size_arg[i];
      outputSpacing[i] = inputSize[i]*inputSpacing[i]/outputSize[i];
    }
  }

  // Spacing is given and not size
  if (!args_info.size_given && args_info.spacing_given) {
    if (args_info.spacing_given != dim) {
      if (args_info.spacing_given == 1) {
        for(unsigned int i=0; i<dim; i++) outputSpacing[i] = args_info.spacing_arg[0];
      } else {
        std::cerr << "Input image is " << dim << "D, please give " << dim << " spacing numbers." << std::endl;
        exit(0);
      }
    } else {
      for(unsigned int i=0; i<dim; i++) {
        if ((args_info.spacing_arg[i] == -1) || (args_info.spacing_arg[i]==0))
          outputSpacing[i] = inputSpacing[i];
        else
          outputSpacing[i] = args_info.spacing_arg[i];

      }
    }
    for(unsigned int i=0; i<dim; i++)
      outputSize[i] = (int)lrint(inputSize[i]*inputSpacing[i]/outputSpacing[i]);
  }

  if (args_info.verbose_flag) {
    std::cout << "Output image will be : " << std::endl;
    DDV(outputSize,dim);
    DDV(outputSpacing,dim);
  }

  // Get sigma option for Gaussian filter
  std::vector<double> sigma;
  sigma.resize(args_info.gauss_given);
  std::copy(args_info.gauss_arg, args_info.gauss_arg+args_info.gauss_given, sigma.begin());
  if (args_info.gauss_given) {
    if (args_info.gauss_given != dim) {
      if (args_info.gauss_given == 1) {
        sigma.resize(dim);
        for(unsigned int i=0; i<dim; i++) sigma[i] = args_info.gauss_arg[0];
      } else {
        std::cerr << "Input image is " << dim << "D, please give " << dim << " sigma numbers for gaussian filter." << std::endl;
        exit(0);
      }
    }
  }

  // Create a filter
  clitk::VFResampleGenericFilter::Pointer filter = clitk::VFResampleGenericFilter::New();
  filter->SetInputFilename(args_info.input_arg);
  filter->SetOutputSize(outputSize);
  filter->SetOutputSpacing(outputSpacing);
  filter->SetInterpolationName(args_info.interp_arg);
  filter->SetBSplineOrder(args_info.order_arg);
  filter->SetBLUTSampling(args_info.sampling_arg);
  if (args_info.gauss_given)
    filter->SetGaussianSigma(sigma);
  filter->SetOutputFilename(args_info.output_arg);

  // Go !
  filter->Update();

  // this is the end my friend
  return 0;
}// end main
//--------------------------------------------------------------------

#endif  /* end #define CLITKVFRESAMPLE_CXX */
