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
#ifndef VVIMAGE_H
#define VVIMAGE_H

#include <iostream>
#include <vector>
#include <itkObjectFactory.h>
#include <vtkSmartPointer.h>

class vtkImageData;
class vtkImageReslice;
class vtkTransform;
class vtkImageReslice;
class vtkAbstractTransform;

//------------------------------------------------------------------------------
class vvImage : public itk::LightObject
{
public :
  typedef vvImage Self;
  typedef itk::SmartPointer<Self> Pointer;
  itkNewMacro(Self);

  void Init();
  void Reset();
  void SetImage(std::vector<vtkImageData*> images);
  void AddImage(vtkImageData* image);
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
  bool IsTimeSequence();
  bool IsScalarTypeInteger();
  bool IsScalarTypeInteger(int t);
  vtkSmartPointer<vtkTransform> GetTransform();
  void SetTransform(vtkSmartPointer<vtkTransform> transform);
  void SetTimeSpacing(double s) { mTimeSpacing = s; }
  void SetTimeOrigin(double o) { mTimeOrigin = o; }
  void UpdateReslice();

private:
  vvImage();
  ~vvImage();

  std::vector<vtkImageData*> mVtkImages;
  std::vector< vtkSmartPointer<vtkImageReslice> > mVtkImageReslice;
  vtkSmartPointer<vtkTransform> mTransform;
  double mTimeOrigin;
  double mTimeSpacing;
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vtkImageData * CopyAndCastToFloatFrom(vtkImageData * p);
//------------------------------------------------------------------------------

#endif
