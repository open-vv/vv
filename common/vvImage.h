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
#ifndef VVIMAGE_H
#define VVIMAGE_H

#include <iostream>
#include <vector>
#include <itkObjectFactory.h>
#include <itkProcessObject.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkCastImageFilter.h>

#define VTK_EXCLUDE_STRSTREAM_HEADERS
#include <vtkSmartPointer.h>
#include <vtkTransform.h>

class vtkImageData;

//------------------------------------------------------------------------------
class vvImage : public itk::LightObject
{
public :
  typedef vvImage Self;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::ProcessObject::Pointer ConverterPointer;
  itkNewMacro(Self);
  
  struct DimensionDispatchBase {};
  template< unsigned int VDimension >
  struct DimensionDispatch:public DimensionDispatchBase {};

  void Init();
  void Reset();
  template<class TItkImageType> void AddItkImage(TItkImageType *input);
  void AddVtkImage(vtkImageData* input);
  const std::vector<vtkImageData*>& GetVTKImages();
  vtkImageData* GetFirstVTKImageData();
  int GetNumberOfDimensions() const;
  int GetNumberOfSpatialDimensions();
  void GetScalarRange(double* range);
  template<class TPixelType, unsigned int VImageDimension> void ComputeScalarRangeBase(itk::Image<TPixelType,VImageDimension> *input);
  template<class TPixelType, unsigned int VImageDimension> void ComputeScalarRange(DimensionDispatchBase, itk::Image<TPixelType,VImageDimension> *input);
  template<class TPixelType, unsigned int VImageDimension> void ComputeScalarRange(DimensionDispatch< 1 >, itk::Image<TPixelType,VImageDimension> *input);
  unsigned long GetActualMemorySize();
  std::vector<double> GetSpacing();
  std::vector<double> GetOrigin() const;
  std::vector<int> GetSize();
  std::vector< std::vector<double> > GetDirection();
  std::string GetScalarTypeAsITKString();
  int GetNumberOfScalarComponents();
  int GetScalarSize();
  bool IsTimeSequence() const;
  bool IsScalarTypeInteger();
  bool IsScalarTypeInteger(int t);
  const std::vector< vtkSmartPointer<vtkTransform> >& GetTransform();
  void SetTimeSpacing(double s) { mTimeSpacing = s; }
  void SetTimeOrigin(double o) { mTimeOrigin = o; }
  bool HaveSameSizeAndSpacingThan(vvImage * other);
  //META DATA
  itk::MetaDataDictionary* GetFirstMetaDataDictionary();

private:
  vvImage();
  ~vvImage();

  std::vector< ConverterPointer > mItkToVtkConverters;
  std::vector< vtkImageData* > mVtkImages;
  std::vector< vtkSmartPointer<vtkTransform> > mTransform;
  //META DATA
  std::vector< itk::MetaDataDictionary* > mDictionary;

  double mTimeOrigin;
  double mTimeSpacing;
  unsigned int mImageDimension;
  double mrange[2];
};
//------------------------------------------------------------------------------

#include "vvImage.txx"

#endif
