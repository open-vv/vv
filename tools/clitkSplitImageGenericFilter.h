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
#ifndef clitkSplitImageGenericFilter_H
#define clitkSplitImageGenericFilter_H
/**
 -------------------------------------------------------------------
 * @file   clitkSplitImageGenericFilter.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   23 Feb 2008
 -------------------------------------------------------------------*/

// clitk include
#include "clitkIO.h"
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
    void SetPng (const bool v) { m_Png = v; }
    void SetWindowLevel(const double w, const double l){ m_Window = w; m_Level = l;}
    void SetMha (const bool v) { m_mha = v; }
    void SetNii (const bool v) { m_nii = v; }
    void SetNiigz (const bool v) { m_niigz = v; }

   //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>  
    void UpdateWithInputImageType();

  protected:  
    template <class ImageType>
    class PngConversion
    {
    public:
      typedef typename ImageType::Pointer ImagePointer;
      typedef itk::Image< unsigned char, ImageType::ImageDimension > OutputPngImageType;
      typedef typename OutputPngImageType::Pointer OutputPngImagePointer;
      OutputPngImagePointer Do(double window, double level, ImagePointer input);
    private:
      template<unsigned int> struct PixelDimType {};
      template<unsigned int Dim> OutputPngImagePointer Do(double window,
                                                          double level,
                                                          ImagePointer input,
                                                          PixelDimType<Dim> *);
      OutputPngImagePointer Do(double window, double level, ImagePointer input, PixelDimType<1> *);
    };

    template<unsigned int Dim> void InitializeImageType();
    int  mSplitDimension;
    bool m_Verbose;
    bool m_Png;
    double m_Window, m_Level;
    bool m_mha;
    bool m_nii;
    bool m_niigz;

  }; // end class SplitImageGenericFilter
//--------------------------------------------------------------------

} // end namespace
//--------------------------------------------------------------------

#endif //#define clitkSplitImageGenericFilter_H

