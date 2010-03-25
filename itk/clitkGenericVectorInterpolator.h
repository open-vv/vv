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
#ifndef __clitkGenericVectorInterpolator_h
#define __clitkGenericVectorInterpolator_h
#include "clitkImageCommon.h"

//itk include
#include "itkVectorNearestNeighborInterpolateImageFunction.h"
#include "itkVectorLinearInterpolateImageFunction.h"
#include "clitkVectorBSplineInterpolateImageFunction.h"
#include "clitkVectorBSplineInterpolateImageFunctionWithLUT.h"

/*

Requires at least the following section is the .ggo file

option "interpVF"		-	"Interpolation: 0=NN, 1=Linear, 2=BSpline, 3=BLUT"	int  	no  default="1"
option "interpVFOrder"	-	"Order if BLUT or BSpline (0-5)"		        int  	no  default="3"
option "interpVFSF"	-	"Sampling factor if BLUT"			        int  	no  default="20"

*/

namespace clitk
{
  
  template <class args_info_type,   class ImageType,  class TCoordRep >  
  class GenericVectorInterpolator : public itk::LightObject
  {
  public:
    //==============================================
    typedef GenericVectorInterpolator     Self;
    typedef itk::LightObject     Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;

    typedef itk::VectorInterpolateImageFunction<ImageType, TCoordRep> InterpolatorType;
    typedef typename InterpolatorType::Pointer InterpolatorPointer;   
     
    /** Method for creation through the object factory. */
    itkNewMacro(Self);  
  
    //============================================== 
    //Set members
    void SetArgsInfo(args_info_type args_info)
    {
      m_ArgsInfo= args_info;
      m_Verbose=m_ArgsInfo.verbose_flag;
    }

    //============================================== 
    //Get members
    InterpolatorPointer GetInterpolatorPointer(void);
    
    //==============================================
  protected:
    GenericVectorInterpolator();
    ~GenericVectorInterpolator() {};

  private:
    args_info_type m_ArgsInfo;
    InterpolatorPointer m_Interpolator;
    bool m_Verbose;
  };

} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkGenericVectorInterpolator.txx"
#endif

#endif // #define __clitkGenericVectorInterpolator_h
