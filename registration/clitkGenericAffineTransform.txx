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

#ifndef __clitkGenericAffineTransform_txx
#define __clitkGenericAffineTransform_txx

#include "clitkTransformUtilities.h"

namespace clitk
{

//=========================================================================================================================
//constructor
template<class args_info_type, class TCoordRep, unsigned int Dimension>
GenericAffineTransform<args_info_type, TCoordRep, Dimension>::GenericAffineTransform()
{
  m_Transform=NULL;
  m_Verbose=false;
}

//=========================================================================================================================
//Get the pointer
template<class args_info_type, class TCoordRep, unsigned int Dimension>
typename GenericAffineTransform<args_info_type, TCoordRep, Dimension>::TransformPointer
GenericAffineTransform<args_info_type, TCoordRep, Dimension>::GetTransform()
{
  //============================================================================
  // We retrieve the type of transform from the command line
  //============================================================================
  typedef itk::MatrixOffsetTransformBase<TCoordRep, Dimension, Dimension> MatrixXType;
  typename MatrixXType::Pointer tMatrix;
  typedef itk::TranslationTransform< TCoordRep, Dimension> TranslationXType;
  typename TranslationXType::Pointer tTranslation;

  switch ( m_ArgsInfo.transform_arg ) {
  case 0:
    m_Transform= itk::IdentityTransform< TCoordRep, Dimension>::New();
    if (m_Verbose) std::cout<<"Using identity transform..."<<std::endl;
    break;
  case 1:
    tTranslation = TranslationXType::New();
    if (m_Verbose) std::cout<<"Using translation transform..."<<std::endl;
    break;
  case 2:
    tMatrix = GetNewEulerTransform();
    if (m_Verbose) std::cout<<"Using euler transform..."<<std::endl;
    break;
  case 3:
    tMatrix = itk::AffineTransform< TCoordRep, Dimension >::New();
    if (m_Verbose) std::cout<<"Using affine transform..."<<std::endl;
    break;
  }//end of switch


  typename MatrixXType::OutputVectorType offset;
  if(m_ArgsInfo.transform_arg>0) {
    //Initialize translations
    offset[0] = m_ArgsInfo.transX_arg;
    offset[1] = m_ArgsInfo.transY_arg;
    if(Dimension>2) offset[2] = m_ArgsInfo.transZ_arg;
    if(m_ArgsInfo.initMatrix_given) {
      itk::Matrix<double, Dimension+1 , Dimension+1> matHom = ReadMatrix<Dimension>(m_ArgsInfo.initMatrix_arg);
      offset = GetTranslationPartMatrix(matHom);
    }
  }
  switch(m_ArgsInfo.transform_arg) {
  case 1:
    tTranslation->SetOffset(offset);
    m_Transform=tTranslation;
    break;
  case 2:
  case 3:
    //Init rigid and affine transform center
    itk::Point<TCoordRep, Dimension> center;
    for (unsigned int i=0; i<Dimension; i++)
      center[i] = 0;
    tMatrix->SetCenter(center);

    //Initialize transform
    tMatrix->SetOffset(offset);
    if(m_ArgsInfo.initMatrix_given) {
      itk::Matrix<double, Dimension+1 , Dimension+1> matHom = ReadMatrix<Dimension>(m_ArgsInfo.initMatrix_arg);
      typename MatrixXType::MatrixType matrix = GetRotationalPartMatrix(matHom);
      tMatrix->SetMatrix(matrix);
    }
    m_Transform=tMatrix;
    break;
  }

  return m_Transform;
}
}

#endif
