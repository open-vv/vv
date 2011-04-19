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

#ifndef __clitkGenericAffineTransform_h
#define __clitkGenericAffineTransform_h


//itk include
#include <itkIdentityTransform.h>
#include <itkTranslationTransform.h>
#include <itkAffineTransform.h>

/*

Requires at least the following section is the .ggo file

option "transform"  - "Type: 0=Identity, 1=Translation, 2=Rigid, 3=Affine" int no  default="2"
option "transX"     x "1-3: Initial translation in mm along the X axis"  float no  default="0.0"
option "transY"     y "1-3: Initial translation in mm along the Y axis"  float no  default="0.0"
option "transZ"     z "1-3: Initial translation in mm along the Z axis"  float no  default="0.0"
option "initMatrix" - "1-3: Initial matrix (reference to object space) filename " string  no
option "moment"     - "1-3: Initialize translation by aligning the center of gravities for the respective intensities" flag off

The use will look something like

  typedef clitk::GenericAffineTransform<args_info_clitkAffineRegistration, TCoordRep, Dimension > GenericAffineTransformType;
  typename GenericAffineTransformType::Pointer genericAffineTransform = GenericAffineTransformType::New();
  genericAffineTransform->SetArgsInfo(m_ArgsInfo);
  typedef itk::Transform< double, Dimension, Dimension > TransformType;
  typename TransformType::Pointer transform = genericAffineTransform->GetTransform();
*/

namespace clitk
{
template <class args_info_type, class TCoordRep, unsigned int Dimension>
class GenericAffineTransform : public itk::LightObject
{
public:
  //==============================================
  typedef GenericAffineTransform     Self;
  typedef itk::LightObject     Superclass;
  typedef itk::SmartPointer<Self>            Pointer;
  typedef itk::SmartPointer<const Self>      ConstPointer;

  typedef itk::Transform< TCoordRep, Dimension, Dimension> TransformType;
  typedef typename TransformType::Pointer TransformPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  //==============================================
  //Set members
  void SetArgsInfo(args_info_type args_info) {
    m_ArgsInfo = args_info;
    m_Verbose = m_ArgsInfo.verbose_flag;
  }

  //==============================================
  //Get members
  TransformPointer GetTransform(void);

  //==============================================
protected:
  GenericAffineTransform();
  ~GenericAffineTransform() {};

  typename itk::MatrixOffsetTransformBase<TCoordRep, Dimension, Dimension>::Pointer GetNewEulerTransform();

private:
  args_info_type m_ArgsInfo;
  TransformPointer m_Transform;
  bool m_Verbose;
};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkGenericAffineTransform.txx"
#endif

#endif // #define __clitkGenericAffineTransform_h
