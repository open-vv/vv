#ifndef clitkSplitImageGenericFilter_H
#define clitkSplitImageGenericFilter_H
/**
 -------------------------------------------------------------------
 * @file   clitkSplitImageGenericFilter.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   23 Feb 2008
 -------------------------------------------------------------------*/

// clitk include
#include "clitkCommon.h"
#include "clitkImageCommon.h"
#include "clitkImageToImageGenericFilter.h"

// itk include
#include "itkImage.h"
#include "itkImageIOBase.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  class SplitImageGenericFilter: 
    public clitk::ImageToImageGenericFilter<SplitImageGenericFilter> {
  
  public:
	
    // Constructor 
    SplitImageGenericFilter ();

    // Types
    typedef SplitImageGenericFilter       Self;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    // New
    itkNewMacro(Self);
    
    // Set methods
    void SetSplitDimension (int dim) { mSplitDimension = dim; }
    void SetVerbose (const bool v) { m_Verbose = v; }

   //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>  
    void UpdateWithInputImageType();

  protected:  
    template<unsigned int Dim> void InitializeImageType();
    int  mSplitDimension;
    bool m_Verbose;

  }; // end class SplitImageGenericFilter
//--------------------------------------------------------------------

} // end namespace
//--------------------------------------------------------------------

#endif //#define clitkSplitImageGenericFilter_H

