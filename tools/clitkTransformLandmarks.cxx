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
#include "clitkTransformLandmarks_ggo.h"

#include "clitkTransformUtilities.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include "vtkPolyDataReader.h"
#include "vtkPolyDataWriter.h"
#include <vtkTransform.h>
#include <vtkTransformFilter.h>

typedef itk::Matrix<double, 4, 4> MatrixType;
typedef itk::Point<double, 4> PointType;
typedef std::vector<PointType> PointArrayType;
typedef itk::FixedArray<std::string, 2> TxtDataType;
typedef std::vector<TxtDataType> TxtDataArrayType;


void read_points_txt(const std::string& fileName, PointArrayType& points, TxtDataArrayType& data);
void write_points_txt(const std::string& fileName, const PointArrayType& points, const TxtDataArrayType& data);

void read_points_pts(const std::string& fileName, PointArrayType& points);
void write_points_pts(const std::string& fileName, const PointArrayType& points);

void apply_spacing(const PointArrayType& input, const double* spacing, PointArrayType& output);
void transform_points(const PointArrayType& input, const MatrixType& matrix, PointArrayType& output);


bool verbose = false;

int main(int argc, char** argv)
{
  GGO(clitkTransformLandmarks, args_info);
  verbose = args_info.verbose_flag;

  TxtDataArrayType data;
  PointArrayType inputPoints;
  if (strcmp(args_info.type_arg, "txt") == 0) {
    read_points_txt(args_info.input_arg, inputPoints, data);
  }
  else if (strcmp(args_info.type_arg, "vtk") == 0) {
    vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
    reader->SetFileName(args_info.input_arg);
    reader->Update();
    vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
    writer->SetFileName( args_info.output_arg );

    if (args_info.matrix_given) {
      vtkSmartPointer<vtkTransformFilter> transformFilter = vtkSmartPointer<vtkTransformFilter>::New();
      vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
      vtkMatrix4x4* matrix = clitk::ReadVTKMatrix3D(args_info.matrix_arg);
      vtkSmartPointer<vtkMatrix4x4> matrixT = vtkSmartPointer<vtkMatrix4x4>::New();
      vtkMatrix4x4::Invert(matrix, matrixT); //not sure why, but this seems necessary for using the same .mat as when loading file with vv (probably due to the inversion trick performed in the vv reader...)
      transform->SetMatrix(matrixT);
      transformFilter->SetInputConnection(reader->GetOutputPort());
      transformFilter->SetTransform(transform);
      writer->SetInputConnection(transformFilter->GetOutputPort());

    }
    else { //just write the output
      writer->SetInputConnection( reader->GetOutputPort() );
    }

    writer->Write();
    return 0;
  }
  else {
    read_points_pts(args_info.input_arg, inputPoints);
  }
  
  PointArrayType outputPoints;
  PointArrayType spacingPoints;
  PointArrayType* workingInputPoints = &inputPoints;
  PointArrayType* workingOutputPoints = &outputPoints;
  if (args_info.spacing_given) {
    if (verbose) std::cout << "Processing spacing..." << std::endl;
    
    apply_spacing(*workingInputPoints, args_info.spacing_arg, spacingPoints);
    workingInputPoints = &spacingPoints;
    workingOutputPoints = &spacingPoints;
  }

  MatrixType matrix;
  if (args_info.matrix_given) {
    matrix = clitk::ReadMatrix3D(args_info.matrix_arg);
    transform_points(*workingInputPoints, matrix, outputPoints);
    workingOutputPoints = &outputPoints;
  }

  if (strcmp(args_info.type_arg, "txt") == 0) {
    write_points_txt(args_info.output_arg, *workingOutputPoints, data);
  }
  else {
    write_points_pts(args_info.output_arg, *workingOutputPoints);
  }
  
  return 0;
}

void read_points_txt(const std::string& fileName, PointArrayType& points, TxtDataArrayType& data)
{
  std::ifstream landmarksFile(fileName.c_str());
  if (landmarksFile.fail()) {
    std::cerr << "ERROR: could not open '" << fileName << "'" << std::endl;
    exit(-2);
  }
  
  std::string line;
  std::getline(landmarksFile, line);
  if (line.find("LANDMARKS") == std::string::npos) {
    std::cerr << "ERROR: invalid landmarks file '" << fileName << "'" << std::endl;
    exit(-3);
  }
  
  PointType p;
  p.Fill(1);
  
  TxtDataType d;
  
  while (!landmarksFile.eof()) {
    // read id, x, y, z, d1, d2
    landmarksFile >> d[0] >> p[0] >> p[1] >> p[2] >> d[1];// >> d[2];
    if (landmarksFile.fail())
      break;
    
    if (verbose){
      std::cout << "point " << p << std::endl;
      std::cout << "data " << " " << d[0] << " " << d[1] << std::endl;
    }
    
    points.push_back(p);
    data.push_back(d);
  }
}

void write_points_txt(const std::string& fileName, const PointArrayType& points, const TxtDataArrayType& data) 
{
  std::ofstream landmarksFile(fileName.c_str());
  
  landmarksFile << "LANDMARKS1" << std::endl;
  for (size_t i = 0; i < points.size(); i++)
    landmarksFile << data[i][0] << " " << points[i][0] << " " << points[i][1] << " " << points[i][2] << " " << data[i][1] << " " << std::endl;
}

void read_points_pts(const std::string& fileName, PointArrayType& points)
{
  std::ifstream landmarksFile(fileName.c_str());
  if (landmarksFile.fail()) {
    std::cerr << "ERROR: could not open '" << fileName << "'" << std::endl;
    exit(-2);
  }
  
  std::string line;
  std::getline(landmarksFile, line);
  if (line.find("#X") != 0) {
    std::cerr << "ERROR: invalid landmarks file '" << fileName << "'" << std::endl;
    exit(-3);
  }
  
  PointType p;
  p.Fill(1);
  
  while (!landmarksFile.eof()) {
    // read id, x, y, z, d1, d2
    landmarksFile >> p[0] >> p[1] >> p[2];
    if (landmarksFile.fail())
      break;
    
    if (verbose){
      std::cout << "point " << p << std::endl;
    }
    
    points.push_back(p);
  }
}

void write_points_pts(const std::string& fileName, const PointArrayType& points)
{
  std::ofstream landmarksFile(fileName.c_str());
  
  landmarksFile << "#X\tY\tZ" << std::endl;
  for (size_t i = 0; i < points.size(); i++)
    landmarksFile << points[i][0] << "\t" << points[i][1] << "\t" << points[i][2] << "\t" << std::endl;
}

void apply_spacing(const PointArrayType& input, const double* spacing, PointArrayType& output)
{
  PointType out;
  out.Fill(1);
  
  for (size_t i = 0; i < input.size(); i++) {
    out[0] = input[i][0] * spacing[0];
    out[1] = input[i][1] * spacing[1];
    out[2] = input[i][2] * spacing[2];
    if (verbose){
      std::cout << "output " << out << std::endl;
    }
    output.push_back(out);
  }
}

void transform_points(const PointArrayType& input, const MatrixType& matrix, PointArrayType& output)
{
  for (size_t i = 0; i < input.size(); i++) {
    output.push_back(matrix * input[i]);
  }
}

