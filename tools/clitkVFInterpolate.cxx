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
#ifndef CLITKVFRESAMPLE_CXX
#define CLITKVFRESAMPLE_CXX

// clitk
#include "clitkVFInterpolate_ggo.h"
#include "clitkIO.h"
#include "clitkVFInterpolateGenericFilter.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[])
{

    // Init command line
    GGO(clitkVFInterpolate, args_info);
    CLITK_INIT;

    // Read input image header to check image dimension
    itk::ImageIOBase::Pointer header = clitk::readImageHeader(args_info.input1_arg);
    unsigned int dim = header->GetNumberOfDimensions();
    std::string pixelTypeName = header->GetComponentTypeAsString(header->GetComponentType());

    // Print image info if verbose
    if (args_info.verbose_flag) {
        std::cout << "Input image <" << args_info.input1_arg << "> is ";
        clitk::printImageHeader(header, std::cout);
        std::cout << std::endl;
    }

    // Get input size/spacing
    std::vector<int>    inputSize;
    std::vector<double> inputSpacing;
    inputSize.resize(dim);
    inputSpacing.resize(dim);
    for (unsigned int i=0; i<dim; i++) {
        inputSpacing[i] = header->GetSpacing(i);
        inputSize[i] = header->GetDimensions(i);
    }

    if (args_info.verbose_flag) {
        std::cout << "Output image will be : " << std::endl;
    }

    // Create a filter
    clitk::VFInterpolateGenericFilter::Pointer filter = clitk::VFInterpolateGenericFilter::New();
    filter->SetInputFilename(args_info.input1_arg);
    filter->SetInputFilename2(args_info.input2_arg);
    filter->SetInterpolationName(args_info.interp_arg);
    filter->SetDistance(args_info.distance_arg);
//     filter->SetBSplineOrder(args_info.order_arg);
//     filter->SetBLUTSampling(args_info.sampling_arg);
    filter->SetOutputFilename(args_info.output_arg);

    // Go !
    filter->Update();

    // this is the end my friend
    return 0;
}// end main
//--------------------------------------------------------------------

#endif  /* end #define CLITKVFRESAMPLE_CXX */
