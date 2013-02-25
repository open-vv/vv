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
#include "clitkMaskLandmarks_ggo.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <clitkCommon.h>
#include <clitkImageCommon.h>

typedef itk::Point<double, 3> PointType;
typedef std::vector<PointType> PointArrayType;

static void read_points_pts(const std::string& fileName, PointArrayType& points, bool verbose)
{
  std::ifstream landmarksFile(fileName.c_str());
  if (landmarksFile.fail())
  {
    std::cerr << "ERROR: could not open '" << fileName << "'" << std::endl;
    exit(-2);
  }

  std::string line;
  std::getline(landmarksFile, line);
  if (line.find("#X") != 0)
  {
    std::cerr << "ERROR: invalid landmarks file '" << fileName << "'" << std::endl;
    exit(-3);
  }

  PointType p;
  while (!landmarksFile.eof())
  {
    landmarksFile >> p[0] >> p[1] >> p[2];
    if (landmarksFile.fail())
      break;
    if (verbose)
      std::cout << "point " << p << std::endl;
    points.push_back(p);
  }
}

void write_points_pts(const std::string& fileName, const PointArrayType& points, bool verbose)
{
  std::ofstream landmarksFile(fileName.c_str());

  landmarksFile << "#X\tY\tZ" << std::endl;
  for (size_t i = 0; i < points.size(); i++)
    landmarksFile << points[i][0] << "\t" << points[i][1] << "\t" << points[i][2] << "\t" << std::endl;
}

int main(int argc, char** argv)
{
  typedef itk::Image<unsigned char, 3>  MaskType;
  typedef MaskType::Pointer             MaskPointer;
  GGO(clitkMaskLandmarks, args_info);

  if (args_info.input_given != args_info.output_given)
  {
    std::cerr << "The number of inputs must be equal to the number of outputs" << std::endl;;
    exit(1);
  }

  std::vector<PointArrayType> inputs(args_info.input_given);
  for (unsigned i = 0; i < args_info.input_given; ++i)
  {
    read_points_pts(args_info.input_arg[i], inputs[i], args_info.verbose_flag);
    if (inputs[i].size() != inputs[0].size())
    {
      std::cerr << "All input files must contain the same amount of points" << std::endl;
      exit(2);
    }
  }

  MaskPointer mask = clitk::readImage<MaskType>(args_info.mask_arg, args_info.verbose_flag);
  std::vector<PointArrayType> outputs(args_info.input_given);
  for (unsigned i = 0; i < inputs[0].size(); ++i)
  {
    MaskType::IndexType idx;
    if (mask->TransformPhysicalPointToIndex(inputs[0][i], idx) && (*mask)[idx])
      for (unsigned j = 0; j < args_info.input_given; j++)
        outputs[j].push_back(inputs[j][i]);
  }

  for (unsigned i = 0; i < args_info.input_given; ++i)
    write_points_pts(args_info.output_arg[i], outputs[i], args_info.verbose_flag);
}
