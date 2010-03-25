#ifndef CLITKIMAGEEXTRACTLINE_CXX
#define CLITKIMAGEEXTRACTLINE_CXX
/**
   -------------------------------------------------
   * @file   clitkImageExtractLine.cxx
   * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
   * @date   23 Feb 2008 08:37:53
   -------------------------------------------------*/

// clitk include
#include "clitkImageExtractLine_ggo.h"
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include <itkLineConstIterator.h>

//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkImageExtractLine, args_info);
  CLITK_INIT;

  // Declare main types
  typedef float PixelType;
  const unsigned int Dimension=3;
  typedef itk::Image<PixelType, Dimension> ImageType;
  
  // Check options
  if (args_info.firstIndex_given != Dimension) {
    std::cerr << "Please give " << Dimension << "values to --firstIndex option" << std::endl;
    exit(0);
  }
  if (args_info.lastIndex_given != Dimension) {
    std::cerr << "Please give " << Dimension << "values to --lastIndex option" << std::endl;
    exit(0);
  }

  // Read image
  ImageType::Pointer input = clitk::readImage<ImageType>(args_info.input_arg, args_info.verbose_flag);

  // Get first and last index
  typedef ImageType::IndexType IndexType;
  IndexType firstIndex;
  IndexType lastIndex;
  ImageType::SpacingType spacing = input->GetSpacing();
  double length = 0.0;
  for(unsigned int i=0; i<Dimension; i++) {
    firstIndex[i] = args_info.firstIndex_arg[i];
    lastIndex[i] = args_info.lastIndex_arg[i];    
    if (args_info.mm_flag) {
      firstIndex[i] /= spacing[i];
      lastIndex[i] /= spacing[i];
    }
    length += pow(lastIndex[i]*spacing[i]-firstIndex[i]*spacing[i],2);
  }
  length = sqrt(length);

  // Loop
  std::vector<double> depth;
  std::vector<double> values;
  itk::LineConstIterator<ImageType> iter(input, firstIndex, lastIndex);
  iter.GoToBegin();
  while (!iter.IsAtEnd()) {
    values.push_back(iter.Get());
    ++iter;
  }
  double step = length/values.size();
  
  // If isocenter is used
  double isoDistance = 0.0;
  if (args_info.isocenter_given) { // isoCenter is in mm
    IndexType isoCenter;
    for(unsigned int i=0; i<Dimension; i++) { 
      isoCenter[i] = args_info.isocenter_arg[i]; 
      isoDistance += pow(isoCenter[i] - firstIndex[i]*spacing[i],2);
    }
    DD(isoCenter);
    isoDistance = sqrt(isoDistance);
    DD(isoDistance);
  }

  // Write result
  std::ofstream os(args_info.output_arg);
  os << "# clitkImageExtractLine from " << args_info.input_arg << std::endl
     << "# \t firstIndex = " << firstIndex << std::endl
     << "# \t lastIndex  = " << lastIndex << std::endl
     << "# \t nb values  = " << values.size() << std::endl
     << "# \t length   = " << length << std::endl
     << "# \t step       = " << step << std::endl;
  if (args_info.depth_flag) {
    double lg = -isoDistance;
    for(unsigned int i=0; i<values.size(); i++) {
      os << lg << " " << values[i] << std::endl;
      lg += step;
    }
    os.close();
  }
  else {
    for(unsigned int i=0; i<values.size(); i++) {
      os << values[i] << std::endl;
    }
    os.close();
  }

  // this is the end my friend  
  return 0;
} // end main

#endif //define CLITKIMAGEEXTRACTLINE_CXX
