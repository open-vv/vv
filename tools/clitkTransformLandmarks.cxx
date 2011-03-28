#include "clitkTransformLandmarks_ggo.h"

#include "clitkTransformUtilities.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

typedef itk::Matrix<double, 4, 4> MatrixType;
typedef itk::Point<double, 4> PointType;
typedef std::vector<PointType> PointArrayType;
typedef itk::FixedArray<std::string, 2> TxtDataType;
typedef std::vector<TxtDataType> TxtDataArrayType;


void read_points_txt(const std::string& fileName, PointArrayType& points, TxtDataArrayType& data);
void write_points_txt(const std::string& fileName, const PointArrayType& points, const TxtDataArrayType& data);

void read_points_pts(const std::string& fileName, PointArrayType& points);
void write_points_pts(const std::string& fileName, const PointArrayType& points);

void transform_points(const PointArrayType& input, const MatrixType& matrix, PointArrayType& output);


bool verbose = false;

int main(int argc, char** argv)
{
  GGO(clitkTransformLandmarks, args_info);
  verbose = args_info.verbose_flag;

  MatrixType matrix = clitk::ReadMatrix3D(args_info.matrix_arg);

  TxtDataArrayType data;
  PointArrayType inputPoints;
  if (strcmp(args_info.type_arg, "txt") == 0) {
    read_points_txt(args_info.input_arg, inputPoints, data);
  }
  else {
    read_points_pts(args_info.input_arg, inputPoints);
  }
  
  PointArrayType outputPoints;
  transform_points(inputPoints, matrix, outputPoints);

  if (strcmp(args_info.type_arg, "txt") == 0) {
    write_points_txt(args_info.output_arg, outputPoints, data);
  }
  else {
    write_points_pts(args_info.output_arg, outputPoints);
  }
  
  return 0;
}

void read_points_txt(const std::string& fileName, PointArrayType& points, TxtDataArrayType& data)
{
  std::ifstream landmarksFile(fileName.c_str());
  if (landmarksFile.fail()) {
    std::cout << "ERROR: could not open '" << fileName << "'" << std::endl;
    exit(-2);
  }
  
  std::string line;
  std::getline(landmarksFile, line);
  if (line.find("LANDMARKS") == std::string::npos) {
    std::cout << "ERROR: invalid landmarks file '" << fileName << "'" << std::endl;
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
    std::cout << "ERROR: could not open '" << fileName << "'" << std::endl;
    exit(-2);
  }
  
  std::string line;
  std::getline(landmarksFile, line);
  if (line.find("#X") != 0) {
    std::cout << "ERROR: invalid landmarks file '" << fileName << "'" << std::endl;
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

void transform_points(const PointArrayType& input, const MatrixType& matrix, PointArrayType& output)
{
  for (size_t i = 0; i < input.size(); i++) {
    output.push_back(matrix * input[i]);
  }
}

