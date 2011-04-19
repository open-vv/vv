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

#ifndef __clitkGenericInterpolator_h
#define __clitkGenericInterpolator_h

//itk include
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunctionWithLUT.h"


/*

Requires at least the following section is the .ggo file

option "interp"		-	"Interpolation: 0=NN, 1=Linear, 2=BSpline, 3=BLUT"	int  	no  default="1"
option "interpOrder"	-	"Order if BLUT or BSpline (0-5)"		        int  	no  default="3"
option "interpSF"	-	"Sampling factor if BLUT"			        int  	no  default="20"

The use will look something like

typedef clitk::GenericInterpolator<InputImageType, double> GenericInterpolatorType;
typename GenericInterpolatorType::Pointer genericInterpolator=GenericInterpolatorType::New();
genericInterpolator->SetArgsInfo(m_ArgsInfo);
typedef itk::InterpolateImageFunction<InputImageType, double> InterpolatorType;
typename InterpolatorType::Pointer interpolator=genericInterpolator->GetInterpolatorPointer();

*/

namespace clitk
{

template <class args_info_type,  class ImageType,  class TCoordRep >
class GenericInterpolator : public itk::LightObject
{
public:
  //==============================================
  typedef GenericInterpolator     Self;
  typedef itk::LightObject     Superclass;
  typedef itk::SmartPointer<Self>            Pointer;
  typedef itk::SmartPointer<const Self>      ConstPointer;

  typedef itk::InterpolateImageFunction<ImageType, TCoordRep> InterpolatorType;
  typedef typename InterpolatorType::Pointer InterpolatorPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  //==============================================
  //Set members
  void SetArgsInfo(args_info_type args_info) {
    m_ArgsInfo= args_info;
    m_Verbose=m_ArgsInfo.verbose_flag;
  }

  //==============================================
  //Get members
  InterpolatorPointer GetInterpolatorPointer(void);

  //==============================================
protected:
  GenericInterpolator();
  ~GenericInterpolator() {};

private:
  args_info_type m_ArgsInfo;
  InterpolatorPointer m_Interpolator;
  bool m_Verbose;
};

} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkGenericInterpolator.txx"
#endif

#endif // #define __clitkGenericInterpolator_h
