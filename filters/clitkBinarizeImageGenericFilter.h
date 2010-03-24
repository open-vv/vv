/*=========================================================================

  Program:   clitk
  Module:    $RCSfile: clitkBinarizeImageGenericFilter.h,v $
  Language:  C++
  Date:      $Date: 2010/03/24 10:48:09 $
  Version:   $Revision: 1.3 $
  Author :   Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
             David Sarrut (david.sarrut@creatis.insa-lyon.fr)

  Copyright (C) 2008
  Léon Bérard cancer center    http://oncora1.lyon.fnclcc.fr
  CREATIS                      http://www.creatis.insa-lyon.fr

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, version 3 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  =========================================================================*/

#ifndef CLITKBINARIZEIMAGEGENERICFILTER_H
#define CLITKBINARIZEIMAGEGENERICFILTER_H

// clitk include
#include "clitkIO.h"
#include "clitkImageToImageGenericFilter.h"

//--------------------------------------------------------------------
namespace clitk 
{
  
  template<class args_info_type>
  class ITK_EXPORT BinarizeImageGenericFilter: 
    public ImageToImageGenericFilter<BinarizeImageGenericFilter<args_info_type> >
  {
    
  public:

    //--------------------------------------------------------------------
    BinarizeImageGenericFilter();

    //--------------------------------------------------------------------
    typedef BinarizeImageGenericFilter         Self;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    //--------------------------------------------------------------------
    // Method for creation through the object factory
    // and Run-time type information (and related methods)
    itkNewMacro(Self);  
    itkTypeMacro(BinarizeImageGenericFilter, LightObject);

    //--------------------------------------------------------------------
    void SetArgsInfo(const args_info_type & a);

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>  
    void UpdateWithInputImageType();

  protected:
    template<unsigned int Dim> void InitializeImageType();
    args_info_type mArgsInfo;
    
  }; // end class
  //--------------------------------------------------------------------
    
} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkBinarizeImageGenericFilter.txx"
#endif

#endif // #define clitkBinarizeImageGenericFilter_h
