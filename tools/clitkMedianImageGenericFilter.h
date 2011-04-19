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
===========================================================================*/
/*=========================================================================

  Program:   clitk
  Module:    $RCSfile: clitkMedianImageGenericFilter.h,v $
  Language:  C++
  Date:      $Date: 2010/07/23 14:56:21 $
  Version:   $Revision: 1.1 $
  Author :  Bharath Navalpakkam <Bharath.Navalpakkam@creatis.insa-lyon.fr>
  Copyright (C) 2010
  Léon Bérard cancer center    http://www.centreleonberard.fr
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

#ifndef CLITKMEDIANIMAGEGENERICFILTER_H
#define CLITKMEDIANIMAGEGENERICFILTER_H

// clitk include
#include "clitkIO.h"
#include "clitkCommon.h"
#include "clitkImageToImageGenericFilter.h"

//--------------------------------------------------------------------
namespace clitk 
{
  
  template<class args_info_type>
  class ITK_EXPORT MedianImageGenericFilter: 
    public ImageToImageGenericFilter<MedianImageGenericFilter<args_info_type> >
  {
    
  public:

    //--------------------------------------------------------------------
   MedianImageGenericFilter();

    //--------------------------------------------------------------------
    typedef MedianImageGenericFilter         Self;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    //--------------------------------------------------------------------
    // Method for creation through the object factory
    // and Run-time type information (and related methods)
    itkNewMacro(Self);  
    itkTypeMacro(MedianImageGenericFilter, LightObject);
  
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
#include "clitkMedianImageGenericFilter.txx"
#endif

#endif // #define clitkMedianImageGenericFilter_h
