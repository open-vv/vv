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

  void Init();
  void Reset();
  template<class TItkImageType> void AddItkImage(TItkImageType *input);
  void AddVtkImage(vtkImageData* input);
  const std::vector<vtkImageData*>& GetVTKImages();
  vtkImageData* GetFirstVTKImageData();
  int GetNumberOfDimensions() const;
  int GetNumberOfSpatialDimensions();
  void GetScalarRange(double* range);
  unsigned long GetActualMemorySize();
  std::vector<double> GetSpacing();
  std::vector<double> GetOrigin() const;
  std::vector<int> GetSize();
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

private:
  vvImage();
  ~vvImage();

  std::vector< ConverterPointer > mItkToVtkConverters;
  std::vector< vtkImageData* > mVtkImages;
  std::vector< vtkSmartPointer<vtkTransform> > mTransform;

  double mTimeOrigin;
  double mTimeSpacing;
  unsigned int mImageDimension;
};
//------------------------------------------------------------------------------

#include "vvImage.txx"

#endif
