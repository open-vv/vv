#ifndef VVIMAGE_H
#define VVIMAGE_H
#include <iostream>
#include <vector>
#include <itkObjectFactory.h>

class vtkImageData;

class vvImage : public itk::LightObject {
public :
  typedef vvImage Self;
  typedef itk::SmartPointer<Self> Pointer;
  itkNewMacro(Self);

  void Init();
  void SetImage(std::vector<vtkImageData*> images);
  void AddImage(vtkImageData* image);
  const std::vector<vtkImageData*>& GetVTKImages();
  vtkImageData* GetFirstVTKImageData();
  
  int GetNumberOfDimensions() const;
  int GetNumberOfSpatialDimensions();
  ///Writes the scalar range to the provided array, which must have room for two doubles
  void GetScalarRange(double* range);
  unsigned long GetActualMemorySize();
  std::vector<double> GetSpacing();
  std::vector<double> GetOrigin() const;
  std::vector<int> GetSize();
  std::string GetScalarTypeAsString();
  int GetNumberOfScalarComponents();
  int GetScalarSize();
  bool IsTimeSequence();
  bool IsScalarTypeInteger();
  bool IsScalarTypeInteger(int t);

private:
  vvImage();
  ~vvImage();
  std::vector<vtkImageData*> mVtkImages;

};

#endif
