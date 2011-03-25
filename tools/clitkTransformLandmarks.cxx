#include "clitkTransformLandmarks_ggo.h"

#include "clitkTransformUtilities.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

typedef itk::Matrix<double, 4, 4> MatrixType;
typedef itk::Point<double, 4> PointType;
typedef std::vector<PointType> PointArrayType;

void read_points(const std::string& fileName, PointArrayType& points);
void transform_points(const PointArrayType& input, const MatrixType& matrix, PointArrayType& output);
void write_points(const std::string& fileName, const PointArrayType& points);

bool verbose = false;

int main(int argc, char** argv)
{
  GGO(clitkTransformLandmarks, args_info);
  verbose = args_info.verbose_flag;

  PointArrayType inputPoints;
  read_points(args_info.input_arg, inputPoints);
  
  MatrixType matrix = clitk::ReadMatrix3D(args_info.matrix_arg);

  PointArrayType outputPoints;
  transform_points(inputPoints, matrix, outputPoints);
  
  write_points(args_info.output_arg, outputPoints);
  return 0;
}

void read_points(const std::string& fileName, PointArrayType& points)
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
  
  int id = 0;
  PointType point;
  point.Fill(1);
  
  std::istringstream linestr; 
  while (!landmarksFile.eof()) {
    std::getline(landmarksFile, line);
    if (verbose) std::cout << "line " << line << std::endl;
    
    if (!line.empty()) {
      linestr.str(line);
      linestr >> id >> point[0] >> point[1] >> point[2];
      
      if (verbose) std::cout << "point " << point << std::endl;
      
      points.push_back(point);
    }
  }
}

void transform_points(const PointArrayType& input, const MatrixType& matrix, PointArrayType& output)
{
  for (size_t i = 0; i < input.size(); i++) {
    output.push_back(matrix * input[i]);
  }
}

void write_points(const std::string& fileName, const PointArrayType& points) 
{
  std::ofstream landmarksFile(fileName.c_str());
  
  landmarksFile << "LANDMARKS1" << std::endl;
  for (size_t i = 0; i < points.size(); i++)
    landmarksFile << i << " " << points[i][0] << " " << points[i][1] << " " << points[i][2] << " " << "0" << " " << std::endl;
}