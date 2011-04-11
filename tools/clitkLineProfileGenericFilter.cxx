#include "clitkLineProfileGenericFilter.h"

namespace clitk
{
  //-----------------------------------------------------------
  // Constructor
  //-----------------------------------------------------------
  LineProfileGenericFilter::LineProfileGenericFilter()
    : ImageToImageGenericFilter<Self>("Resample")
  {
    m_Verbose=false;

    InitializeImageType<2>();
    InitializeImageType<3>();
    //InitializeImageType<4>();
  }
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  template<unsigned int Dim>
  void LineProfileGenericFilter::InitializeImageType() 
  {      
    ADD_DEFAULT_IMAGE_TYPES(Dim);
    //ADD_IMAGE_TYPE(Dim, short);
  }
  //--------------------------------------------------------------------
    
}