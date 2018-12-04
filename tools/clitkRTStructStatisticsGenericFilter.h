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
#ifndef CLITKRTSTRUCTSTATISTICSGENERICFILTER_H
#define CLITKRTSTRUCTSTATISTICSGENERICFILTER_H
/**
 -------------------------------------------------------------------
 * @file   clitkRTStructStatisticsGenericFilter.h
 * @author Thomas Baudier <thomas.baudier@creatis.insa-lyon.fr>
 * @date   11 Jul 2016 08:37:53

 * @brief
 -------------------------------------------------------------------*/

// clitk include
#include "clitkCommon.h"
#include "clitkImageToImageGenericFilter.h"
#include "clitkRTStructStatistics_ggo.h"

// itk include
#include "itkImage.h"
#include "itkImageIOBase.h"

//--------------------------------------------------------------------
namespace clitk {

  template<class args_info_type>
  class ITK_EXPORT RTStructStatisticsGenericFilter:
    public clitk::ImageToImageGenericFilter<RTStructStatisticsGenericFilter<args_info_type> > {

  public:

    // Constructor
    RTStructStatisticsGenericFilter ();

    // Types
    typedef RTStructStatisticsGenericFilter        Self;
    typedef ImageToImageGenericFilterBase   Superclass;
    typedef itk::SmartPointer<Self>         Pointer;
    typedef itk::SmartPointer<const Self>   ConstPointer;

    // New
    itkNewMacro(Self);



    //--------------------------------------------------------------------
    void SetArgsInfo(const args_info_type & a);

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>
    void UpdateWithInputImageType();

  protected:
    template<unsigned int Dim> void InitializeImageType();
    args_info_type mArgsInfo;

  }; // end class RTStructStatisticsGenericFilter

} // end namespace
//--------------------------------------------------------------------


#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkRTStructStatisticsGenericFilter.txx"
#endif

#endif //#define CLITKRTSTRUCTSTATISTICSGENERICFILTER_H

