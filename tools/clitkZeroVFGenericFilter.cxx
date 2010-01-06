#ifndef CLITKINVERTVFGENERICFILTER_CXX
#define CLITKINVERTVFGENERICFILTER_CXX

//clitk include
#include "clitkZeroVFGenericFilter.h"


namespace clitk {

  clitk::ZeroVFGenericFilter::ZeroVFGenericFilter()
  {
    m_Verbose=false;
  }


  void clitk::ZeroVFGenericFilter::Update()
  {
    //Get the image Dimension and PixelType
    int Dimension;
    std::string PixelType;

    clitk::ReadImageDimensionAndPixelType(m_InputName, Dimension, PixelType);

    if(Dimension==2) UpdateWithDim<2>(PixelType);
    else if(Dimension==3) UpdateWithDim<3>(PixelType);
    else 
      {
	std::cout<<"Error, Only for 2 and 3 Dimensions!!!"<<std::endl ;
	return;
      }

  }
} //end namespace

#endif
