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
#ifndef CLITKIMAGEEXTRACTLINE_CXX
#define CLITKIMAGEEXTRACTLINE_CXX
/**
   -------------------------------------------------
   * @file   clitkImageExtractLine.cxx
   * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
   * @date   23 Feb 2008 08:37:53
   * @modified by Loïc Grevillot <Loic.Grevillot@creatis.insa-lyon.fr>
   * @date   10 March 2011
      * Option -I added, in order to integrate plans perpendicular to a line
   
   -------------------------------------------------*/

// clitk include
#include "clitkImageExtractLine_ggo.h"
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkCommon.h"
#include <itkLineConstIterator.h>

//--------------------------------------------------------------------
int main(int argc, char * argv[])
{

  // Init command line
  GGO(clitkImageExtractLine, args_info);
  CLITK_INIT;

  // Declare main types
  typedef float PixelType;
  const unsigned int Dimension=3;
  typedef itk::Image<PixelType, Dimension> ImageType;
  typedef itk::Size<Dimension> SizeType;

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
//   std::vector<double> depth;
//   std::vector<double> values;
//   itk::LineConstIterator<ImageType> iter(input, firstIndex, lastIndex);
//   iter.GoToBegin();
//   while (!iter.IsAtEnd()) {
//     values.push_back(iter.Get());
//     ++iter;
//   }
//   double step = length/values.size();

  std::vector<double> depth;
  std::vector<double> values;
  itk::LineConstIterator<ImageType> iter(input, firstIndex, lastIndex);
  int direction=0;
  
  // args_info.integral_arg=0, so, it does not compute the integral
  if (args_info.integral_arg==0){
    iter.GoToBegin();
    while (!iter.IsAtEnd()) {
      values.push_back(iter.Get());
      ++iter;
    }
  }
  // args_info.integral_arg=1, so, it computes the integral
  else if (lastIndex[0]-firstIndex[0]==0 && lastIndex[1]-firstIndex[1]==0 && lastIndex[2]-firstIndex[2]>0)
    direction=1;
  else if (lastIndex[0]-firstIndex[0]==0 && lastIndex[1]-firstIndex[1]>0 && lastIndex[2]-firstIndex[2]==0)
    direction=2;
  else if (lastIndex[0]-firstIndex[0]>0 && lastIndex[1]-firstIndex[1]==0 && lastIndex[2]-firstIndex[2]==0)
    direction=3;
  else{
    //std::cout<<lastIndex[0]-firstIndex[0]<<"  "<<lastIndex[1]-firstIndex[1]<<"  "<<lastIndex[3]-firstIndex[3]<<std::endl;
    std::cout<<"Index are not defined along a straight along x or y or z axis."<<std::endl;
    std::cout<<"The line cannot be extracted."<<std::endl;
    exit(0);
  }
  
  if (args_info.integral_arg!=0){
    SizeType dim;
    dim=input->GetLargestPossibleRegion().GetSize();
    DD(dim);
    DD(direction);
    
    int a=0, b=0, c=0;
    
    if (direction==2){
      a=0;
      b=1;
      c=2;
    }
    if (direction==1){
      a=0;
      b=2;
      c=1;
    }    
    if (direction==3){
      a=2;
      b=0;
      c=1;
    } 
    
    double val[dim[b]];
      for (int i=0; i<dim[b]; i++)
	val[i]=0;
      
    int k;
    for (int i=0; i<dim[a]; i++){
      for (int j=0; j<dim[c]; j++){
  //      std::cout<<"i "<<i<<"  j "<<j<<std::endl;
	k=0;
	firstIndex[a]=i;
	firstIndex[c]=j;
	lastIndex[a]=i;
	lastIndex[c]=j;
  //      std::cout<<"A"<<std::endl;
	itk::LineConstIterator<ImageType> iter(input, firstIndex, lastIndex);
	iter.GoToBegin();
  //      std::cout<<"B"<<std::endl;
	val[k]+=iter.Get();
	k++;
  //      std::cout<<"C"<<std::endl;
	while (!iter.IsAtEnd()) {
  //	std::cout<<"D "<<k<<std::endl;
	  val[k]+=iter.Get();
	  ++iter;
	  k++;
	}
      }
    }
  
    for (unsigned int i=0; i<dim[b]; i++){
      values.push_back(val[i]);
    }
  }
  
  double step = length/values.size();
  DD(values.size());
  
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
  } else {
    for(unsigned int i=0; i<values.size(); i++) {
      os << values[i] << std::endl;
    }
    os.close();
  }

  // this is the end my friend
  return 0;
} // end main

#endif //define CLITKIMAGEEXTRACTLINE_CXX
