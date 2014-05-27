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
#ifndef CLITKDVHGENERICFILTER_H
#define CLITKDVHGENERICFILTER_H

// clitk include
#include "clitkIO.h"
#include "clitkCommon.h"
#include "clitkImageCommon.h" 
#include "clitkImageToImageGenericFilter.h"
#include "clitkDVH_ggo.h"

// itk include
#include "itkImage.h"
#include "itkImageIOBase.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkLightObject.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkLabelGeometryImageFilter.h"

//--------------------------------------------------------------------
namespace clitk
{

//template<class args_info_type>
class ITK_EXPORT DVHGenericFilter: public itk::LightObject
{

public:


    //--------------------------------------------------------------------
    typedef DVHGenericFilter         Self;
    typedef itk::LightObject               Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;

    //--------------------------------------------------------------------
    // Method for creation through the object factory
    // and Run-time type information (and related methods)
    itkNewMacro(Self);
    itkTypeMacro(DVHGenericFilter, LightObject);

    //--------------------------------------------------------------------
    void SetArgsInfo(const args_info_clitkDVH & a)
    {
     m_ArgsInfo=a;
     m_Verbose=m_ArgsInfo.verbose_flag;

     if(m_ArgsInfo.input_given)
      m_InputFileName=m_ArgsInfo.input_arg[0];
     else if(m_ArgsInfo.inputs_num>0)
     m_InputFileName=m_ArgsInfo.inputs[0];
     else {
     std::cerr << "You must give an input file name" << std::endl;
     exit(1);
      }
    }
    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    //template<class InputImageType>
    //void UpdateWithInputImageType();
    void Update();

    // Set methods
    void SetDefaultPixelValue (double value) {  mDefaultPixelValue = value ;}
    void SetTypeOfOperation (int value) {  mTypeOfOperation = value ;}
    void SetScalar (double value) {  mScalar = value ;}
    void EnableOverwriteInputImage(bool b);

    // Get methods
    double GetDefaultPixelValue () { return  mDefaultPixelValue ;}
    int GetTypeOfOperation () { return  mTypeOfOperation ;}
    double GetScalar () { return  mScalar ;}

protected:

   //--------------------------------------------------------------------
   // Constructor & Destructor
   DVHGenericFilter();
   ~DVHGenericFilter() {};
   // template<unsigned int Dim> void InitializeImageType();
   // args_info_type mArgsInfo;

   //--------------------------------------------------------------------
   // Tempated members
   //--------------------------------------------------------------------
   template<unsigned int Dimension, unsigned int Components> void UpdateWithDim(std::string PixelType);
   template <unsigned int Dimension, class PixelType, unsigned int Components> void UpdateWithDimAndPixelType();

   //--------------------------------------------------------------------
   // Data members
   //--------------------------------------------------------------------
   args_info_clitkDVH m_ArgsInfo;
   bool m_Verbose;
   std::string m_InputFileName;

   template<unsigned int Dim> void InitializeImageType();
   bool mIsOperationUseASecondImage;
   double mScalar;
   double mDefaultPixelValue;
   int mTypeOfOperation;
   bool mOverwriteInputImage;
   bool mOutputIsFloat;

   template<class Iter1, class Iter2> void ComputeImage(Iter1 it, Iter2 ito);

   template<class Iter1, class Iter2, class Iter3> void ComputeImage(Iter1 it1, Iter2 it2, Iter3 ito);


}; // end class
//--------------------------------------------------------------------

} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkDVHGenericFilter.txx"
#endif

#endif // #define clitkDVHGenericFilter_h
